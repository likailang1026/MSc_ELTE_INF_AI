// AffineTransformations.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

cv::Mat createTransformation(float angle, float tx, float ty, float sx, float sy, float skew_factor, float persp1, float persp2)
{
	// Rotation
	cv::Mat R = cv::Mat::eye(3, 3, CV_32F);
	R.at<float>(0, 0) = cos(angle);
	R.at<float>(0, 1) = -sin(angle);
	R.at<float>(1, 0) = sin(angle);
	R.at<float>(1, 1) = cos(angle);

	// Translation
	cv::Mat t = cv::Mat::eye(3, 3, CV_32F);
	t.at<float>(0, 2) = tx;
	t.at<float>(1, 2) = ty;

	// Scaling
	cv::Mat Scale = cv::Mat::eye(3, 3, CV_32F);
	Scale.at<float>(0, 0) = sx;
	Scale.at<float>(1, 1) = sy;

	// Skewing
	cv::Mat Skew = cv::Mat::eye(3, 3, CV_32F);
	Skew.at<float>(0, 1) = skew_factor;

	cv::Mat T = t * R * Skew * Scale;

	// Perspective transformation
	T.at<float>(2, 0) = persp1;
	T.at<float>(2, 1) = persp2;

	return T;
}

cv::Vec3b bilinearInterpolation(const cv::Mat& img, float x, float y)
{
	int x1 = floor(x);
	int y1 = floor(y);
	int x2 = ceil(x);
	int y2 = ceil(y);

	// 确保 x2 和 y2 也在图像范围内
	if (x1 >= img.rows || x2 >= img.rows || y1 >= img.cols || y2 >= img.cols || x1 < 0 || y1 < 0)
		return cv::Vec3b(0, 0, 0);  // 超出边界返回黑色

	cv::Vec3b P1 = img.at<cv::Vec3b>(x1, y1);
	cv::Vec3b P2 = img.at<cv::Vec3b>(x1, y2);
	cv::Vec3b P3 = img.at<cv::Vec3b>(x2, y1);
	cv::Vec3b P4 = img.at<cv::Vec3b>(x2, y2);

	float fx = x - x1;
	float fy = y - y1;

	// 双线性插值计算
	cv::Vec3b interp = (1 - fx) * (1 - fy) * P1 + (1 - fx) * fy * P2 + fx * (1 - fy) * P3 + fx * fy * P4;
	return interp;
}


cv::Mat applyTransformation(cv::Mat& img, cv::Mat T, bool isPerspective)
{
	cv::Mat newImg = cv::Mat::zeros(img.size(), CV_8UC3);
	int HEIGHT = img.size().height;
	int WIDTH = img.size().width;

	for (int i = 0; i < HEIGHT; ++i)
	{
		for (int j = 0; j < WIDTH; ++j)
		{
			cv::Mat p(3, 1, CV_32F);
			p.at<float>(0, 0) = i;
			p.at<float>(1, 0) = j;
			p.at<float>(2, 0) = 1.f;

			cv::Mat newP = T * p;
			float newZ = 1.f;
			if (isPerspective)
			{
				newZ = newP.at<float>(2, 0);
			}

			float newX = newP.at<float>(0, 0) / newZ;
			float newY = newP.at<float>(1, 0) / newZ;

			if (0 <= newX && newX < HEIGHT && 0 <= newY && newY < WIDTH)
			{
				newImg.at<cv::Vec3b>(i, j) = bilinearInterpolation(img, newX, newY);
			}
		}
	}

	return newImg;
}


int main()
{
	cv::Mat img = cv::imread("T:\\opencv\\sources\\samples\\data\\apple.jpg");

	if (img.empty())
	{
		std::cout << "无法读取图像！" << std::endl;
		return 0;
	}

	float angle = 0.0f, tx = 0.0f, ty = 0.0f, sx = 1.0f, sy = 1.0f, skew = 0.0f, persp1 = 0.0f, persp2 = 0.0f;
	bool paramsChanged = true;

	while (true)
	{
		if (paramsChanged)
		{
			cv::Mat T = createTransformation(angle, tx, ty, sx, sy, skew, persp1, persp2);
			cv::Mat newImg = applyTransformation(img, T, true);
			cv::imshow("Transformations", newImg);
			paramsChanged = false;
		}

		// 等待按键输入（延迟50毫秒）
		char key = cv::waitKey(5);

		// 根据按键更新变换参数
		switch (key)
		{
		case 'w': tx += 5; paramsChanged = true; break;  // 向上移动
		case 's': tx -= 5; paramsChanged = true; break;  // 向下移动
		case 'a': ty += 5; paramsChanged = true; break;  // 向左移动
		case 'd': ty -= 5; paramsChanged = true; break;  // 向右移动
		case 'q': angle -= 0.05f; paramsChanged = true; break; // 逆时针旋转
		case 'e': angle += 0.05f; paramsChanged = true; break; // 顺时针旋转
		case 'z': sx += 0.05f; sy += 0.05f; paramsChanged = true; break; // 缩放增大
		case 'x': sx -= 0.05f; sy -= 0.05f; paramsChanged = true; break; // 缩放减小
		case 'r': skew += 0.01f; paramsChanged = true; break; // 水平倾斜右移
		case 'f': skew -= 0.01f; paramsChanged = true; break; // 水平倾斜左移
		case 27: return 0; // 按下Esc键退出
		}
	}

	return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

/*
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>


int main()
{
    std::string image_path =
        "T:\\OpenCV-4.6.0\\opencv\\sources\\samples\\data\\starry_night.jpg";
    cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR);

    if (img.empty())
    {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }

    cv::imshow("Display window", img);

    int k = cv::waitKey(0); // Wait for a keystroke in the window
    if (k == 's')
    {
        cv::imwrite("starry_night.png", img);
    }

    return 0;
}
*/

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

#define WIDTH 800
#define HEIGHT 600
cv::Mat image;
int xCoord, yCoord;

int main()
{
    image = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);

    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);

    cv::imshow("Display window", image);

    int key = cv::waitKey(0);

    return 0;
}

#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <fstream>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main() {
    // 图片路径和文件名
    string folderPath = "C:/Users/moonb/Desktop/picture_group_2/";
    vector<string> imageFiles = { "DSCF8688.jpg", "DSCF8689.jpg", "DSCF8690.jpg" }; // 替换为实际图片名

    // 加载图片
    vector<Mat> images;
    for (const string& file : imageFiles) {
        Mat img = imread(folderPath + file);
        if (img.empty()) {
            cout << "无法加载图片: " << file << endl;
            return -1;
        }
        images.push_back(img);
    }

    if (images.size() < 2) {
        cout << "需要至少两张图片进行拼接。" << endl;
        return -1;
    }

    Mat panorama = images[0]; // 初始化拼接结果为第一张图片

    // 初始化特征检测器（SIFT）
    Ptr<SIFT> detector = SIFT::create(800);

    // 创建输出文件以保存匹配点
    ofstream coordinatesFile("C:/Users/moonb/Desktop/matching_points_4.csv");
    coordinatesFile << "Image1_X,Image1_Y,Image2_X,Image2_Y\n";

    for (size_t i = 1; i < images.size(); i++) {
        cout << "正在拼接第 " << i + 1 << " 张图片..." << endl;

        // **1. 特征点检测和描述**
        vector<KeyPoint> keypoints1, keypoints2;
        Mat descriptors1, descriptors2;
        detector->detectAndCompute(panorama, noArray(), keypoints1, descriptors1);
        detector->detectAndCompute(images[i], noArray(), keypoints2, descriptors2);

        // **2. 特征点匹配**
        BFMatcher matcher(NORM_L2);
        vector<DMatch> matches;
        matcher.match(descriptors1, descriptors2, matches);

        // **3. 筛选匹配点 - 比例测试**
        vector<DMatch> goodMatches;
        for (size_t j = 0; j < matches.size() - 1; j++) {
            if (matches[j].distance < 0.7 * matches[j + 1].distance) {
                goodMatches.push_back(matches[j]);
            }
        }

        // 提取匹配点坐标
        vector<Point2f> points1, points2;
        for (const auto& match : goodMatches) {
            points1.push_back(keypoints1[match.queryIdx].pt);
            points2.push_back(keypoints2[match.trainIdx].pt);

            // 保存匹配点到 CSV 文件
            coordinatesFile << keypoints1[match.queryIdx].pt.x << ","
                << keypoints1[match.queryIdx].pt.y << ","
                << keypoints2[match.trainIdx].pt.x << ","
                << keypoints2[match.trainIdx].pt.y << "\n";
        }

        if (points1.size() < 4 || points2.size() < 4) {
            cout << "匹配点不足，无法计算单应性矩阵。" << endl;
            return -1;
        }

        // **4. 计算单应性矩阵**
        Mat H = findHomography(points2, points1, RANSAC);

        // **5. 透视变换并动态扩展画布**
        Mat warped;
        warpPerspective(images[i], warped, H, Size(panorama.cols + images[i].cols, panorama.rows));

        // 创建足够大的画布以容纳新的拼接结果
        Size canvasSize(max(panorama.cols, warped.cols), max(panorama.rows, warped.rows));
        Mat expandedCanvas(canvasSize, panorama.type(), Scalar::all(0));

        // 将现有的拼接图像复制到画布上
        Mat roi1(expandedCanvas, Rect(0, 0, panorama.cols, panorama.rows));
        panorama.copyTo(roi1);

        // 将新图像叠加到画布上
        Mat roi2(expandedCanvas, Rect(0, 0, warped.cols, warped.rows));
        warped.copyTo(roi2, warped);

        panorama = expandedCanvas; // 更新拼接结果
    }

    // 关闭文件
    coordinatesFile.close();

    // **6. 裁剪多余部分**
    Rect cropRect(0, 0, panorama.cols, panorama.rows);
    panorama = panorama(cropRect);

    // **7. 保存拼接结果到桌面**
    string desktopPath = "C:/Users/moonb/Desktop/panorama_result_4.jpg"; // 替换"moonb"为你的用户名
    imwrite(desktopPath, panorama);

    // **8. 显示拼接结果**
    namedWindow("Panorama", WINDOW_NORMAL);
    resizeWindow("Panorama", 800, 600);
    imshow("Panorama", panorama);

    cout << "拼接完成，结果已保存到: " << desktopPath << endl;
    cout << "匹配点坐标已保存到: C:/Users/moonb/Desktop/matching_points.csv" << endl;

    waitKey(0);
    return 0;
}

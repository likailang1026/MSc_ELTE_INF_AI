#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;
using namespace cv;

// 校准参数
Mat cameraMatrix = (Mat_<double>(3, 3) << 1296.0177, 0, 940.7268, 0, 1294.8322, 583.7191, 0, 0, 1);
Mat distCoeffs = Mat::zeros(1, 5, CV_64F); // 无畸变
Mat R = (Mat_<double>(3, 3) << 0.9511, 0.0237, 0.3080, -0.3082, 0.0065, 0.9513, 0.0205, -0.9997, 0.0135);
Mat t = (Mat_<double>(3, 1) << -0.0022, -0.0792, -0.1151);

// 文件路径
string coordinatesFile = "C:\\Users\\moonb\\Desktop\\poster_coordinates.txt";
string pointCloudFolder = "C:\\Users\\moonb\\Desktop\\3D_CV\\20230530\\20230530\\20230530_ELTEkorV2_RTK_cartesians";
string imageFolder = "C:\\Users\\moonb\\Desktop\\3D_CV\\20230530\\20230530\\20230530_ELTEkorV2_RTK_pictures";
string videoOutputPath = "C:\\Users\\moonb\\Desktop\\output_video_10.avi";

// 加载角点文件
vector<vector<Point2f>> loadCorners(const string& filepath) {
    ifstream file(filepath);
    vector<vector<Point2f>> allCorners;
    string line;

    while (getline(file, line)) {
        istringstream iss(line);
        vector<Point2f> corners;
        int frameIndex;
        iss >> frameIndex;
        for (int i = 0; i < 4; i++) {
            float x, y;
            iss >> x >> y;
            corners.emplace_back(x, y);
        }
        allCorners.push_back(corners);
    }

    return allCorners;
}

// 加载点云文件
vector<Point3f> loadPointCloud(const string& filepath) {
    ifstream file(filepath);
    vector<Point3f> points;
    string line;

    while (getline(file, line)) {
        istringstream iss(line);
        float x, y, z;
        iss >> x >> y >> z;
        points.emplace_back(x, y, z);
    }

    return points;
}

// 点云投影到图像
vector<Point2f> projectPointCloud(const vector<Point3f>& points, const Mat& K, const Mat& R, const Mat& t) {
    vector<Point2f> projectedPoints;

    for (const auto& point : points) {
        Mat point3D = (Mat_<double>(3, 1) << point.x, point.y, point.z);
        Mat transformedPoint = R * point3D + t;

        if (transformedPoint.at<double>(2) > 0) { // Z > 0
            Mat uvw = K * transformedPoint;
            float u = uvw.at<double>(0) / uvw.at<double>(2);
            float v = uvw.at<double>(1) / uvw.at<double>(2);
            projectedPoints.emplace_back(u, v);
        }
    }

    return projectedPoints;
}

// 应用顺时针旋转角度到旋转矩阵
Mat applyRotation(Mat R, double angle_deg) {
    // 将角度转换为弧度
    double angle_rad = angle_deg * CV_PI / 180.0;
    // 构造旋转矩阵 R_rotate
    Mat R_rotate = (Mat_<double>(3, 3) <<
        cos(angle_rad), -sin(angle_rad), 0,
        sin(angle_rad), cos(angle_rad), 0,
        0, 0, 1);

    // 组合旋转矩阵
    return R_rotate * R;
}

// 应用中心对称操作
Mat applySymmetry(Mat R) {
    // 构造中心对称矩阵
    Mat R_symmetry = (Mat_<double>(3, 3) <<
        -1, 0, 0,
        0, -1, 0,
        0, 0, -1);

    // 组合对称矩阵
    return R_symmetry * R;
}

// 应用竖直中线对称操作
Mat applyVerticalSymmetry(Mat R) {
    // 构造竖直中线对称矩阵
    Mat R_vertical_symmetry = (Mat_<double>(3, 3) <<
        -1, 0, 0,
        0, 1, 0,
        0, 0, 1);

    // 组合对称矩阵
    return R_vertical_symmetry * R;
}

// 主程序
int main() {
    // 应用顺时针旋转15度
    double angle = 15.0; // 顺时针
    R = applyRotation(R, angle);

    // 应用中心对称操作
    R = applySymmetry(R);

    // 应用竖直中线对称操作
    R = applyVerticalSymmetry(R);

    // 加载角点文件
    vector<vector<Point2f>> corners = loadCorners(coordinatesFile);

    // 视频输出设置
    VideoWriter videoWriter(videoOutputPath, VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, Size(1920, 1200));

    if (!videoWriter.isOpened()) {
        cerr << "Error: Could not open video writer." << endl;
        return -1;
    }

    for (size_t i = 0; i < corners.size(); ++i) {
        string imagePath = imageFolder + "\\Dev0_Image_w1920_h1200_fn" + to_string(722 + i) + ".jpg";
        string pointCloudPath = pointCloudFolder + "\\test_fn" + to_string(722 + i) + ".xyz";

        Mat image = imread(imagePath);
        if (image.empty()) {
            cerr << "Error: Could not load image " << imagePath << endl;
            continue;
        }

        vector<Point3f> pointCloud = loadPointCloud(pointCloudPath);
        if (pointCloud.empty()) {
            cerr << "Error: Could not load point cloud " << pointCloudPath << endl;
            continue;
        }

        // 投影点云到图像
        vector<Point2f> projectedPoints = projectPointCloud(pointCloud, cameraMatrix, R, t);

        // 绘制投影点
        for (const auto& pt : projectedPoints) {
            circle(image, pt, 3, Scalar(255, 0, 0), FILLED);
        }

        // 写入到视频
        videoWriter.write(image);
    }

    videoWriter.release();
    cout << "Video saved to " << videoOutputPath << endl;

    return 0;
}

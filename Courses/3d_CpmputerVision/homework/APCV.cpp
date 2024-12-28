#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>

// 定义一个结构体用于存储点云数据
struct PointCloud {
    std::vector<cv::Point3f> points;
    std::vector<cv::Vec3b> colors;  // 用于存储有颜色的点云
};

// 读取 .PLY 文件
PointCloud readPLYFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    bool headerParsed = false;
    int numVertices = 0;
    bool hasColor = false;

    PointCloud pointCloud;

    // 解析头文件
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        if (line.substr(0, 14) == "element vertex") {
            // 获取顶点数量
            iss >> line >> line >> numVertices;
        }
        else if (line == "property uchar red") {
            hasColor = true;
        }
        else if (line == "end_header") {
            headerParsed = true;
            break;
        }
    }

    // 读取顶点数据
    for (int i = 0; i < numVertices; ++i) {
        float x, y, z;
        unsigned char r = 0, g = 0, b = 0;  // 默认颜色为黑色

        file >> x >> y >> z;

        if (hasColor) {
            int ir, ig, ib;
            file >> ir >> ig >> ib;
            r = static_cast<unsigned char>(ir);
            g = static_cast<unsigned char>(ig);
            b = static_cast<unsigned char>(ib);
        }

        pointCloud.points.emplace_back(cv::Point3f(x, y, z));
        if (hasColor) {
            pointCloud.colors.emplace_back(cv::Vec3b(b, g, r));  // OpenCV 使用 BGR 颜色格式
        }
    }

    return pointCloud;
}

// 使用 cv::projectPoints 投影 3D 点到 2D 平面，并在图像上绘制
void projectAndDrawPoints(const PointCloud& pointCloud, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
    const cv::Mat& rvec, const cv::Mat& tvec, cv::Mat& outputImage) {
    // 存储投影后的 2D 点
    std::vector<cv::Point2f> imagePoints;

    // 将 3D 点投影到 2D 图像平面
    cv::projectPoints(pointCloud.points, rvec, tvec, cameraMatrix, distCoeffs, imagePoints);

    // 调试输出：打印投影后的 2D 点坐标
    for (size_t i = 0; i < imagePoints.size(); ++i) {
        float x = imagePoints[i].x;
        float y = imagePoints[i].y;

        std::cout << "Projected point " << i << ": (" << x << ", " << y << ")" << std::endl;

        if (x >= 0 && x < outputImage.cols && y >= 0 && y < outputImage.rows) {
            cv::circle(outputImage, imagePoints[i], 1, cv::Scalar(0, 255, 0), -1);  // 将点的半径设为 5
        }
    }
}

int main() {
    // 读取 .PLY 文件
    PointCloud pointCloud = readPLYFile("C:\\Users\\moonb\\Desktop\\3D_CV\\Advanced Point Cloud Visualization\\PLYfiles\\sphereCalibScan.ply");  // 修改为你的 .PLY 文件路径

    // 定义相机内参矩阵 (调整焦距和光心)
    cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << 1000, 0, 750, 0, 1000, 500, 0, 0, 1);  // 调整焦距和图像中心
    cv::Mat distCoeffs = cv::Mat::zeros(4, 1, CV_64F);  // 无畸变

    // 定义旋转向量和平移向量 (可以修改为实际值)
    cv::Mat rvec = (cv::Mat_<double>(3, 1) << 0, 0, 0);  // 无旋转
    cv::Mat tvec = (cv::Mat_<double>(3, 1) << 0, 0, 500);  // 将点云移到相机前方 500 单位

    // 创建更大尺寸的输出图像
    cv::Mat outputImage = cv::Mat::zeros(1000, 1500, CV_8UC3);  // 调整图像分辨率

    // 缩放点云数据
    for (auto& point : pointCloud.points) {
        point.x *= 5;
        point.y *= 5;
        point.z *= 5;  // 将点云坐标放大 5 倍
    }

    // 将 3D 点投影并绘制到图像
    projectAndDrawPoints(pointCloud, cameraMatrix, distCoeffs, rvec, tvec, outputImage);

    // 显示结果
    cv::imshow("Projected Point Cloud", outputImage);
    cv::waitKey(0);

    return 0;
}

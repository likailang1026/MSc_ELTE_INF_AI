#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>

// ����һ���ṹ�����ڴ洢��������
struct PointCloud {
    std::vector<cv::Point3f> points;
    std::vector<cv::Vec3b> colors;  // ���ڴ洢����ɫ�ĵ���
};

// ��ȡ .PLY �ļ�
PointCloud readPLYFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    bool headerParsed = false;
    int numVertices = 0;
    bool hasColor = false;

    PointCloud pointCloud;

    // ����ͷ�ļ�
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        if (line.substr(0, 14) == "element vertex") {
            // ��ȡ��������
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

    // ��ȡ��������
    for (int i = 0; i < numVertices; ++i) {
        float x, y, z;
        unsigned char r = 0, g = 0, b = 0;  // Ĭ����ɫΪ��ɫ

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
            pointCloud.colors.emplace_back(cv::Vec3b(b, g, r));  // OpenCV ʹ�� BGR ��ɫ��ʽ
        }
    }

    return pointCloud;
}

// ʹ�� cv::projectPoints ͶӰ 3D �㵽 2D ƽ�棬����ͼ���ϻ���
void projectAndDrawPoints(const PointCloud& pointCloud, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
    const cv::Mat& rvec, const cv::Mat& tvec, cv::Mat& outputImage) {
    // �洢ͶӰ��� 2D ��
    std::vector<cv::Point2f> imagePoints;

    // �� 3D ��ͶӰ�� 2D ͼ��ƽ��
    cv::projectPoints(pointCloud.points, rvec, tvec, cameraMatrix, distCoeffs, imagePoints);

    // �����������ӡͶӰ��� 2D ������
    for (size_t i = 0; i < imagePoints.size(); ++i) {
        float x = imagePoints[i].x;
        float y = imagePoints[i].y;

        std::cout << "Projected point " << i << ": (" << x << ", " << y << ")" << std::endl;

        if (x >= 0 && x < outputImage.cols && y >= 0 && y < outputImage.rows) {
            cv::circle(outputImage, imagePoints[i], 1, cv::Scalar(0, 255, 0), -1);  // ����İ뾶��Ϊ 5
        }
    }
}

int main() {
    // ��ȡ .PLY �ļ�
    PointCloud pointCloud = readPLYFile("C:\\Users\\moonb\\Desktop\\3D_CV\\Advanced Point Cloud Visualization\\PLYfiles\\sphereCalibScan.ply");  // �޸�Ϊ��� .PLY �ļ�·��

    // ��������ڲξ��� (��������͹���)
    cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << 1000, 0, 750, 0, 1000, 500, 0, 0, 1);  // ���������ͼ������
    cv::Mat distCoeffs = cv::Mat::zeros(4, 1, CV_64F);  // �޻���

    // ������ת������ƽ������ (�����޸�Ϊʵ��ֵ)
    cv::Mat rvec = (cv::Mat_<double>(3, 1) << 0, 0, 0);  // ����ת
    cv::Mat tvec = (cv::Mat_<double>(3, 1) << 0, 0, 500);  // �������Ƶ����ǰ�� 500 ��λ

    // ��������ߴ�����ͼ��
    cv::Mat outputImage = cv::Mat::zeros(1000, 1500, CV_8UC3);  // ����ͼ��ֱ���

    // ���ŵ�������
    for (auto& point : pointCloud.points) {
        point.x *= 5;
        point.y *= 5;
        point.z *= 5;  // ����������Ŵ� 5 ��
    }

    // �� 3D ��ͶӰ�����Ƶ�ͼ��
    projectAndDrawPoints(pointCloud, cameraMatrix, distCoeffs, rvec, tvec, outputImage);

    // ��ʾ���
    cv::imshow("Projected Point Cloud", outputImage);
    cv::waitKey(0);

    return 0;
}

#include <opencv2/opencv.hpp>
#include <vector>
#include <random>
#include <cmath>

// 生成带噪声的随机圆上的数据点
std::vector<cv::Point2f> generateCircleData(cv::Point2f center, float radius, int numPoints, float noise, int numOutliers)
{
    std::vector<cv::Point2f> points;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> angle_dist(0, 2 * CV_PI);
    std::uniform_real_distribution<> noise_dist(-noise, noise);
    std::uniform_real_distribution<> outlier_dist(-radius * 2, radius * 2);

    // 生成在圆上的点（带噪声）
    for (int i = 0; i < numPoints; ++i)
    {
        float angle = angle_dist(gen);
        float x = center.x + radius * cos(angle) + noise_dist(gen);
        float y = center.y + radius * sin(angle) + noise_dist(gen);
        points.emplace_back(cv::Point2f(x, y));
    }

    // 生成离群点（不在圆上）
    for (int i = 0; i < numOutliers; ++i)
    {
        float x = center.x + outlier_dist(gen);
        float y = center.y + outlier_dist(gen);
        points.emplace_back(cv::Point2f(x, y));
    }

    return points;
}

// 使用三个点拟合一个圆并返回圆心和半径
cv::Vec3f fitCircleFromPoints(const cv::Point2f& p1, const cv::Point2f& p2, const cv::Point2f& p3)
{
    float x1 = p1.x, y1 = p1.y;
    float x2 = p2.x, y2 = p2.y;
    float x3 = p3.x, y3 = p3.y;

    float A = x1 * (y2 - y3) - y1 * (x2 - x3) + (x2 * y3 - x3 * y2);
    if (std::abs(A) < 1e-6) return cv::Vec3f(0, 0, 0);  // 共线的情况，无法拟合

    float B = (x1 * x1 + y1 * y1) * (y3 - y2) + (x2 * x2 + y2 * y2) * (y1 - y3) + (x3 * x3 + y3 * y3) * (y2 - y1);
    float C = (x1 * x1 + y1 * y1) * (x2 - x3) + (x2 * x2 + y2 * y2) * (x3 - x1) + (x3 * x3 + y3 * y3) * (x1 - x2);
    float D = (x1 * x1 + y1 * y1) * (x3 * y2 - x2 * y3) + (x2 * x2 + y2 * y2) * (x1 * y3 - x3 * y1) + (x3 * x3 + y3 * y3) * (x2 * y1 - x1 * y2);

    float centerX = -B / (2 * A);
    float centerY = -C / (2 * A);
    float radius = std::sqrt((B * B + C * C - 4 * A * D) / (4 * A * A));

    return cv::Vec3f(centerX, centerY, radius);
}

// 使用RANSAC来找到最佳拟合圆
cv::Vec3f ransacCircleFit(const std::vector<cv::Point2f>& points, int maxIterations, float threshold)
{
    int bestInliers = 0;
    cv::Vec3f bestCircle;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, points.size() - 1);

    for (int i = 0; i < maxIterations; ++i)
    {
        // 随机选择三个点
        cv::Point2f p1 = points[dist(gen)];
        cv::Point2f p2 = points[dist(gen)];
        cv::Point2f p3 = points[dist(gen)];

        // 拟合圆
        cv::Vec3f circle = fitCircleFromPoints(p1, p2, p3);
        if (circle[2] == 0) continue;  // 忽略共线情况

        // 计算内点个数
        int inliers = 0;
        for (const auto& p : points)
        {
            float distToCircle = std::hypot(p.x - circle[0], p.y - circle[1]) - circle[2];
            if (std::abs(distToCircle) < threshold) inliers++;
        }

        // 更新最优拟合圆
        if (inliers > bestInliers)
        {
            bestInliers = inliers;
            bestCircle = circle;
        }
    }

    return bestCircle;
}

void visualize(const std::vector<cv::Point2f>& points, const cv::Vec3f& circle)
{
    // 创建窗口和画布
    int size = 800;
    cv::Mat canvas(size, size, CV_8UC3, cv::Scalar(255, 255, 255));

    // 绘制数据点
    for (const auto& point : points)
    {
        cv::circle(canvas, point, 3, cv::Scalar(0, 0, 255), -1);
    }

    // 绘制拟合圆
    cv::Point2f center(circle[0], circle[1]);
    float radius = circle[2];
    cv::circle(canvas, center, radius, cv::Scalar(0, 255, 0), 2);

    // 显示结果
    cv::imshow("Circle Fitting with RANSAC", canvas);
    cv::waitKey(0);
}

int main()
{
    // 生成随机圆的数据
    cv::Point2f center(400, 400);
    float radius = 200;
    std::vector<cv::Point2f> points = generateCircleData(center, radius, 100, 10.0f, 20);

    // 使用RANSAC拟合圆
    cv::Vec3f fittedCircle = ransacCircleFit(points, 1000, 5.0f);

    // 可视化结果
    visualize(points, fittedCircle);

    return 0;
}

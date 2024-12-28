#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;
using namespace cv;

// 全局变量
vector<Point2f> selectedPoints; // 用户点击的点
int currentImageIndex = 0;      // 当前图片索引
vector<string> imageNames;      // 图片名称列表
string imageFolder = "C:\\Users\\moonb\\Desktop\\3D_CV\\20230530\\20230530\\20230530_ELTEkorV2_RTK_pictures";
string outputFile = "C:\\Users\\moonb\\Desktop\\poster_coordinates.txt"; // 输出文件路径
Mat currentImage;

// 鼠标点击事件回调函数
void onMouse(int event, int x, int y, int, void*) {
    if (event == EVENT_LBUTTONDOWN) {
        // 记录点击的点
        selectedPoints.emplace_back(x, y);
        cout << "Point selected: (" << x << ", " << y << ")" << endl;

        // 在图片上绘制点击的点
        circle(currentImage, Point(x, y), 5, Scalar(0, 0, 255), FILLED);
        imshow("Select Poster Corners", currentImage);

        // 如果四个点选定，保存并加载下一张图片
        if (selectedPoints.size() == 4) {
            ofstream file(outputFile, ios::app); // 以追加模式打开文件
            if (file.is_open()) {
                file << currentImageIndex << " ";
                for (const auto& point : selectedPoints) {
                    file << point.x << " " << point.y << " ";
                }
                file << endl;
                file.close();
                cout << "Points saved for image " << imageNames[currentImageIndex] << endl;
            }
            else {
                cerr << "Error: Could not open output file!" << endl;
            }

            // 清空点并加载下一张图片
            selectedPoints.clear();
            currentImageIndex++;
            if (currentImageIndex < imageNames.size()) {
                string nextImagePath = imageFolder + "\\" + imageNames[currentImageIndex];
                currentImage = imread(nextImagePath);
                if (currentImage.empty()) {
                    cerr << "Error loading image: " << nextImagePath << endl;
                    destroyAllWindows();
                    exit(-1);
                }
                cout << "Loaded image: " << nextImagePath << endl;
                imshow("Select Poster Corners", currentImage);
            }
            else {
                cout << "All images processed!" << endl;
                destroyAllWindows();
            }
        }
    }
}

int main() {
    // 初始化图片名称列表
    for (int i = 721; i <= 740; i++) {
        string imageName = "Dev0_Image_w1920_h1200_fn" + to_string(i) + ".jpg";
        imageNames.push_back(imageName);
    }

    // 检查图片文件夹路径
    if (imageNames.empty()) {
        cerr << "Error: No images found!" << endl;
        return -1;
    }

    // 加载第一张图片
    string firstImagePath = imageFolder + "\\" + imageNames[currentImageIndex];
    currentImage = imread(firstImagePath);

    if (currentImage.empty()) {
        cerr << "Error loading image: " << firstImagePath << endl;
        return -1;
    }
    else {
        cout << "Successfully loaded first image: " << firstImagePath << endl;
    }

    // 创建窗口并显示第一张图片
    namedWindow("Select Poster Corners", WINDOW_AUTOSIZE);
    imshow("Select Poster Corners", currentImage);

    // 设置鼠标点击事件回调
    setMouseCallback("Select Poster Corners", onMouse);

    // 等待用户输入
    cout << "Click on the four corners of the poster in each image." << endl;
    waitKey(0);

    return 0;
}

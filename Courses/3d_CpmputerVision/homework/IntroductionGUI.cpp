#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

#define WIDTH 800
#define HEIGHT 600
cv::Mat image;
int xCoord, yCoord;

void redraw() {
    cv::rectangle(image, cv::Point(0, 0), cv::Point(WIDTH, HEIGHT), cv::Scalar(0, 0, 0), cv::FILLED);
    cv::rectangle(image, cv::Point(xCoord, yCoord), cv::Point(xCoord + 100, yCoord + 100), cv::Scalar(255, 0, 0), cv::FILLED);
    cv::imshow("Display window", image);
}

void MouseCallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        xCoord = x;
        yCoord = y;
        redraw();
    }
}

int main()
{
    image = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);
    xCoord = 100;
    yCoord = 100;
    redraw();

    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display window", image);
    cv::setMouseCallback("Display window", MouseCallBackFunc, NULL);


    int key;
    int speed = 10;
    while (true) {
        key = cv::waitKey(100);

        if (key == 27) break;

        switch (key) {
        case 'a':
            xCoord -= speed;
            break;
        case 'd':
            xCoord += speed;
            break;
        case 'w':
            yCoord -= speed;
            break;
        case 's':
            yCoord += speed;
            break;
        }
        redraw();
    }

    return 0;
}



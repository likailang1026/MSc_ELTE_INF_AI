// ball.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <opencv2/opencv.hpp>
#include <iostream>

// Window dimensions
const int width = 640;
const int height = 480;

// Racket properties
const int racket_width = 100;
const int racket_height = 10;
int racket_x = (width - racket_width) / 2;
const int racket_y = height - racket_height - 10;

// Ball properties
const int ball_radius = 10;
int ball_x = width / 2;
int ball_y = height / 2;
int ball_speed_x = 3;
int ball_speed_y = 3;

// Racket movement speed
const int racket_speed = 10;

// Function to draw the game elements
void draw_game(cv::Mat& field, int racket_x, int ball_x, int ball_y) {
    field = cv::Mat::zeros(height, width, CV_8UC3); // Clear the field (black background)

    // Draw racket (white rectangle)
    cv::rectangle(field, cv::Point(racket_x, racket_y), cv::Point(racket_x + racket_width, racket_y + racket_height), cv::Scalar(255, 255, 255), cv::FILLED);

    // Draw ball (green circle)
    cv::circle(field, cv::Point(ball_x, ball_y), ball_radius, cv::Scalar(0, 255, 0), cv::FILLED);
}

// Main game loop
int main() {
    // Create game window
    cv::Mat field(height, width, CV_8UC3);

    while (true) {
        // Draw the game
        draw_game(field, racket_x, ball_x, ball_y);
        cv::imshow("Bouncing Ball Game", field);

        // Ball movement
        ball_x += ball_speed_x;
        ball_y += ball_speed_y;

        // Collision with walls (left, right, top)
        if (ball_x <= ball_radius || ball_x >= width - ball_radius) {
            ball_speed_x = -ball_speed_x; // Reverse direction horizontally
        }
        if (ball_y <= ball_radius) {
            ball_speed_y = -ball_speed_y; // Reverse direction vertically
        }

        // Collision with racket
        if (ball_y + ball_radius >= racket_y && ball_x >= racket_x && ball_x <= racket_x + racket_width) {
            ball_speed_y = -ball_speed_y; // Reverse direction vertically
        }

        // Game over: ball falls past the racket
        if (ball_y >= height) {
            std::cout << "Game Over!" << std::endl;
            break;
        }

        // Racket control with keyboard
        int key = cv::waitKey(30);
        if (key == 27) { // ESC key to exit
            break;
        }
        if (key == 'a' && racket_x > 0) { // Move racket left
            racket_x -= racket_speed;
        }
        if (key == 'd' && racket_x < width - racket_width) { // Move racket right
            racket_x += racket_speed;
        }
    }

    cv::destroyAllWindows();
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

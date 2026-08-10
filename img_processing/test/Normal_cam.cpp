#include <opencv2/opencv.hpp>
#include <iostream>
#include "config.h"

using namespace std;

int main() {
    CameraConfig* cfg = &g_camConfig; // <-- con trỏ chung của bạn

    cv::VideoCapture cap(0);
    cap.set(cv::CAP_PROP_BRIGHTNESS, cfg->brightness);

    if (!cap.isOpened()) {
        cerr << "Cannot open camera!" << endl;
        return -1;
    }

    cout << "Camera is running... 'q'/'ESC' thoat | 'm' doi mode mau | '+/-' brightness | 'd' denoise on/off" << endl;

    cv::Mat frame;

    while (true) {
        cap >> frame;
        if (frame.empty()) {
            cerr << "Cannot receive frame!" << endl;
            break;
        }

        // --- áp dụng config động ---
        cv::resize(frame, frame, cv::Size(cfg->width, cfg->height));

        if (cfg->mirror)
            cv::flip(frame, frame, 1);

        // đổi kênh màu theo config
        cv::Mat colorFrame;
        switch (cfg->colorMode) {
            case ColorMode::GRAY:
                cv::cvtColor(frame, colorFrame, cv::COLOR_BGR2GRAY);
                break;
            case ColorMode::HSV:
                cv::cvtColor(frame, colorFrame, cv::COLOR_BGR2HSV);
                break;
            default:
                colorFrame = frame;
                break;
        }

        if (cfg->denoise)
            cv::medianBlur(colorFrame, colorFrame, 5);

        cv::imshow("Camera", colorFrame);

        int key = cv::waitKey(1);
        if (key == 'q' || key == 27) {
            cout << "quit program." << endl;
            break;
        } else if (key == 'm') {
            cfg->colorMode = static_cast<ColorMode>(
                (static_cast<int>(cfg->colorMode) + 1) % static_cast<int>(ColorMode::COUNT));
        } else if (key == '+') {
            cfg->brightness = std::min(255, cfg->brightness + 10);
            cap.set(cv::CAP_PROP_BRIGHTNESS, cfg->brightness);
        } else if (key == '-') {
            cfg->brightness = std::max(0, cfg->brightness - 10);
            cap.set(cv::CAP_PROP_BRIGHTNESS, cfg->brightness);
        } else if (key == 'd') {
            cfg->denoise = !cfg->denoise;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
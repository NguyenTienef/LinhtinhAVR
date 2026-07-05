#include <opencv2/opencv.hpp>
#include <iostream>
#include "config.h"

using namespace std;

int main() {
    
    // static uint8_t data[100];
    // uint8_t *ws = data;

    
    // Mở camera mặc định (index 0)
    cv::VideoCapture cap(1);

    int value = 255;  // Giá trị độ sáng (0-255)
    cap.set(cv::CAP_PROP_BRIGHTNESS, value);

    if (!cap.isOpened()) {
        cerr << "Cannot open camera!" << endl;
        return -1;
    }

    cout << "Camera is running... Press 'q' to quit" << endl;

    cv::Mat frame;

    while (true) {
        // Đọc frame từ camera
        cap >> frame;

        cv::resize(frame, frame, cv::Size(1366 , 768 )); // Thay đổi kích thước nếu cần

        if (frame.empty()) {
            cerr << "Cannot receive frame!" << endl;
            break;
        }

        cv::flip(frame, frame, 1); // Lật ảnh ngang (mirror)

        // Hiển thị cửa sổ
        cv::Mat denoised;
        cv::medianBlur(frame, denoised, 5);  // kernel size = 5

        cv::imshow("Camera - Normal", frame);
        cv::imshow("Denoised", denoised);

        
        
        if (cv::waitKey(1) == 'q' || cv::waitKey(1) == 27) { // Nhấn 'q' hoặc 'ESC' để thoát
            cout << "quit program." << endl;
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
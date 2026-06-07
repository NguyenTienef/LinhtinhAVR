#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Mở camera mặc định (index 0)
    cv::VideoCapture cap(0);

    if (!cap.isOpened()) {
        std::cerr << "Cannot open camera!" << std::endl;
        return -1;
    }

    std::cout << "Camera is running... Press 'q' to quit" << std::endl;

    cv::Mat frame_bgr, frame_hsv;

    while (true) {
        // Đọc frame từ camera
        cap >> frame_bgr;

        if (frame_bgr.empty()) {
            std::cerr << "Cannot receive frame!" << std::endl;
            break;
        }

        // Chuyển BGR sang HSV
        cv::cvtColor(frame_bgr, frame_hsv, cv::COLOR_BGR2HSV);

        // Hiển thị cả 2 cửa sổ
        cv::imshow("BGR - Original", frame_bgr);
        cv::imshow("HSV - Converted", frame_hsv);

        // Nhấn 'q' để thoát
        if (cv::waitKey(1) == 'q') {
            std::cout << "quit program." << std::endl;
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
#include <opencv2/opencv.hpp>
#include <iostream>


using namespace std;

int main() {
    // Mở camera mặc định (index 0)
    cv::VideoCapture cap(0);

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

        cv::resize(frame, frame, cv::Size(720, 680)); // Thay đổi kích thước nếu cần

        if (frame.empty()) {
            cerr << "Cannot receive frame!" << endl;
            break;
        }

        cv::flip(frame, frame, 1); // Lật ảnh ngang (mirror)

        // Hiển thị cửa sổ
        cv::imshow("Camera - Normal", frame);

        // Nhấn 'q' để thoát
        if (cv::waitKey(1) == 'q') {
            cout << "quit program." << endl;
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
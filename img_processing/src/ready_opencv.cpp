#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    std::cout << "OpenCV version: " << CV_VERSION << std::endl;
    
    cv::Mat img(300, 300, CV_8UC3, cv::Scalar(0, 255, 0));
    cv::putText(img, "OpenCV OK!", cv::Point(50, 150),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
    
    if (img.empty()) {
        std::cout << "ERROR: image is empty!" << std::endl;
        return -1;
    }
    
    std::cout << "Showing image..." << std::endl;
    cv::imshow("Test", img);
    std::cout << "Press any key in the image window" << std::endl;
    cv::waitKey(0);
    return 0;
}
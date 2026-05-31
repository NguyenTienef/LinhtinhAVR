#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    try {
        std::cout << "Starting..." << std::endl;
        std::cout << "OpenCV version: " << CV_VERSION << std::endl;
        
        cv::Mat img(300, 300, CV_8UC3, cv::Scalar(0, 255, 0));
        std::cout << "Image created: " << img.size() << std::endl;
        
        std::cout << "Success!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
}

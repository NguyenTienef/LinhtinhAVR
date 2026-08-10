#include<bits/stdc++.h>
#include<opencv2/opencv.hpp>



int main(){
    cv::VideoCapture cap(0);
    

    if (!cap.isOpened()) {
        std::cerr << "Cannot open camera!" << std::endl;
        return -1;
    }
    std::cout << "Camera is running... Press 'q' to quit" << std::endl;
    cv::Mat frame_bgr;



}

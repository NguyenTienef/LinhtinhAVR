#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Đọc ảnh gốc
    cv::Mat image = cv::imread("STM32_pinout.png");

    if (image.empty()) {
        std::cout << "Khong doc duoc anh!" << std::endl;
        return 1;
    }

    // Vung chua chu "dientunhattung.com"
    // (x, y, width, height) - da xac dinh bang cach quet pixel,
    // tranh dung vao vien den va khoi "Vbat" ben duoi
    cv::Rect watermarkROI(12, 18, 462 - 12, 99 - 18);

    // Cach 1: Fill trang truc tiep (nhanh, sach, phu hop vi nen la trang thuan)
    image(watermarkROI) = cv::Scalar(255, 255, 255);

    // Neu muon an toan hon (phong truong hop bien khong hoan toan trang thuan),
    // co the dung inpainting thay vi fill truc tiep - bo comment doan duoi:
    /*
    cv::Mat mask = cv::Mat::zeros(image.size(), CV_8UC1);
    mask(watermarkROI) = 255;
    cv::Mat result;
    cv::inpaint(image, mask, result, 5, cv::INPAINT_TELEA);
    image = result;
    */

    cv::imwrite("STM32_pinout_no_watermark.png", image);
    cv::save("STM32_pinout_no_watermark.png", image);

    cv::imshow("Ket qua", image);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
#pragma once
#include <cstdint>

enum class ColorMode : uint8_t {
    BGR = 0,
    GRAY,
    HSV,
    COUNT // dùng để cycle qua các mode (không phải mode thật)
};

struct CameraConfig {
    int width      = 1366;
    int height     = 768;
    int brightness = 255;   // 0-255
    bool mirror    = true;
    bool denoise   = true;
    ColorMode colorMode = ColorMode::BGR;
};

// khai báo extern, định nghĩa thật nằm ở config.cpp
extern CameraConfig g_camConfig;
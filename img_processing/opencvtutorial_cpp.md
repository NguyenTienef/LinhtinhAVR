# Hướng Dẫn Cài Đặt OpenCV với MinGW Compiler Trên Windows

## Giới Thiệu
Hướng dẫn này giúp bạn cài đặt OpenCV (thư viện xử lý hình ảnh) để dùng với MinGW 64-bit compiler trên Windows. Chúng tôi sẽ build từ source code để có phiên bản ổn định nhất.

---

## Bước 1: Cài Đặt Các Công Cụ Cần Thiết

Mở MSYS2 MinGW 64-bit terminal và chạy các lệnh sau để cập nhật và cài đặt các công cụ xây dựng:

```bash
pacman -Syu
pacman -S mingw-w64-x86_64-cmake \
          mingw-w64-x86_64-ninja \
          mingw-w64-x86_64-pkg-config
```

**Giải thích:**
- `pacman -Syu`: Cập nhật tất cả package
- `cmake`: Công cụ để configure project
- `ninja`: Công cụ build (tùy chọn, có thể dùng Make)
- `pkg-config`: Công cụ kiểm tra thư viện

---

## Bước 2: Tải Về Source Code OpenCV

Chọn một thư mục để lưu source code (ví dụ `C:\opencv-src`):

```bash
# Clone repository chính của OpenCV
git clone https://github.com/opencv/opencv.git

# (Tùy chọn) Clone các module bổ sung
git clone https://github.com/opencv/opencv_contrib.git
```

Chọn phiên bản ổn định (4.10.0):

```bash
cd opencv
git checkout 4.10.0

# Nếu bạn đã clone opencv_contrib, cũng checkout version
cd ../opencv_contrib
git checkout 4.10.0
```

**Lưu ý:** 
- Nếu không cần `opencv_contrib` (các module bổ sung), bạn có thể bỏ qua phần này
- Sau khi clone xong, bạn sẽ có cấu trúc thư mục như sau:
```
C:\opencv-src\
  ├── opencv\
  └── opencv_contrib\
```

---

## Bước 3: Cấu Hình Build Với CMake

Tạo thư mục `build` và cấu hình:

```bash
mkdir build
cd build
```

Chạy lệnh CMake sau (nếu bạn có `opencv_contrib`):

```bash
cmake -G "MinGW Makefiles" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=C:/opencv \
  -DCMAKE_C_COMPILER=gcc \
  -DCMAKE_CXX_COMPILER=g++ \
  -DBUILD_SHARED_LIBS=ON \
  -DBUILD_TESTS=OFF \
  -DBUILD_PERF_TESTS=OFF \
  -DBUILD_EXAMPLES=OFF \
  -DWITH_OPENMP=ON \
  -DOPENCV_EXTRA_MODULES_PATH=C:/opencv-src/opencv_contrib/modules \
  C:/opencv-src/opencv
```

**Nếu bạn không dùng `opencv_contrib`**, xoá dòng này:
```bash
-DOPENCV_EXTRA_MODULES_PATH=C:/opencv-src/opencv_contrib/modules
```

**Giải thích các tùy chọn:**
- `-G "MinGW Makefiles"`: Sử dụng MinGW Makefiles generator
- `-DCMAKE_BUILD_TYPE=Release`: Build version tối ưu (nhanh)
- `-DCMAKE_INSTALL_PREFIX=C:/opencv`: Nơi cài đặt (thư mục đích)
- `-DBUILD_SHARED_LIBS=ON`: Tạo file `.dll` (thư viện động)
- `-DBUILD_TESTS=OFF`: Không build test (tiết kiệm thời gian)
- `-DWITH_OPENMP=ON`: Hỗ trợ multi-threading

---

## Bước 4: Build OpenCV

Chạy lệnh build (điều chỉnh `-j` theo số lõi CPU của bạn):

```bash
# -j8 = sử dụng 8 lõi CPU (thay đổi con số nếu cần)
cmake --build . --config Release -j8
```

**Thời gian:** Tuỳ vào máy tính, quá trình này có thể mất 15-30 phút.

Sau khi build xong, cài đặt OpenCV:

```bash
cmake --install .
```

**Kết quả:** Thư mục `C:\opencv\` sẽ chứa:
```
C:\opencv\
  ├── include\opencv2\      ← Header files (.h)
  ├── lib\                  ← Import libraries (.a)
  └── bin\                  ← DLL files (.dll)
```

---

## Bước 5: Thêm Đường Dẫn Vào System Environment Variables

Để compiler tìm được thư viện OpenCV, cần thêm `C:\opencv\bin` vào PATH:

1. Mở **Environment Variables**:
   - Nhấn `Win + X` → Chọn **System**
   - Click **Advanced system settings**
   - Click **Environment Variables**

2. Tìm biến **Path** trong phần **System variables** → Click **Edit**

3. Click **New** và thêm: `C:\opencv\bin`

4. Click **OK** để lưu

---

## Bước 6: Cấu Hình VS Code

Tạo 2 file cấu hình trong thư mục `.vscode` của project:

### 6.1 File `c_cpp_properties.json`

File này cấu hình đường dẫn include và compiler cho IntelliSense:

```json
{
  "configurations": [
    {
      "name": "Win32",
      "includePath": [
        "${workspaceFolder}/**",
        "C:/opencv/include"
      ],
      "compilerPath": "C:/msys64/mingw64/bin/g++.exe",
      "cppStandard": "c++17",
      "intelliSenseMode": "windows-gcc-x64"
    }
  ],
  "version": 4
}
```

**Giải thích:**
- `includePath`: Nơi tìm header files (`.h`)
- `compilerPath`: Đường dẫn đến g++ compiler
- `cppStandard`: Phiên bản C++ (c++17 hoặc cao hơn)

### 6.2 File `tasks.json`

File này cấu hình lệnh build (Ctrl+Shift+B):

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "build",
      "type": "shell",
      "command": "g++",
      "args": [
        "${file}",
        "-o", "${fileDirname}/${fileBasenameNoExtension}.exe",
        "-I", "C:/opencv/include",
        "-L", "C:/opencv/lib",
        "-lopencv_core",
        "-lopencv_highgui",
        "-lopencv_imgproc",
        "-lopencv_imgcodecs",
        "-lopencv_videoio",
        "-std=c++17"
      ],
      "group": { "kind": "build", "isDefault": true },
      "problemMatcher": ["$gcc"]
    }
  ]
}
```

**Giải thích:**
- `-I C:/opencv/include`: Nơi tìm header files
- `-L C:/opencv/lib`: Nơi tìm library files
- `-lopencv_core`, `-lopencv_highgui`: Link các thư viện OpenCV cần thiết
- `-o`: Tên file output (`.exe`)

---

## Bước 7: Test Thử

Tạo file `test.cpp` để kiểm tra:

```cpp
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    cout << "OpenCV version: " << CV_VERSION << endl;
    return 0;
}
```

Build bằng `Ctrl+Shift+B` trong VS Code.

---

## Ghi Chú & Khắc Phục Lỗi

### Lỗi: "Cannot find -lopencv_core4100"
- Kiểm tra version OpenCV bạn vừa build
- Xem tên file trong `C:\opencv\lib` để lấy tên đúng
- Cập nhật tên library trong `tasks.json`

### Lỗi: "g++: command not found"
- Kiểm tra đã cài MinGW 64-bit chưa
- Thêm `C:\msys64\mingw64\bin` vào System PATH

### Build chậm?
- Giảm số lõi: `-j4` hoặc `-j6`
- Bỏ `-DWITH_OPENMP=ON` nếu không cần multi-threading

---

## Tóm Tắt Các Bước Chính

1. ✅ Cài MSYS2 extensions
2. ✅ Clone OpenCV source code
3. ✅ Configure CMake
4. ✅ Build và cài đặt
5. ✅ Thêm PATH environment
6. ✅ Cấu hình VS Code
7. ✅ Test thử

Chúc bạn thành công! 🎉
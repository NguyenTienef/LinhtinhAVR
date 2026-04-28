# C/AVR Embedded Systems Project Collection

Một bộ sưu tập các dự án nhúng sử dụng ngôn ngữ C và AVR microcontroller, bao gồm các ứng dụng điều khiển phần cứng, LCD display, xử lý tín hiệu, và các bài toán lập trình.

## 📋 Cấu Trúc Dự Án

```
Cfile/
├── project/                 # Các dự án nhúng AVR chính
│   ├── Blink_led/          # LED blinking (Làm nhấp nháy LED)
│   ├── Lcd1602/            # LCD 1602 Clock (Đồng hồ LCD)
│   └── Motor_control/      # Motor Control (Điều khiển động cơ)
├── review/                 # Bài tập lập trình C++
│   ├── add2num.cpp
│   ├── twosum.cpp
│   └── palindrome_number.cpp
└── README.md
```

---

## 🔧 Dự Án trong `project/`

### 1. **Blink_led** - Làm Nhấp Nháy LED

Dự án cơ bản nhất - LED nhấp nháy với chu kỳ 500ms.

**Thông số kỹ thuật:**
- Microcontroller: ATmega (tùy biến)
- Output: PB5 (LED)
- Chu kỳ: 500ms bật/tắt
- Công nghệ: `util/delay.h` delay

**Cấu trúc:**
```
Blink_led/
├── CMakeLists.txt
├── src/
│   └── main.c
└── build/                  # Build output
```

**Cách build:**
```bash
cd project/Blink_led
mkdir build
cd build
cmake ..
make
```

---

### 2. **Lcd1602** - LCD 1602 Digital Clock

Dự án LCD display hiển thị đồng hồ số trên màn hình LCD 16x2.

**Thông số kỹ thuật:**
- Microcontroller: ATmega (16 MHz)
- Display: LCD 16x2 (I2C/Parallel)
- Features: 
  - Khởi tạo LCD
  - Hiển thị đồng hồ real-time
  - Quản lý timer để update thời gian

**Cấu trúc tệp:**
```
Lcd1602/
├── CMakeLists.txt
├── config.h               # Cấu hình chung
├── toolchain-avr.cmake    # AVR toolchain config
├── include/
│   ├── lcd.h             # LCD driver header
│   └── timer.h           # Timer/Clock header
├── src/
│   ├── main.c            # Main program
│   ├── lcd.c             # LCD implementation
│   └── timer.c           # Timer implementation
├── build.bat / build.ps1 # Windows build scripts
└── build/                # Build output
```

**Cách build:**
```bash
cd project/Lcd1602
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-avr.cmake
make
```

**Hoặc trên Windows:**
```powershell
cd project/Lcd1602
.\build.ps1
```

---

### 3. **Motor_control** - LinhtinhAVR Motor Controller

Dự án điều khiển động cơ DC với phản hồi encoder, LCD display 16x2, và các nút điều khiển.

**Thông số kỹ thuật:**
- **Microcontroller**: ATmega16 @ 16 MHz
- **Display**: LCD 16x2
- **Motor Control**: PWM-based speed control
- **Feedback**: Encoder (INT0 interrupt)
- **Inputs**: 4 push buttons (PB0-PB3)
  - PB0: Tăng tốc độ
  - PB1: Giảm tốc độ
  - PB2: Đảo chiều
  - PB3: Dừng khẩn cấp

**Features:**
- ✅ Điều khiển tốc độ mượt mà
- ✅ Hoạt động hai chiều
- ✅ Giám sát RPM real-time
- ✅ Hiển thị trạng thái trên LCD
- ✅ Dừng khẩn cấp
- ✅ PWM chính xác qua timer hardware
- ✅ Input responsive via hardware interrupts

**Cấu trúc tệp:**
```
Motor_control/
├── CMakeLists.txt
├── README.md             # Tài liệu chi tiết
├── PCB1.PcbDoc          # Sơ đồ PCB custom
├── src/
│   ├── main.c           # Main program
│   ├── lcd.c/.h         # LCD driver
│   └── (thêm các file khác)
└── build/               # Build output
```

**Cách build:**
```bash
cd project/Motor_control
mkdir build
cd build
cmake ..
make
```

---

## 📚 Dự Án trong `review/`

Thư mục này chứa các bài toán lập trình C++ để ôn tập:

### File bao gồm:
- **add2num.cpp** - Cộng hai số
- **twosum.cpp** - Two Sum (tìm cặp số cộng bằng target)
- **palindrome_number.cpp** - Kiểm tra số palindrome
- **CMakeLists.txt** - Build config
- **build.bat / build.ps1** - Windows build scripts

**Cách build:**
```bash
cd review
mkdir build
cd build
cmake ..
make
```

---

## 🛠️ Yêu Cầu Hệ Thống

### Bắt buộc:
- **CMake** >= 3.10
- **Compiler**: 
  - GCC (cho review)
  - AVR-GCC (cho project)
- **Makefile generator** (Make / Ninja)

### Optional:
- **avrdude** - Để upload code lên microcontroller
- **Python** - Nếu sử dụng build scripts advanced

### Cài đặt AVR toolchain:

**Linux/Ubuntu:**
```bash
sudo apt-get install gcc-avr avr-libc cmake make
```

**macOS:**
```bash
brew install avr-gcc avr-libc cmake
```

**Windows:**
- Download từ: https://www.microchip.com/en-us/development-tools-and-ecosystem/development-tools-and-boards
- Hoặc sử dụng package manager như `chocolatey`:
```powershell
choco install avr-gcc
```

---

## 🚀 Quick Start

### Build tất cả dự án:

**Linux/macOS:**
```bash
# Motor Control
cd project/Motor_control/build && make

# LCD Clock
cd ../../Lcd1602/build && make

# Blink LED
cd ../../Blink_led/build && make

# Review problems
cd ../../../review/build && make
```

**Windows (PowerShell):**
```powershell
cd project/Motor_control
.\build.ps1

cd ../Lcd1602
.\build.ps1

cd ../review
.\build.ps1
```

---

## 📝 Ghi Chú

- Các dự án sử dụng **CMake** để hỗ trợ cross-platform
- AVR projects cần **AVR toolchain** đặc biệt
- Review folder chứa các bài toán lập trình thuần C++
- Một số dự án có build scripts sẵn (`.bat` và `.ps1`) cho Windows

---

## 👨‍💻 Cấu Trúc Tiêu Chuẩn

Mỗi dự án tuân theo cấu trúc:
```
project_name/
├── CMakeLists.txt        # Build configuration
├── src/                  # Source files (.c, .cpp)
├── include/              # Header files (.h)
├── build/                # Build output
└── [Platform files]      # build.bat, build.ps1, etc.
```

---

## 📖 Tài Liệu Thêm

Xem file `README.md` chi tiết trong từng thư mục con để biết thêm thông tin.

---

**Cập nhật lần cuối:** April 28, 2026

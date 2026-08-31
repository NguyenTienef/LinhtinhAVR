# C/AVR Embedded Systems Project Collection

This project collection contains embedded applications built with the C language and AVR microcontrollers. It includes hardware control projects, LCD display applications, signal processing examples, and programming practice exercises.

## 📁 Project Structure

```text
Cfile/
├── project/                 # Main AVR embedded projects
│   ├── Blink_led/           # LED blinking project
│   ├── Lcd1602/             # LCD 1602 clock project
│   └── Motor_control/       # Motor control project
├── review/                  # C++ programming exercises
│   ├── add2num.cpp
│   ├── twosum.cpp
│   └── palindrome_number.cpp
├── CMakeLists.txt           # Optional root build configuration
├── build.bat                # Optional Windows build script
├── build.ps1                # Optional PowerShell build script
└── README.md
```

---

## 🔧 Projects in `project/`

### 1. Blink_led — LED Blinking

This is the simplest and most basic AVR project. It makes an LED blink on and off at a fixed interval.

Features:
- AVR microcontroller-based
- Output: PB5
- Blink period: 500ms ON / 500ms OFF
- Built using `util/delay.h`

Project structure:
```text
Blink_led/
├── CMakeLists.txt
├── src/
│   └── main.c
├── build/
└── README.md
```

Build instructions:
```bash
cd project/Blink_led
mkdir build
cd build
cmake ..
make
```

---

### 2. Lcd1602 — LCD 1602 Digital Clock

This project displays a real-time digital clock on a 16x2 LCD. It demonstrates timer usage and hardware display control.

Specifications:
- Microcontroller: ATmega (16 MHz)
- Display: LCD 16x2
- Features:
  - LCD initialization
  - Real-time clock display
  - Timer-based time update
  - Hardware-driven UI output

Project structure:
```text
Lcd1602/
├── CMakeLists.txt
├── config.h
├── toolchain-avr.cmake
├── include/
│   ├── lcd.h
│   └── timer.h
├── src/
│   ├── main.c
│   ├── lcd.c
│   └── timer.c
├── build.bat
├── build.ps1
├── build/
└── README.md
```

Build instructions:
```bash
cd project/Lcd1602
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-avr.cmake
make
```

Windows:
```powershell
cd project/Lcd1602
.\build.ps1
```

---

### 3. Motor_control — AVR Motor Controller

This project is a more advanced embedded system that controls a DC motor with encoder feedback, LCD display, and button inputs.

Specifications:
- Microcontroller: ATmega16 @ 16 MHz
- Display: LCD 16x2
- Motor control: PWM-based speed control
- Feedback: Encoder using hardware interrupt
- Inputs: 4 push buttons
  - PB0: Increase speed
  - PB1: Decrease speed
  - PB2: Reverse direction
  - PB3: Emergency stop

Features:
- Smooth speed control
- Bidirectional motor operation
- Real-time RPM monitoring
- LCD status display
- Emergency stop
- Hardware timer-driven PWM signal generation
- Responsive input handling through interrupts

Project structure:
```text
Motor_control/
├── CMakeLists.txt
├── README.md
├── PCB1.PcbDoc
├── src/
│   ├── main.c
│   ├── lcd.c
│   ├── lcd.h
│   └── ...
├── build/
└── ...
```

Build instructions:
```bash
cd project/Motor_control
mkdir build
cd build
cmake ..
make
```

---

## 📚 Review Projects in `review/`

The `review/` folder contains C++ programming exercises used for training and problem-solving practice.

Included files:
- `add2num.cpp` — Add two numbers
- `twosum.cpp` — Find two numbers whose sum equals a target
- `palindrome_number.cpp` — Check whether a number is a palindrome
- `CMakeLists.txt` — Build configuration
- `build.bat` / `build.ps1` — Windows build scripts

Build instructions:
```bash
cd review
mkdir build
cd build
cmake ..
make
```

---

## 🛠️ System Requirements

### Required tools
- CMake >= 3.10
- Compiler:
  - GCC for review projects
  - AVR-GCC for embedded projects
- Make or Ninja

### Optional tools
- avrdude — for uploading firmware to AVR microcontrollers
- Python — for advanced build scripts
- Git — for version control

### Install AVR toolchain

Ubuntu / Debian:
```bash
sudo apt-get update
sudo apt-get install gcc-avr avr-libc cmake make
```

macOS:
```bash
brew install avr-gcc avr-libc cmake
```

Windows:
- Download from Microchip's official AVR toolchain page:
  https://www.microchip.com/en-us/development-tools-and-ecosystem/development-tools-and-boards
- Or install using Chocolatey:
```powershell
choco install avr-gcc
```

---

## 🚀 Quick Start

### Build all projects manually

Linux / macOS:
```bash
# Motor control
cd project/Motor_control/build && make

# LCD clock
cd ../../Lcd1602/build && make

# Blink LED
cd ../../Blink_led/build && make

# Review problems
cd ../../../review/build && make
```

Windows (PowerShell):
```powershell
cd project/Motor_control
.\build.ps1

cd ../Lcd1602
.\build.ps1

cd ../review
.\build.ps1
```

---

## 📝 Notes

- All projects use CMake for cross-platform build support.
- AVR projects require an AVR-compatible toolchain.
- The `review/` folder contains standard C++ algorithm and problem-solving exercises.
- Some projects include ready-to-use Windows build scripts (`.bat` and `.ps1`).

---

## 👨‍💻 Standard Project Structure

Each project follows a consistent structure:

```text
project_name/
├── CMakeLists.txt
├── src/                  # Source files (.c / .cpp)
├── include/              # Header files (.h)
├── build/                # Build output
├── README.md             # Project documentation
└── build scripts / config files
```

---

## 📖 Additional Documentation

For deeper technical details, refer to the README file inside each project folder.

---

Last updated: August 31, 2026
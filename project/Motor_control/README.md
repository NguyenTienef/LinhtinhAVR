# LinhtinhAVR

A motor controller project built on a custom PCB using the ATmega16 microcontroller. This project provides DC motor speed control with feedback monitoring, LCD display interface, and intuitive control buttons.

## Overview

**LinhtinhAVR** is an embedded systems project designed to control and monitor a DC motor. The system integrates an encoder for real-time speed feedback, a 16x2 character LCD display for status information, and four control buttons for user interaction. The project is built using the AVR toolchain with CMake for cross-platform compilation.

## Features

- **Motor Speed Control**: Smooth speed adjustment with increment and decrement buttons
- **Reverse Operation**: Bidirectional motor control capability
- **Speed Feedback**: Encoder-based real-time RPM monitoring
- **LCD Display**: 16x2 character LCD showing current speed and status
- **Emergency Stop**: Immediate motor shutdown with dedicated button
- **Timer-based Control**: Uses hardware timers for precise PWM duty cycle management
- **Interrupt-driven Input**: Hardware interrupts for responsive button inputs and encoder feedback

## Hardware Requirements

- **Microcontroller**: ATmega16 running at 16 MHz
- **Display**: 16x2 character LCD module
- **Input Controls**: 
  - Speed Up button (PB0)
  - Speed Down button (PB1)
  - Reverse button (PB2)
  - Stop button (PB3)
- **Motor Feedback**: Incremental encoder connected to INT0 (external interrupt)
- **Custom PCB**: Designed and integrated for all components (see `PCB1.PcbDoc`)

## Building the Project

### Prerequisites

Install the AVR development tools on your system:

```bash
# On Ubuntu/Debian
sudo apt-get install gcc-avr avr-libc cmake avrdude

# On macOS (using Homebrew)
brew install avr-gcc avr-libc cmake avrdude
```

### Compilation

1. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

2. Configure and build with CMake:
   ```bash
   cmake -DCMAKE_SYSTEM_NAME=AVR ..
   make
   ```

3. The build process generates:
   - `main.elf` - Compiled ELF binary
   - `main.hex` - Hex file for programming (suitable for AVRDUDE or in-circuit programmers)

### Project Structure

```
LinhtinhAVR/
├── CMakeLists.txt           # Main CMake configuration
├── README.md                # This file
├── PCB1.PcbDoc             # Custom PCB design file
├── src/                     # Main project source files
│   ├── main.c              # Main controller logic with interrupt handlers
│   ├── lcd.h               # LCD driver header
│   └── lcd.c               # LCD driver implementation
├── Blink_led/              # Example LED blinking project
│   ├── CMakeLists.txt
│   └── src/
│       └── main.c          # Simple LED toggle example (PB5)
└── build/                  # Build output directory (generated)
    ├── main.elf            # Compiled binary
    └── main.hex            # Intel HEX format file
```

## Usage

### Control Buttons

- **Tang (Speed Up)**: Press button on PB0 to increase motor speed
- **Giam (Speed Down)**: Press button on PB1 to decrease motor speed
- **Dao (Reverse)**: Press button on PB2 to reverse motor direction
- **Stop**: Press button on PB3 to stop the motor immediately

### LCD Display

The 16x2 character LCD displays:
- Current motor speed in percentage (0-100%)
- RPM calculated from encoder feedback
- Motor status (running, stopped, reversing)

### Motor Speed Calculation

Speed is calculated from encoder counts:
- The system samples encoder counts every 2 seconds (20 overflow cycles at 100ms each)
- Each encoder pulse represents 1/128 revolution
- Formula: `Speed = (encoder_count / 128) × 10`

## Technical Details

### Interrupt Handlers

- **INT0_vect**: External interrupt for encoder pulse counting
- **TIMER0_OVF_vect**: Overflow interrupt triggers every 100ms through 20 cycles, updates speed calculation
- **TIMER2_OVF_vect**: Overflow interrupt at 5-cycle intervals, handles button debouncing and status updates

### Timer Configuration

- **Timer0**: Used for speed calculation and control loop timing
- **Timer2**: Used for debouncing and periodic tasks
- **Prescaler**: CS02 and CS00 bits set for appropriate clock division

### PWM Control

Motor speed is controlled through PWM, with duty cycle adjusted based on user input. The system supports smooth acceleration/deceleration to protect motor and electronics.

## Programming the Microcontroller

Once compiled, program the ATmega16 using AVRDUDE:

```bash
avrdude -p atmega16 -c [programmer] -U flash:w:main.hex:i
```

Replace `[programmer]` with your hardware programmer type (e.g., `usbtiny`, `usbasp`, `stk500`, etc.)

## Example Project

The `Blink_led/` subdirectory contains a simple LED blinking example that toggles PB5 every 500ms. Use this as a reference for basic AVR setup and compilation with CMake.

## Notes

- The project uses volatile variables for interrupt-safe data access
- Button inputs are debounced to avoid false triggering
- Speed percentage is calculated as a percentage of maximum speed
- Encoder resolution affects speed feedback accuracy

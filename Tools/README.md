# Arduino Flash Tool — Python MVP

Windows GUI for flashing Arduino Uno/Nano ATmega328P application firmware over
the existing UART bootloader. The GUI uses Python's built-in Tkinter/ttk; no Qt
or GUI designer is required.

## Features

- Detect and list Windows COM ports with `pyserial`.
- Arduino Uno and Nano ATmega328P profiles.
- Nano new bootloader (115200 baud) and old bootloader (57600 baud).
- Flash `.hex` using `avrdude`.
- Convert `.elf` to Intel HEX using `avr-objcopy` before flashing.
- Optional verification, progress display, process log, and cancellation.
- Save tool paths and user settings under `%APPDATA%\ArduinoFlashTool`.
- Run long operations in a worker thread so the GUI remains responsive.

This MVP flashes an **application through the manufacturer's/existing board
bootloader**. It does not install or replace the bootloader. Bootloader flashing
requires a separate ISP programmer backend and fuse/lock-bit safeguards.

## Run from Python

Install Python 3.11 or newer from python.org. During installation, enable
`Add Python to PATH` and ensure `tcl/tk and IDLE` is selected.

Open Command Prompt in this directory:

```bat
python -m venv .venv
.venv\Scripts\activate
python -m pip install -r requirements.txt
python app.py
```

Alternatively, run `run.bat`.

## Build a standalone EXE

Run:

```bat
build_exe.bat
```

The output is:

```text
dist\ArduinoFlashTool.exe
```

The EXE contains the Python runtime and GUI. It does **not** automatically
contain `avrdude.exe`, `avrdude.conf`, or `avr-objcopy.exe`; select their paths
in the application. They can come from an installed Arduino AVR toolchain.

## Use

1. Connect the Arduino by USB.
2. Close Arduino Serial Monitor and any program using the COM port.
3. Start the application and click **Refresh**.
4. Select the correct Uno/Nano profile and COM port.
5. Select `.hex` or `.elf` application firmware.
6. Select `avrdude.exe` and its matching `avrdude.conf`.
7. For ELF input, also select `avr-objcopy.exe`.
8. Click **Flash application**.

Do not bypass a device-signature mismatch. It normally means that the selected
board/MCU profile is wrong.

## Typical avrdude command

```text
avrdude -C avrdude.conf -v -p atmega328p -c arduino \
  -P COM5 -b 115200 -D -U flash:w:firmware.hex:i
```

If verification is disabled, the program adds `-V`.


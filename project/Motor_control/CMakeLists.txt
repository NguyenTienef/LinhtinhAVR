cmake_minimum_required(VERSION 3.16)

# Set compiler and system BEFORE project()
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)
set(CMAKE_C_COMPILER "C:/msys64/mingw64/bin/avr-gcc.exe")
set(CMAKE_CXX_COMPILER "C:/msys64/mingw64/bin/avr-g++.exe")

# Disable Windows PE linker flags for AVR
set(CMAKE_EXE_LINKER_FLAGS_INIT "")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "")

project(Motor_control C)

set(MCU atmega328p)
set(F_CPU 16000000UL)

set(CMAKE_C_STANDARD 11)
set(CMAKE_OBJCOPY "C:/msys64/mingw64/bin/avr-objcopy.exe")
set(CMAKE_SIZE "C:/msys64/mingw64/bin/avr-size.exe")

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

set(SOURCES
    src/main.c
    src/lcd.c
    # src/timer.c
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -mmcu=${MCU}
    -DF_CPU=${F_CPU}
    -Os
    -Wall
    -Wextra
)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -mmcu=${MCU}
)

add_custom_command(
    TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom
            ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} --mcu=${MCU} --format=avr
            ${PROJECT_NAME}.elf
)
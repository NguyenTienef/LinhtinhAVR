cmake_minimum_required(VERSION 3.13)

project(AVR_MOTOR C)

set(MCU atmega16)
set(F_CPU 16000000UL)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER avr-gcc)

set(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -Os")

include_directories(lib)

file(GLOB SRC
    src/*.c
    lib/*.c
)

add_executable(main.elf ${SRC})

add_custom_command(TARGET main.elf POST_BUILD
    COMMAND avr-objcopy -O ihex -R .eeprom main.elf main.hex
    COMMAND avr-size main.elf
)
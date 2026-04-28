cmake_minimum_required(VERSION 3.13)

project(AVR_LED C)

set(MCU atmega328p)
set(F_CPU 16000000UL)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER avr-gcc)

set(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -Os")

file(GLOB SRC
    src/*.c
)

add_executable(main.elf ${SRC})

add_custom_command(TARGET main.elf POST_BUILD
    COMMAND avr-objcopy -O ihex -R .eeprom main.elf main.hex
    COMMAND avr-size main.elf
)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_CXX_COMPILER avr-g++)

set(CMAKE_OBJCOPY avr-objcopy)
set(CMAKE_SIZE avr-size)

# MCU config
set(MCU atmega328p)
set(F_CPU 16000000UL)

set(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -Os")
set(CMAKE_EXE_LINKER_FLAGS "-mmcu=${MCU}")
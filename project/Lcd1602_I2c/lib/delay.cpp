#include "Arduino.h"
#include <avr/io.h>

void delay(unsigned long ms) {
    // Assuming 16MHz clock, each cycle is ~62.5ns
    // Each iteration ~4 cycles
    // Approximate: 1ms = 4000 iterations
    for (unsigned long i = 0; i < ms; i++) {
        for (unsigned int j = 0; j < 3970; j++) {
            __asm__ __volatile__ ("nop");
        }
    }
}

void delayMicroseconds(unsigned int us) {
    // Similar approach for microseconds
    // Each iteration ~4 cycles = 250ns
    // 1us = 4 iterations
    for (unsigned int i = 0; i < us; i++) {
        for (unsigned int j = 0; j < 4; j++) {
            __asm__ __volatile__ ("nop");
        }
    }
}


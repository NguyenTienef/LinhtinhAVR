#ifndef Arduino_h
#define Arduino_h

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define ARDUINO 101

// Delay functions
void delay(unsigned long ms);
void delayMicroseconds(unsigned int us);

#endif

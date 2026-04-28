#pragma once

#define F_CPU 16000000UL

#include <avr/io.h>

/* LCD pins */
#define LCD_RS_PORT PORTD
#define LCD_RS_DDR  DDRD
#define LCD_RS_PIN  PD2

#define LCD_E_PORT  PORTD
#define LCD_E_DDR   DDRD
#define LCD_E_PIN   PD3

#define LCD_D4_PORT PORTD
#define LCD_D4_DDR  DDRD
#define LCD_D4_PIN  PD4

#define LCD_D5_PORT PORTD
#define LCD_D5_DDR  DDRD
#define LCD_D5_PIN  PD5

#define LCD_D6_PORT PORTD
#define LCD_D6_DDR  DDRD
#define LCD_D6_PIN  PD6

#define LCD_D7_PORT PORTD
#define LCD_D7_DDR  DDRD
#define LCD_D7_PIN  PD7
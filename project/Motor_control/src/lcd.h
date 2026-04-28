#ifndef LCD_H
#define LCD_H

#include <avr/io.h>

void lcd_init(void);
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_clear(void);
void lcd_gotoxy(unsigned char x, unsigned char y);
void lcd_puts(char *str);

#endif
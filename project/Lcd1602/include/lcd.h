#pragma once

#include <stdint.h>

void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print(const char *s);
void lcd_print_at(uint8_t row, uint8_t col, const char *s);
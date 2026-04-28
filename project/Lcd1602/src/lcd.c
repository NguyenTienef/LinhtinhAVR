#include "config.h"
#include "lcd.h"
#include <util/delay.h>

static void lcd_write_pin(volatile uint8_t *port, uint8_t pin, uint8_t value)
{
    if (value) {
        *port |= (1 << pin);
    } else {
        *port &= ~(1 << pin);
    }
}

static void lcd_pulse_enable(void)
{
    lcd_write_pin(&LCD_E_PORT, LCD_E_PIN, 1);
    _delay_us(1);
    lcd_write_pin(&LCD_E_PORT, LCD_E_PIN, 0);
    _delay_us(100);
}

static void lcd_write4(uint8_t nibble)
{
    lcd_write_pin(&LCD_D4_PORT, LCD_D4_PIN, (nibble >> 0) & 0x01);
    lcd_write_pin(&LCD_D5_PORT, LCD_D5_PIN, (nibble >> 1) & 0x01);
    lcd_write_pin(&LCD_D6_PORT, LCD_D6_PIN, (nibble >> 2) & 0x01);
    lcd_write_pin(&LCD_D7_PORT, LCD_D7_PIN, (nibble >> 3) & 0x01);
    lcd_pulse_enable();
}

static void lcd_send(uint8_t value, uint8_t rs)
{
    lcd_write_pin(&LCD_RS_PORT, LCD_RS_PIN, rs);
    lcd_write4(value >> 4);
    lcd_write4(value & 0x0F);
}

static void lcd_command(uint8_t cmd)
{
    lcd_send(cmd, 0);
    _delay_ms(2);
}

static void lcd_data(uint8_t data)
{
    lcd_send(data, 1);
    _delay_ms(1);
}

void lcd_init(void)
{
    LCD_RS_DDR |= (1 << LCD_RS_PIN);
    LCD_E_DDR  |= (1 << LCD_E_PIN);
    LCD_D4_DDR |= (1 << LCD_D4_PIN);
    LCD_D5_DDR |= (1 << LCD_D5_PIN);
    LCD_D6_DDR |= (1 << LCD_D6_PIN);
    LCD_D7_DDR |= (1 << LCD_D7_PIN);

    _delay_ms(20);

    lcd_write_pin(&LCD_RS_PORT, LCD_RS_PIN, 0);
    lcd_write_pin(&LCD_E_PORT, LCD_E_PIN, 0);

    lcd_write4(0x03);
    _delay_ms(5);

    lcd_write4(0x03);
    _delay_us(150);

    lcd_write4(0x03);
    lcd_write4(0x02);

    lcd_command(0x28); // 4-bit, 2 line, 5x8
    lcd_command(0x0C); // display on, cursor off
    lcd_command(0x06); // entry mode set
    lcd_command(0x01); // clear display
    _delay_ms(2);
}

void lcd_clear(void)
{
    lcd_command(0x01);
    _delay_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t address = (row == 0) ? (0x00 + col) : (0x40 + col);
    lcd_command(0x80 | address);
}

void lcd_print(const char *s)
{
    while (*s) {
        lcd_data((uint8_t)(*s));
        s++;
    }
}

void lcd_print_at(uint8_t row, uint8_t col, const char *s)
{
    lcd_set_cursor(row, col);
    lcd_print(s);
}
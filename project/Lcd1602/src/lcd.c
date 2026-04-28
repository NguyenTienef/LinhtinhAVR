#include "config.h"
#include "lcd.h"
#include <util/delay.h>

/* ================= LOW LEVEL ================= */

static void lcd_write_pin(volatile uint8_t *port, uint8_t pin, uint8_t value)
{
    if (value) *port |= (1 << pin);
    else *port &= ~(1 << pin);
}

static void lcd_set_data_output(void)
{
    LCD_D4_DDR |= (1 << LCD_D4_PIN);
    LCD_D5_DDR |= (1 << LCD_D5_PIN);
    LCD_D6_DDR |= (1 << LCD_D6_PIN);
    LCD_D7_DDR |= (1 << LCD_D7_PIN);
}

static void lcd_set_data_input(void)
{
    LCD_D4_DDR &= ~(1 << LCD_D4_PIN);
    LCD_D5_DDR &= ~(1 << LCD_D5_PIN);
    LCD_D6_DDR &= ~(1 << LCD_D6_PIN);
    LCD_D7_DDR &= ~(1 << LCD_D7_PIN);
}

static void lcd_pulse_enable(void)
{
    lcd_write_pin(&LCD_E_PORT, LCD_E_PIN, 1);
    _delay_us(1);
    lcd_write_pin(&LCD_E_PORT, LCD_E_PIN, 0);
    _delay_us(1);
}

/* ================= WRITE ================= */

static void lcd_write4(uint8_t nibble)
{
    lcd_write_pin(&LCD_D4_PORT, LCD_D4_PIN, (nibble >> 0) & 1);
    lcd_write_pin(&LCD_D5_PORT, LCD_D5_PIN, (nibble >> 1) & 1);
    lcd_write_pin(&LCD_D6_PORT, LCD_D6_PIN, (nibble >> 2) & 1);
    lcd_write_pin(&LCD_D7_PORT, LCD_D7_PIN, (nibble >> 3) & 1);
    lcd_pulse_enable();
}

/* ================= READ ================= */

static uint8_t lcd_read4(void)
{
    uint8_t value = 0;

    lcd_write_pin(&LCD_E_PORT, LCD_E_PIN, 1);
    _delay_us(1);

    if (LCD_D4_PINR & (1 << LCD_D4_PIN)) value |= (1 << 0);
    if (LCD_D5_PINR & (1 << LCD_D5_PIN)) value |= (1 << 1);
    if (LCD_D6_PINR & (1 << LCD_D6_PIN)) value |= (1 << 2);
    if (LCD_D7_PINR & (1 << LCD_D7_PIN)) value |= (1 << 3);

    lcd_write_pin(&LCD_E_PORT, LCD_E_PIN, 0);
    _delay_us(1);

    return value;
}

/* ================= BUSY FLAG ================= */

static void lcd_wait_busy(void)
{
    lcd_set_data_input();

    lcd_write_pin(&LCD_RS_PORT, LCD_RS_PIN, 0);
    lcd_write_pin(&LCD_RW_PORT, LCD_RW_PIN, 1);

    uint8_t busy;

    do {
        uint8_t high = lcd_read4(); // chứa BF ở bit 3
        lcd_read4(); // bỏ nibble thấp

        busy = high & 0x08; // D7
    } while (busy);

    lcd_write_pin(&LCD_RW_PORT, LCD_RW_PIN, 0);
    lcd_set_data_output();
}

/* ================= SEND ================= */

static void lcd_send(uint8_t value, uint8_t rs)
{
    lcd_wait_busy();

    lcd_write_pin(&LCD_RS_PORT, LCD_RS_PIN, rs);
    lcd_write_pin(&LCD_RW_PORT, LCD_RW_PIN, 0);

    lcd_write4(value >> 4);
    lcd_write4(value & 0x0F);
}

/* ================= API ================= */

static void lcd_command(uint8_t cmd)
{
    lcd_send(cmd, 0);
}

static void lcd_data(uint8_t data)
{
    lcd_send(data, 1);
}

void lcd_init(void)
{
    LCD_RS_DDR |= (1 << LCD_RS_PIN);
    LCD_RW_DDR |= (1 << LCD_RW_PIN);
    LCD_E_DDR  |= (1 << LCD_E_PIN);

    lcd_set_data_output();

    _delay_ms(20);

    lcd_write_pin(&LCD_RS_PORT, LCD_RS_PIN, 0);
    lcd_write_pin(&LCD_RW_PORT, LCD_RW_PIN, 0);

    // init 4-bit
    lcd_write4(0x03);
    _delay_ms(5);

    lcd_write4(0x03);
    _delay_us(150);

    lcd_write4(0x03);
    lcd_write4(0x02);

    lcd_command(0x28);
    lcd_command(0x0C);
    lcd_command(0x06);
    lcd_command(0x01);
}

void lcd_clear(void)
{
    lcd_command(0x01);
}

void lcd_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t addr = (row == 0) ? col : (0x40 + col);
    lcd_command(0x80 | addr);
}

void lcd_print(const char *s)
{
    while (*s) lcd_data(*s++);
}

void lcd_print_at(uint8_t row, uint8_t col, const char *s)
{
    lcd_set_cursor(row, col);
    lcd_print(s);
}
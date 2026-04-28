#include "lcd.h"
#include <util/delay.h>

#define LCD_PORT PORTC
#define LCD_DDR  DDRC

#define RS PC0
#define EN PC1

void lcd_enable()
{
    LCD_PORT |= (1<<EN);
    _delay_us(1);
    LCD_PORT &= ~(1<<EN);
    _delay_ms(2);
}

void lcd_cmd(unsigned char cmd)
{
    LCD_PORT = cmd;
    LCD_PORT &= ~(1<<RS);
    lcd_enable();
}

void lcd_data(unsigned char data)
{
    LCD_PORT = data;
    LCD_PORT |= (1<<RS);
    lcd_enable();
}

void lcd_clear()
{
    lcd_cmd(0x01);
    _delay_ms(2);
}

void lcd_init()
{
    LCD_DDR = 0xFF;

    _delay_ms(20);

    lcd_cmd(0x38);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
}

void lcd_gotoxy(unsigned char x, unsigned char y)
{
    unsigned char addr;

    if(y==0)
        addr = 0x80 + x;
    else
        addr = 0xC0 + x;

    lcd_cmd(addr);
}

void lcd_puts(char *str)
{
    while(*str)
    {
        lcd_data(*str++);
    }
}
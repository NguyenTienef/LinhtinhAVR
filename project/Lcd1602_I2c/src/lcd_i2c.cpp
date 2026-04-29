#include "LiquidCrystal_I2C.h"
#include <avr/io.h>
#include <util/delay.h>

// Initialize LCD at I2C address 0x27, 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
    lcd.init();
    lcd.backlight();
}

int main()
{
    setup();
    
    lcd.setCursor(0, 0);
    lcd.print("Hello, World!");
    
    while (1)
    {
        // Vòng lặp chính
    }
    
    return 0;
}

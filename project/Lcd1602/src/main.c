#include "config.h"
#include "lcd.h"
#include "timer.h"

int main(void)
{
    char line[17];

    lcd_init();
    clock_init();
    lcd_clear();

    while (1) {
        clock_get_string(line);

        lcd_print_at(0, 0, "                ");
        lcd_print_at(0, 0, line);

        lcd_print_at(1, 0, "LCD CLOCK       ");

        clock_update();
    }

    return 0;
}
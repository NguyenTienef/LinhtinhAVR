#include "config.h"
#include "lcd.h"
#include <util/delay.h>

int main(void)
{
    // Khởi tạo LCD
    lcd_init();
    
    // Chờ LCD sẵn sàng
    _delay_ms(100);

    // Xóa màn hình
    lcd_clear();
    _delay_ms(50);
    
    // In dòng 1
    lcd_set_cursor(0, 0);
    _delay_ms(10);
    lcd_print("Hello LCD");
    _delay_ms(50);
    
    // In dòng 2
    lcd_set_cursor(1, 0);
    _delay_ms(10);
    lcd_print("ATmega328P");
    _delay_ms(50);

    while (1)
    {
        // Chạy vô hạn - LED blink hoặc làm gì đó tùy ý
        _delay_ms(1000);
    }

    return 0;
}
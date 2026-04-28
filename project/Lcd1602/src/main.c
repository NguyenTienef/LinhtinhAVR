#include "config.h"
#include "lcd.h"
#include "timer.h"

int main(void)
{
    lcd_init();
    clock_init();

    char buf[17];

    // Dòng 1: tiêu đề cố định
    lcd_print_at(0, 2, "-- CLOCK --");

    // Hiển thị thời gian ban đầu ngay lập tức
    clock_get_string(buf);
    lcd_print_at(1, 1, buf);

    while (1)
    {
        // clock_update() trả về 1 đúng mỗi giây (hardware Timer1)
        if (clock_update())
        {
            clock_get_string(buf);
            lcd_print_at(1, 1, buf);
        }
        // CPU rảnh làm việc khác hoặc sleep ở đây
    }

    return 0;
}
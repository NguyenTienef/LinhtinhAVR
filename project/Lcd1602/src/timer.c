#include "timer.h"
#include "config.h"   // F_CPU
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

/* ========== Biến thời gian ========== */
static volatile uint8_t g_hour   = 12;
static volatile uint8_t g_minute = 0;
static volatile uint8_t g_second = 0;

/* Flag báo cho main biết vừa qua 1 giây */
static volatile uint8_t g_tick = 0;

/* ========== Timer1 ISR - kích mỗi 1 giây chính xác ========== */
ISR(TIMER1_COMPA_vect)
{
    g_second++;

    if (g_second >= 60) {
        g_second = 0;
        g_minute++;

        if (g_minute >= 60) {
            g_minute = 0;
            g_hour++;

            if (g_hour >= 24)
                g_hour = 0;
        }
    }

    g_tick = 1;  // báo hiệu main cập nhật LCD
}

/* ========== Khởi tạo Timer1 CTC, prescaler 1024 ========== */
/*
 * F_CPU = 16 000 000 Hz, prescaler = 1024
 * Tần số đếm = 16000000 / 1024 = 15625 Hz
 * OCR1A = 15625 - 1 = 15624  →  ngắt mỗi đúng 1 giây
 */
void clock_init(void)
{
    g_hour   = 12;
    g_minute = 0;
    g_second = 0;
    g_tick   = 0;

    /* CTC mode (WGM12 = 1) */
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);  // prescaler 1024

    /* So sánh = 15624 → đúng 1 giây */
    OCR1A = 15624;

    /* Cho phép ngắt Compare Match A */
    TIMSK1 = (1 << OCIE1A);

    /* Bật ngắt toàn cục */
    sei();
}

/* ========== Gọi trong main loop - trả về 1 nếu vừa qua 1 giây ========== */
uint8_t clock_update(void)
{
    if (g_tick) {
        g_tick = 0;
        return 1;
    }
    return 0;
}

/* ========== Lấy chuỗi hiển thị ========== */
void clock_get_string(char *buf)
{
    uint8_t h, m, s;

    /* Đọc atomic - tắt ngắt tạm thời */
    cli();
    h = g_hour;
    m = g_minute;
    s = g_second;
    sei();

    snprintf(buf, 17, "TIME %02u:%02u:%02u", h, m, s);
}
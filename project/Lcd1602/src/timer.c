#include "timer.h"
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

static uint8_t g_hour   = 12;
static uint8_t g_minute = 0;
static uint8_t g_second = 0;

void clock_init(void)
{
    g_hour = 12;
    g_minute = 0;
    g_second = 0;
}

void clock_update(void)
{
    _delay_ms(1000);

    g_second++;

    if (g_second >= 60) {
        g_second = 0;
        g_minute++;

        if (g_minute >= 60) {
            g_minute = 0;
            g_hour++;

            if (g_hour >= 24) {
                g_hour = 0;
            }
        }
    }
}

void clock_get_string(char *buf)
{
    snprintf(buf, 17, "TIME %02u:%02u:%02u", g_hour, g_minute, g_second);
}
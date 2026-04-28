#pragma once

#include <stdint.h>

void    clock_init(void);
uint8_t clock_update(void);       // trả về 1 nếu vừa qua đúng 1 giây
void    clock_get_string(char *buf);
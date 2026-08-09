#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    // PB5 output
    DDRB |= (1 << DDB5);

    // Bắt đầu ở mức LOW
    PORTB &= ~(1 << PORTB5);

    while (1)
    {
        PORTB ^= (1 << PORTB5);
        _delay_ms(500);
    }

    return 0;
}
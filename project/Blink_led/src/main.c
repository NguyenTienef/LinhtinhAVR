#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    // PB5 output
    DDRB |= (1 << PB5);

    while (1)
    {
        PORTB ^= (1 << PB5);   // toggle LED
        _delay_ms(500);
    }
}
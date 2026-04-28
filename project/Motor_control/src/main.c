#include "lcd.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#define Tang  (!(PINB & (1<<PB0)))
#define Giam  (!(PINB & (1<<PB1)))
#define Dao   (!(PINB & (1<<PB2)))
#define Stop  (!(PINB & (1<<PB3)))

volatile unsigned long encoder=0;
volatile unsigned char countT0=0,countT2=0 ;
unsigned int speed;
unsigned int temp=0;
unsigned char phantram=0;
char lcd_buf[20];

ISR(INT0_vect)
{
    encoder++;
}

ISR(TIMER0_OVF_vect)
{
    TCCR0 = 0;

    if(countT0 >= 20)
    {
        countT0=0;
        speed=((float)encoder/128)*10;
        encoder=0;
    }

    countT0++;

    TCCR0=(1<<CS02)|(1<<CS00);
    TCNT0=0x06;
}

ISR(TIMER2_OVF_vect)
{
    TCCR2=0;

    if(countT2 >=5)
    {
        countT2=0;

        if(Tang)
        {
            _delay_ms(10);
            if(Tang)
            {
                while(Tang);
                temp+=500;
                if(temp>=7999) temp=7999;
            }
        }

        else if(Giam)
        {
            if(temp>=500)
            {
                _delay_ms(10);
                if(Giam)
                {
                    while(Giam);
                    temp-=500;
                    if(temp<=0) temp=0;
                }
            }
            else temp=0;
        }

        else if(Dao)
        {
            _delay_ms(10);
            if(Dao)
            {
                while(Dao);
                PORTD ^= (1<<PD3);
            }
        }

        else if(Stop)
        {
            _delay_ms(10);
            if(Stop)
            {
                while(Stop);
                PORTD ^= (1<<PD1);
            }
        }

        if(!(PORTD & (1<<PD3)))
        {
            OCR1A = temp;
        }
        else
        {
            unsigned int Temp=7999-temp;
            OCR1A = Temp;
        }
    }

    countT2++;

    TCNT2=0x00;
    TCCR2=(1<<CS22)|(1<<CS21)|(1<<CS20);
}

int main(void)
{

DDRB &= ~((1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3));
PORTB |= (1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3);

DDRD |= (1<<PD5)|(1<<PD3)|(1<<PD1);

TCCR0=(1<<CS02)|(1<<CS00);
TCNT0=0x06;

TCCR1A=(1<<COM1A1)|(1<<WGM11);
TCCR1B=(1<<WGM13)|(1<<WGM12)|(1<<CS10);

ICR1=7999;
OCR1A=0;

TCCR2=(1<<CS22)|(1<<CS21)|(1<<CS20);
TCNT2=0x06;

TIMSK=(1<<TOIE2)|(1<<TOIE0);

GICR |= (1<<INT0);
MCUCR |= (1<<ISC01);

lcd_init();
sei();

lcd_clear();
lcd_gotoxy(0,0);
lcd_puts("Xung: ");
lcd_gotoxy(0,1);
lcd_puts("Toc do: ");

while (1)
{
    lcd_gotoxy(8,1);
    sprintf(lcd_buf,"%d.%d V/s   ",speed/10,speed%10);
    lcd_puts(lcd_buf);

    lcd_gotoxy(6,0);
    phantram = ((float)temp/7999)*100;
    sprintf(lcd_buf,"%d %%  ",phantram);
    lcd_puts(lcd_buf);

    _delay_ms(100);
}
}
#include <mega16.h>
#include <delay.h>
#include <alcd.h>
#include <stdio.h>

#define Tang PINB.0
#define Giam PINB.1
#define Dao PINB.2
#define Stop PINB.3
unsigned long encoder=0;
unsigned char countT0=0,countT2=0 ;
unsigned int speed;
unsigned int temp=0;
unsigned char phantram=0;
char lcd[20];

interrupt [EXT_INT0] void ext_int0_isr(void)
{
    encoder++;
}
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
    TCCR0=(0<<CS02) | (0<<CS01) | (0<<CS00);
    if(countT0 >= 20)
    {
        countT0=0; 
       // speed=(float)((float)(encoder*1000)/(200*640))*10;  
        speed=((float)encoder/128)*10;
        encoder=0;        
    }
    countT0++;     
    TCCR0=(1<<CS02) | (0<<CS01) | (1<<CS00);     
    TCNT0=0x06;
}

interrupt [TIM2_OVF] void timer2_ovf_isr(void)
{
    TCCR2=(0<<CS22) | (0<<CS21) | (0<<CS20);  
    if(countT2 >=5)
    {  
        countT2=0;
       if(!Tang)
       {
            delay_ms(10);
            if(!Tang)
            {
                while(!Tang); 
                temp+=500;
                if(temp>=7999)temp=7999;            
            }
       }  
       else if(!Giam)
       {    
            if(temp>=500)
            {
                delay_ms(10);
                if(!Giam)
                {
                    while(!Giam); 
                    temp-=500;
                    if(temp<=0) temp=0;            
                }
            }
            else temp=0;
       }
       else if(!Dao)
       {
            delay_ms(10);
            if(!Dao)
            {
                while(!Dao);
                PORTD.3=!PORTD.3;              
            }
       }
       else if(!Stop)
       {      
            delay_ms(10);
            if(!Stop)
            {
                while(!Stop);
                PORTD.1=!PORTD.1;               
            }
       } 
       if(PORTD.3==0)
       {
            OCR1AH= temp >>8 ;
            OCR1AL= temp && 0xFF ;
       }
       else
       {
            unsigned int Temp=7999-temp;   
            OCR1AH=Temp>>8 ;
            OCR1AL=Temp && 0xFF ;          
       }         
    } 
    countT2++; 
   TCNT2=0x00; 
   TCCR2=(1<<CS22) | (1<<CS21) | (1<<CS20);
}

void main(void)
{
DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (1<<PORTB3) | (1<<PORTB2) | (1<<PORTB1) | (1<<PORTB0);
 
DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

DDRD=(0<<DDD7) | (0<<DDD6) | (1<<DDD5) | (0<<DDD4) | (1<<DDD3) | (0<<DDD2) | (1<<DDD1) | (0<<DDD0);
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (1<<PORTD1) | (0<<PORTD0);

TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (1<<CS02) | (0<<CS01) | (1<<CS00);
TCNT0=0x06;
OCR0=0x00;

TCCR1A=(1<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (1<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (1<<WGM13) | (1<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x1F;
ICR1L=0x3F;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

ASSR=0<<AS2;
TCCR2=(0<<PWM2) | (0<<COM21) | (0<<COM20) | (0<<CTC2) | (1<<CS22) | (1<<CS21) | (1<<CS20);
TCNT2=0x06;
OCR2=0x00;  

TIMSK=(0<<OCIE2) | (1<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (1<<TOIE0);

GICR|=(0<<INT1) | (1<<INT0) | (0<<INT2);
MCUCR=(0<<ISC11) | (0<<ISC10) | (1<<ISC01) | (0<<ISC00);
MCUCSR=(0<<ISC2);
GIFR=(0<<INTF1) | (1<<INTF0) | (0<<INTF2);
   

lcd_init(16);
#asm("sei")
lcd_clear();
lcd_gotoxy(0,0);
lcd_puts("Xung: ");
lcd_gotoxy(0,1);
lcd_puts("Toc do: ");
while (1)
      {
        lcd_gotoxy(8,1);    
        sprintf(lcd,"%d.%d V/s   ",speed/10,speed%10);
        lcd_puts(lcd);
        lcd_gotoxy(6,0);  
        phantram = ((float)temp/7999) *100;   
        sprintf(lcd,"%d %%  ",phantram);    
        lcd_puts(lcd);
        delay_ms(100);
      }
}

/*****************************************************
This program was produced by the
CodeWizardAVR V1.25.7 beta 5 Professional
Automatic Program Generator
© Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : AVR Clock
Version :
Date    :
Author  :
Company :
Comments:


Chip type           : ATtiny2313
Clock frequency     : 16,000000 MHz
Memory model        : Tiny
External SRAM size  : 0
Data Stack size     : 32
*****************************************************/

#include <avr/io.h>
#undef  F_CPU
#define F_CPU 16000000L
#include <util/delay.h>
#include <avr/interrupt.h>
//#include <avr/sleep.h>
#define digit_display_time 1

unsigned char milliseconds, seconds, ten_seconds, minutes, ten_minutes, hours, ten_hours;
uint8_t button_pressed1, button_pressed2;


// Timer 1 output compare A interrupt service routine
//interrupt [TIM1_COMPA] void timer1_compa_isr(void)    //Прерывание происходит 100 раз в сек, сохраняем в глобальные переменные текущее время
ISR(TIMER1_COMPA_vect)
{ milliseconds++;
	TCNT1H=0x00;
	TCNT1L=0x00;
	MCUCR ;
	if (milliseconds >= 100 )
	{
		milliseconds = 0;
		seconds++;
	};
	
	if (seconds >= 10)
	{
		seconds = 0;
		ten_seconds++;
	};
	
	if (ten_seconds >= 6)
	{
		ten_seconds = 0;
		minutes++;
	};
	
	if (minutes >= 10)
	{
		minutes = 0;
		ten_minutes++;
	};
	
	if (ten_minutes >= 6)
	{
		ten_minutes = 0;
		hours++;
	};

	if (hours >= 10)
	{
		hours = 0;
		ten_hours++;
	};

	if (ten_hours >= 2 && hours == 4)
	{
		ten_hours = 0;
		hours=0;
	};
	

}



int main()
{
	unsigned char digits[10] = {18, 159, 56, 28, 149, 84, 80, 31, 16, 20};     //массив для генерации цифр. Какой элемент массива будет отправлен в порт, такая цифра и загорится.


	// Crystal Oscillator division factor: 1

	CLKPR=0x80;
	CLKPR=0x00;


	// Input/Output Ports initialization
	// Port A initialization
	// Func2=In Func1=In Func0=In
	// State2=T State1=T State0=T
	PORTA=0x00;
	DDRA=0x00;

	// Port B initialization
	// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out
	// State7=1 State6=1 State5=1 State4=1 State3=1 State2=1 State1=1 State0=1
	PORTB=0xFF;
	DDRB=0xFF;

	// Port D initialization
	// Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=In Func0=In
	// State6=0 State5=0 State4=0 State3=0 State2=0 State1=P State0=P
	PORTD=0x03;
	DDRD=0x7C;

	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: Timer 0 Stopped
	// Mode: Normal top=FFh
	// OC0A output: Disconnected
	// OC0B output: Disconnected
	TCCR0A=0x00;
	TCCR0B=0x00;
	TCNT0=0x00;
	OCR0A=0x00;
	OCR0B=0x00;

	// Timer/Counter 1 initialization
	// Clock source: System Clock
	// Clock value: 62,500 kHz
	// Mode: Normal top=FFFFh
	// OC1A output: Discon.
	// OC1B output: Discon.
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	// Timer 1 Overflow Interrupt: Off
	// Input Capture Interrupt: Off
	// Compare A Match Interrupt: On
	// Compare B Match Interrupt: Off
	TCCR1A=0x00;
	TCCR1B=0x04;
	TCNT1H=0x00;
	TCNT1L=0x00;
	ICR1H=0x00;
	ICR1L=0x00;
	OCR1AH=0x02;
	OCR1AL=0x70;
	OCR1BH=0x00;
	OCR1BL=0x00;

	// External Interrupt(s) initialization
	// INT0: Off
	// INT1: Off
	// Interrupt on any change on pins PCINT0-7: Off
	GIMSK=0x00;
	MCUCR=0x00;

	// Timer(s)/Counter(s) Interrupt(s) initialization
	TIMSK=0x40;

	// Universal Serial Interface initialization
	// Mode: Disabled
	// Clock source: Register & Counter=no clk.
	// USI Counter Overflow Interrupt: Off
	USICR=0x00;

	// Analog Comparator initialization
	// Analog Comparator: Off
	// Analog Comparator Input Capture by Timer/Counter 1: Off
	ACSR=0x80;

	// Global enable interrupts
	sei();

	while (1)
	{
		PORTD |=1<<5;                              //включаем первый индикатор
		PORTB = digits[ten_hours];                 //выводим на него десятки часов
		_delay_ms(digit_display_time);              //ждем, время индикации одного разряда задается в заголовке программы
		PORTD &=~(1<<5);                           //выключаем первый индикатор и переходим дальше и так в цикле для каждого из 4 разрядов
		PORTD |=1<<4;
		PORTB = digits[hours];
		if (milliseconds >= 50) PORTB &=~(1<<4); else PORTB|=1<<4;  //моргание точки
		_delay_ms(digit_display_time);
		PORTD &=~(1<<4);
		PORTD |=1<<3;
		PORTB = digits[ten_minutes];
		_delay_ms(digit_display_time);
		PORTD &=~(1<<3);
		PORTD |=1<<6;
		PORTB = digits[minutes];
		_delay_ms(digit_display_time);
		PORTD &=~(1<<6);
		
		//а вот теперь проверим кнопочки в стиле часов Электроника 13
		
		if ((PIND & 1<<0) == 0 && button_pressed1 == 0)                     //Если нажата кнопка 1
		{
			_delay_ms(1);
			hours++;
			button_pressed1 = 1;
		};
		
		if ((PIND & 1<<0) == 1) button_pressed1=0;                          //Если отпущена то   сбрасываем флаг
		

		
		if ((PIND & 1<<1) == 0 && button_pressed2 == 0)                     //Если нажата  кнопка 2
		{
			_delay_ms(1);
			minutes++;
			button_pressed2 = 1;
		};
		
		if (PIND & 1<<1) button_pressed2=0;
		
		
		

	};
}
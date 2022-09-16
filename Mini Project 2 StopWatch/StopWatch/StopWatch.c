/*
 * Mini_Project2_StopWatch
 *  Author: Ahmed Yasser
 *  Diploma 56
 */
				//////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>
#include "util/delay.h"

					///////////////////////////////
//Global Variables
unsigned char seconds=0,minutes=0,hours=0; // Variables For counting Time
unsigned char flag = 0; // Flag Variable for The Timer Interrupt

					////////////////////////////
// FUNCTION PROTOTYPE
void display();
 	 	 	 	 	////////////////////////////

//Timer1 Interrupt Function and its ISR
ISR(TIMER1_COMPA_vect) //Interrupt service routine for Timer1
{
	flag = 1; 	/*When The Interrupt in and gate ISR raise flag = 1 and this for decrease
				the time needed to done the ISR*/
}

void Timer1_Init_CTC_mode()
{
	TCNT1 = 0 ;  //Setting Initial Value For The Timer = 0
	OCR1A =977;  // Compare Match Value Each ONE SECOND
	TIMSK |= (1<<OCIE1A) ;   // Enable The Interrupt For Timer1 Output Compare A Match Interrupt Enable
	/* Configure timer1 control registers
		* for non PWM set FOC1A and FOC1B
		*CTC mode Set WGM12 and For Prescalar 1024 it needs 977 millisecond and set in OCR1A
		to complete one second
		and set CS10 and CS12 For prescalar 1024*/

	TCCR1A = (1<<FOC1A) | (1<<FOC1B);
	TCCR1B = (1<<CS12)|(1<<CS10) | (1<<WGM12) ;
}

					/////////////////////////////
//Reset Interrupt Function and its ISR
ISR(INT0_vect) 	//Interrupt service routine for INT0 for Reset Set all counting Variables = 0
{
seconds = 0;
minutes = 0;
hours = 0;
}

void INT0_Init()
{
	GICR |= (1<<INT0);		// Enable The Interrupt INT0
	MCUCR |= (1<<ISC01) ;   //Trigger with falling edge
	MCUCR &=~ (1<<ISC00);	//Trigger with falling edge
	DDRD &= (~(1<<PD2));				//configuring PD2 to be input
	PORTD |= (1<<PD2);				//enabling internal pull up resistance

}

					/////////////////////////////

//Pause Interrupt Function and its ISR
ISR(INT1_vect) 		//Interrupt service routine for INT1 for Pause StopWatch
{
	/* To Pause StopWatch Clear bits of (CS12=0 CS10=0 CS11=0) To stop The Clock*/
	if((PIND & (1<<3)) )
	{
	TCCR1B &= (~(1<<CS10))  & (~(1<<CS12));
	}
}

void INT1_Init()
{
	DDRD &= (~(1<<PD3));					//configuring PD3 to be input
	GICR |= (1<<INT1);						// Enable The Interrupt INT1
	MCUCR |= (1<<ISC11) | (1<<ISC10);		//Trigger with rising edge
}

					////////////////////////////
//Resume Interrupt Function and its ISR
ISR(INT2_vect)   //Interrupt service routine for INT2 for Resume StoWatch
{
	TCCR1B |= (1<<CS10) | (1<<CS12); //Enable The clock bits CS10 CS12
}

void INT2_Init()
{
	DDRB &= (~(1<<PB2));				//configuring PB2 to be input
	PORTB |= (1<<PB2);			//enabling internal pull up resistance
	GICR |= (1<<INT2);				//enabling INT2 interrupt
	MCUCSR &= ~(1<<ISC2);				//Trigger with falling edge
}


/*function to display The 6 seven segment in same time by enable the first one
 * which is PORTA=0X01 and insert the reminder of the seconds ex.. 1 % 10 = 1 and etc..
 */
void display()
{
	PORTA = 0x01;
	PORTC = seconds % 10;
	_delay_ms(2);
	PORTA = 0x02;
	PORTC = seconds / 10;
	_delay_ms(2);
	PORTA = 0x04;
	PORTC = minutes % 10;
	_delay_ms(2);
	PORTA = 0x08;
	PORTC = minutes / 10;
	_delay_ms(2);
	PORTA = 0x10;
	PORTC = hours % 10;
	_delay_ms(2);
	PORTA = 0x20;
	PORTC = hours / 10;
	_delay_ms(2);

}

					//////////////////////////////

int main(void)
{
	SREG |= (1<<7); 			//Enable I-Bit
	DDRC=0x0F; 					//First 4 pins OUTPUT
	PORTC &= 0xF0; 				//initialize with zero
	DDRA|=0x3F; 				//First 6 Pins OUTPUT
	PORTA |=0x3F; 				// Enable all 7-Segments
	 Timer1_Init_CTC_mode(); 	// Generating The Timer1 in CTC Mode
	INT0_Init(); 				//Generating The Reset
	INT1_Init();				//Generating The Pause
	INT2_Init();				//Generating The Resume
	for (;;)
	{
		if(flag == 1)      //When ISR is Done
		{
			seconds ++ ;
				if(seconds == 60)
				{
					seconds = 0;
					minutes ++ ;
				}
				if(minutes == 60)
				{
					seconds = 0;
					minutes = 0;
					hours ++ ;
				}
				if(hours == 24)
				{
					seconds = 0;
					minutes = 0;
					hours = 0;
				}
				flag = 0;
		}

			display();

	}
}

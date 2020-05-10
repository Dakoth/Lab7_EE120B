/*	Author: agonz250
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab # 7 Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h> 
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//timer stuff 
volatile unsigned char TimerFlag = 0; //stuff added 

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	//AVR timer/counter controller register ....
	TCCR1B = 0x0B;

	//AVR output compare register ....
	OCR1A = 125;

	//AVR timer interrupt mask register
	TIMSK1 = 0x02;

	//initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	//TimerIRS called every _avr ... milliseconds
	//
	
	//enable global interrupts
	SREG |= 0x80; //0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; //timer off
}

void TimerISR() {
	TimerFlag = 1;
}

//TimerISR() calls this 
ISR(TIMER1_COMPA_vect) {
	//cpu calls when TCNT1 == OCR1
	_avr_timer_cntcurr--;	//counts down to 0

	if (_avr_timer_cntcurr == 0) {
		TimerISR();	//calls ISR that user uses
		_avr_timer_cntcurr = _avr_timer_M;

	}
}

//Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//State Machine stuff 


enum States {Start, Wait, incHold, decHold, reset} state; 

unsigned char tmpA; //global variables 

unsigned char tmpC;//= 0x00;
unsigned char tmpD;
unsigned char i = 0;	//Used to count up to 1 second, for the inc hold states 

void Tick() {
	//unsigned char tmpA = PINA;
	switch(state) { //transitions
		case Start:
			tmpC = 0; 
			state = Wait;
			break;
		
		case Wait:
			if ( ((tmpA & 0x01) == 0x01) && ((tmpA & 0x02) == 0x00) ) { //if PA0 && !PA1
				state = incHold;

				if (tmpC < 9) { 
					++tmpC; 
					i = 0;
					LCD_ClearScreen();
					LCD_WriteData(tmpC + '0');

				} //TRANSITION ACTION

			} 
			else if ( ((tmpA & 0x02) == 0x02) && ((tmpA & 0x01) == 0x00) ) //if PA1 && !PA0
			{
				state = decHold;

				if (tmpC > 0) { 
					--tmpC; 
					i = 0; 
					LCD_ClearScreen();
					LCD_WriteData(tmpC + '0');
				}	//TRANSITION ACTION

			}
			else if ((tmpA & 0x03) == 0x03) { //if PA1 && PA0
				state = reset;
				tmpC = 0; 	//SHOULD BE 0 , SETTING TO 0x7F  FOR TEST
			}
			else { //If none of the above
				state = Wait;
			}
			break;

		case incHold: //Might need a transition for resetting?
			if( ((tmpA & 0x01) == 0x01) && ((tmpA & 0x02) == 0x00) ) { //if PA0 && !PA1 
				state = incHold;

				//NEW 
				if (tmpC < 9 && i == 10) { //if 1 second has passed 
					++tmpC;
					i = 0; //reset i
					LCD_ClearScreen();
					LCD_WriteData(tmpC + '0');
				}

			}
	 			
			else if ( (tmpA & 0x03) == 0x03  ) { //added this so can reset
				state = reset;
			}	
			else { 
				state = Wait;
			}			
			break;

		case decHold:
			if( ((tmpA & 0x02) == 0x02) && ((tmpA & 0x01) == 0x00) ) { //if !PA0 && PA1 
			
				state = decHold;

				//NEW
				if (tmpC > 0 && i == 10) { //If 1 sec has passed 
					--tmpC;
					i = 0; //reset i ; 
					LCD_ClearScreen();
					LCD_WriteData(tmpC + '0');
				}

			}
			else if ( (tmpA & 0x03) == 0x03) {
				state = reset;
			}
			else {
				state = Wait;
			}
			break;

		case reset: 
			if ((tmpA & 0x03) == 0x03) {
				state = reset;
			}
			else {
				state = Wait;
			}
			break;

		default: 
			//shouldn't go here
			state = Start;
			break;					
	}

	switch(state) { 
		case Wait:
			break;

		case incHold:
			//if (tmpC < 9) { ++tmpC; }
			i++; 
			break;
		
		case decHold:
			//if (tmpC > 0) { --tmpC; }
			i++;
			break;
		
		case reset: 
			tmpC = 0;
			LCD_ClearScreen();
			LCD_WriteData(tmpC + '0');
			break;
		default:
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;

	DDRC = 0xFF; PORTC = 0x00; 	//LCD data lines
	DDRD = 0xFF; PORTD = 0x00;	//LCD control lines 

	//Timer stuff
	TimerSet(100);
	TimerOn();

	//initalizes LCD display
	LCD_init();
	
	//Starting at position 1, writes message
	//LCD_DisplayString(1, "Beep beep I'm a sheep");
	tmpA = 0x00;
	tmpC = 0x00;
	tmpD = 0x00;

    /* Insert your solution below */
    while (1) {
	//continue;
	tmpA = ~PINA;

	Tick();
	PORTC = tmpC;

	while (!TimerFlag) {}
	TimerFlag = 0;
    }
    //return 1;
}

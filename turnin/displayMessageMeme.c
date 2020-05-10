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
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
//#include "io.h"

//timer stuff 

// States n stuff 



int main(void) {
    /* Insert DDR and PORT initializations */
	DDRC = 0xFF; PORTC = 0x00; 	//LCD data lines
	DDRD = 0xFF; PORTD = 0x00;	//LCD control lines 

	//initalizes LCD display
	LCD_init();

	//const unsigned char* message;
	//message = "Hello World";
	
	//Starting at position 1, writes message
	LCD_DisplayString(1, "Beep beep I'm a sheep");

    /* Insert your solution below */
    while (1) {
	continue;
    }
    //return 1;
}

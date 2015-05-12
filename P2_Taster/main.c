/*
 * main.c
 *
 *  Created on: 12.05.2015
 *      Author: raphael
 */

#include <avr/io.h>
#include <avr/iom16.h> // Eclipse makefile does not work :/
#include <util/delay.h>

int main() {
	// set port A pin 0 as input (switch)
	DDRA &= ~((1<<PA0));
	// set port C pin 0 as output (led)
	DDRC |= ((1<<PC0));
	while(1) {
		// is switch at port A pin 0 pressed (active low)
		if(!(PINA & (1<<PA0))){
			while(!(PINA & (1<<PA0))){
				_delay_ms(10);
			}
			// toggle led at port C pin 0
			PORTC ^= (1<<PC0);
		}
	}
	return 0;
}

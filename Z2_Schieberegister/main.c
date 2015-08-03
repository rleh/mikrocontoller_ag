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
	// set port A pin 0 and pin 1 as input (switches)
	DDRA &= ~((1<<PA0)|(1<<PA1));
	PORTA |= ((1<<PA0)|(1<<PA1));
	// set port C any pins as output (8 leds)
	DDRC |= ((1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3)
			|(1<<PC4)|(1<<PC5)|(1<<PC6)|(1<<PC7));

	uint8_t leds = 0; // leds off

	while(1) {
		// is switch at port A pin 0 pressed (active low)
		if(!(PINA & (1<<PA0))){
			_delay_ms(10);
			while(!(PINA & (1<<PA0)));
			_delay_ms(10);
			// shift leds byte left
			leds <<= 1;
			// if PA1 is pressed, set lowest bit
			if(!(PINA & (1<<PA1))){
				leds |= 0x01; // 0x01 = 0b00000001
			}
			// display leds byte
			PORTC = ~leds;
		}
	}
	return 0;
}

/*
 * main.c
 *
 *  Created on: 12.05.2015
 *      Author: raphael
 */

#define F_CPU 14745000
#include <avr/io.h>
#include <avr/iom16.h> // Eclipse makefile does not work :/
#include <util/delay.h>

#define LEDS_DELAY 62 // 1 second

int main() {
	// set port C any pins as output (8 leds)
	DDRC |= ((1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3)
			|(1<<PC4)|(1<<PC5)|(1<<PC6)|(1<<PC7));

	uint8_t leds = (1<<PC0); // = 0x01;
	uint8_t counter = 0;

	while(1) {
		// shift bit in leds byte 8 times left
		for(counter = 0; counter < 7; counter++){
			leds = (leds<<1);
			// display leds
			PORTC = ~leds;
			// wait COUNTER_DELAY
			_delay_ms(LEDS_DELAY);
		}
		// shift bit in leds byte 8 times right
		for(counter = 0; counter < 7; counter++){
			leds = (leds>>1);
			// display leds
			PORTC = ~leds;
			// wait COUNTER_DELAY
			_delay_ms(LEDS_DELAY);
		}
	}
	return 0;
}

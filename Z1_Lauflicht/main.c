/*
 * main.c
 *
 *  Created on: 12.05.2015
 *      Author: raphael
 */

#include <avr/io.h>
#include <avr/iom16.h> // Eclipse makefile does not work :/
#include <util/delay.h>

#define LEDS_DELAY 100 // 100 milliseconds

int main() {
	// set port C any pins as output (8 leds)
	DDRC |= ((1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3)
			|(1<<PC4)|(1<<PC5)|(1<<PC6)|(1<<PC7));

	uint8_t leds = (1<<PC0); // = 1;
	uint8_t counter = 0;

	while(1) {
		// shift bit in leds byte 8 times left
		for(counter = 0; counter < 8; counter++){
			leds <<= 1;
			// display leds
			PORTC = leds;
			// wait COUNTER_DELAY (0.1s)
			_delay_ms(LEDS_DELAY);
		}
		// shift bit in leds byte 8 times right
		for(counter = 0; counter < 8; counter++){
			leds >>= 1;
			// display leds
			PORTC = leds;
			// wait COUNTER_DELAY (0.1s)
			_delay_ms(LEDS_DELAY);
		}
	}
	return 0;
}

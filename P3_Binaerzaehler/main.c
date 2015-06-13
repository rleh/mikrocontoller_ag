/*
 * main.c
 *
 *  Created on: 12.05.2015
 *      Author: raphael
 */

#include <avr/io.h>
#include <avr/iom16.h> // Eclipse makefile does not work :/
#include <util/delay.h>

#define COUNTER_DELAY 500 // 100 milliseconds
#define COUNTER_RESET_DELAY 10000 // 10 seconds

int main() {
	// set port C any pins as output (8 leds)
	DDRC |= ((1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3)
			|(1<<PC4)|(1<<PC5)|(1<<PC6)|(1<<PC7));

	uint8_t counter = 0;

	while(1) {
		for(counter = 0; counter <= 255; counter++){
			// display counter value
			PORTC = counter;
			// wait COUNTER_DELAY (0.1s)
			_delay_ms(COUNTER_DELAY);
		}
		// wait COUNTER_DELAY (10s)
		_delay_ms(COUNTER_RESET_DELAY);
	}

	// alternative implementation
	while(1) {
		// counter is uint8_t -> overflow
		if(counter++ == 0){
			// wait COUNTER_DELAY (10s)
			_delay_ms(COUNTER_RESET_DELAY);
		}
		else{
			// wait COUNTER_DELAY (0.1s)
			_delay_ms(COUNTER_DELAY);
		}
		// display counter value
		PORTC = counter;
	}
	return 0;
}

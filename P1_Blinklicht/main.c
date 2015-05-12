/*
 * main.c
 *
 *  Created on: 12.05.2015
 *      Author: raphael
 */

#include <avr/io.h>
#include <avr/iom16.h> // Eclipse makefile does not work :/
#include <util/delay.h>

#define LED_BLINK_DELAY 2000

int main() {
	// set port C pin 3 as output
	DDRC |= (1<<PC3);
	// led (port C pin 3) on
	PORTC |= (1<<PC3);
	while(1) {
		// toggle port C pin 3
		PORTC ^= (1<<PC3);
		// wait LED_BLINK_DELAY (ms)
		_delay_ms(LED_BLINK_DELAY);
	}
	return 0;
}

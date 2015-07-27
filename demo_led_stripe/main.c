/*
 * main.c
 *
 *  Created on: 12.06.2015
 *      Author: raphael
 */

#ifndef F_CPU
#define F_CPU 14745600
#endif
#include <avr/io.h>
#include <avr/iom16.h> // Eclipse makefile does not work :/
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "ws2812_config.h" // override config in submodule
#include "Light_WS2812/light_ws2812.h"

//struct cRGB led[35];
struct cRGB black[35];
struct cRGB white[35];
//struct cRGB colors[39];
struct cRGB red_wave[42];
struct cRGB blue_wave[42];
struct cRGB green_wave[42];


void doWaveCycle(struct cRGB * wave, uint8_t cycles) {
	for (uint8_t i = 0; i < cycles; i++) {
		ws2812_setleds((wave), 35);
		_delay_ms(50);
		ws2812_setleds((wave + 1), 35);
		_delay_ms(50);
		ws2812_setleds((wave + 2), 35);
		_delay_ms(50);
		ws2812_setleds((wave + 3), 35);
		_delay_ms(50);
		ws2812_setleds((wave + 4), 35);
		_delay_ms(50);
		ws2812_setleds((wave + 5), 35);
		_delay_ms(50);
	}
}

int main(void) {
	DDRC = 0xFF;

	for (uint8_t j = 0; j < 35; j++) {
		black[j].r = 0;
		black[j].g = 0;
		black[j].b = 0;
		white[j].r = 255;
		white[j].g = 255;
		white[j].b = 255;
	}

	for (uint8_t j = 0; j < 42; j++) {
		red_wave[j].g = 0;
		red_wave[j].b = 0;
	}
	for (uint8_t j = 0; j < 42; j += 6) {
		red_wave[j].r = 2;
		red_wave[j + 1].r = 30;
		red_wave[j + 2].r = 110;
		red_wave[j + 3].r = 255;
		red_wave[j + 4].r = 110;
		red_wave[j + 5].r = 30;
	}

	for (uint8_t j = 0; j < 42; j++) {
		blue_wave[j].r = 0;
		blue_wave[j].g = 0;
	}
	for (uint8_t j = 0; j < 42; j += 6) {
		blue_wave[j].b = 110;
		blue_wave[j + 1].b = 255;
		blue_wave[j + 2].b = 110;
		blue_wave[j + 3].b = 30;
		blue_wave[j + 4].b = 2;
		blue_wave[j + 5].b = 30;
	}

	for (uint8_t j = 0; j < 42; j++) {
		green_wave[j].r = 0;
		green_wave[j].b = 0;
	}
	for (uint8_t j = 0; j < 42; j += 6) {
		green_wave[j].g = 110;
		green_wave[j + 1].g = 30;
		green_wave[j + 2].g = 2;
		green_wave[j + 3].g = 30;
		green_wave[j + 4].g = 110;
		green_wave[j + 5].g = 255;
	}

	/*for (uint8_t j = 0; j < 39; j += 3) {
		colors[j].r = 100;
		colors[j].g = 200;
		colors[j].b = 255;
		colors[j + 1].r = 200;
		colors[j + 1].g = 255;
		colors[j + 1].b = 100;
		colors[j + 2].r = 255;
		colors[j + 2].g = 100;
		colors[j + 2].b = 200;
	}*/

	while (1) {
		PORTC ^= 0xFF;

		doWaveCycle(red_wave, 10);
		doWaveCycle(blue_wave, 10);
		doWaveCycle(green_wave, 10);

		for (int i = 0; i < 20; i++) {
			ws2812_setleds(black, 35);
			_delay_ms(50);
			ws2812_setleds(white, 35);
			_delay_ms(50);
		}
	}
}

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
#include <avr/pgmspace.h>
#include <stdlib.h>

#include "ws2812_config.h" // override config in submodule
#include "Light_WS2812/light_ws2812.h"

/*struct cRGB led[35];
 struct cRGB colors[39]; */
struct cRGB red_wave[42];
struct cRGB blue_wave[42];
//struct cRGB green_wave[42];
//struct cRGB rgb_wave[50];
//struct cRGB black[35];
struct cRGB white[35];
struct cRGB blind[35];
struct cRGB point[35];
struct cRGB flip[92];

struct cRGB *black = (struct cRGB *)3000;

/*uint8_t blind_r[40];
uint8_t blind_g[40];
uint8_t blind_b[40];
#define YELLOW_R 255
#define YELLOW_G 90
#define YELLOW_B 25 */

/*
blind_r[j] = round((double) YELLOW_R * ((double) j / 40.0));
blind_g[j] = round((double) YELLOW_G * ((double) j / 40.0));
blind_b[j] = round((double) YELLOW_B * ((double) j / 40.0));
 */

const uint8_t blind_r[64] PROGMEM = { 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44,
		48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112,
		116, 120, 124, 128, 131, 135, 139, 143, 147, 151, 155, 159, 163, 167,
		171, 175, 179, 183, 187, 191, 195, 199, 203, 207, 211, 215, 219, 223,
		227, 231, 235, 239, 243, 247, 251, 255 };
const uint8_t blind_g[64] PROGMEM = { 1, 3, 4, 6, 7, 8, 10, 11, 13, 14, 15, 17,
		18, 20, 21, 23, 24, 25, 27, 28, 30, 31, 32, 34, 35, 37, 38, 39, 41, 42,
		44, 45, 46, 48, 49, 51, 52, 53, 55, 56, 58, 59, 60, 62, 63, 65, 66, 68,
		69, 70, 72, 73, 75, 76, 77, 79, 80, 82, 83, 84, 86, 87, 89, 90 };
const uint8_t blind_b[64] PROGMEM = { 0, 1, 1, 2, 2, 2, 3, 3, 4, 4, 4, 5, 5, 5,
		6, 6, 7, 7, 7, 8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 13,
		14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 20,
		21, 21, 21, 22, 22, 23, 23, 23, 24, 24, 25, 25 };

void doWaveCycle2(struct cRGB * wave, uint8_t delay, uint8_t delay2,
		uint16_t steps, uint8_t repeat) {
	uint8_t i1;
	for (uint8_t r = 1; r < repeat; r++) {
		for (uint8_t i = 0; i < steps; i++) {
			i1 = i + 1;
			if (i1 == steps) {
				i1 = 0;
			}
			ws2812_setleds((wave + i), 35);
			_delay_ms(delay);

			ws2812_setleds((wave + i1), 35);
			_delay_ms(delay2);
			ws2812_setleds((wave + i), 35);
			_delay_ms(delay2);
			ws2812_setleds((wave + i1), 35);
			_delay_ms(delay2);
			ws2812_setleds((wave + i), 35);
			_delay_ms(delay2);
			ws2812_setleds((wave + i1), 35);
			_delay_ms(delay2);
			ws2812_setleds((wave + i), 35);
			_delay_ms(delay2);
			ws2812_setleds((wave + i1), 35);
			_delay_ms(delay2);
			ws2812_setleds((wave + i), 35);
			_delay_ms(delay2);
		}
	}
}

void doBlind2(struct cRGB * wave, double delay_factor, uint8_t count) {
	for (uint8_t c = 1; c < count; c++) {
		for (uint8_t i = 1; i < 40; i++) {
			for (uint8_t j = 0; j < 35; j++) {
				wave[j].r = pgm_read_word(&blind_r[i]);
				wave[j].g = pgm_read_word(&blind_g[i]);
				wave[j].b = pgm_read_word(&blind_b[i]);
			}
			ws2812_setleds(wave, 35);
			_delay_ms(delay_factor * 2.0);
		}
		for (uint8_t i = 40; i > 0; i--) {
			for (uint8_t j = 0; j < 35; j++) {
				wave[j].r = pgm_read_word(&blind_r[i]);
				wave[j].g = pgm_read_word(&blind_g[i]);
				wave[j].b = pgm_read_word(&blind_b[i]);
			}
			ws2812_setleds(wave, 35);
			_delay_ms(delay_factor * 3.0);
		}
		_delay_ms(delay_factor * 25.0);
	}
}

void doBlackout(double time) {
	ws2812_setleds(black, 35);
	_delay_ms(time);
}

void doLight(double time) {
	ws2812_setleds(white, 35);
	_delay_ms(time);
}

void doStrobe2(uint8_t count, uint8_t delay) {
	for (uint8_t i = 1; i < count; i++) {
		ws2812_setleds(black, delay);
		_delay_ms(50);
		ws2812_setleds(white, delay);
		_delay_ms(50);
	}
}

void doFlipRB(uint8_t count, double delay) {
	for (uint8_t i = 1; i < count; i++) {
		ws2812_setleds(flip, 35);
		_delay_ms(delay);
		ws2812_setleds(flip + 18, 35);
		_delay_ms(delay);
	}
}

void doFlipBG(uint8_t count, double delay) {
	for (uint8_t i = 1; i < count; i++) {
		ws2812_setleds(flip + 36, 35);
		_delay_ms(delay);
		ws2812_setleds(flip + 54, 35);
		_delay_ms(delay);
	}
}

uint8_t randomRange(uint8_t range) {
	return rand() / (RAND_MAX / range + 1);
}

void randomLight(uint8_t mode, uint8_t max3, uint8_t max5, uint8_t max10,
		uint8_t max30) {
	switch (mode) {
	/*case 2:
	 doWaveCycle2(green_wave, max10, max10 / 8, 6, max3);
	 break;*/
	case 3:
		doWaveCycle2(blue_wave, max10, max10 / 8, 6, max3);
		break;
	case 4:
		doWaveCycle2(red_wave, max10, max10 / 8, 6, max3);
		break;
	/*case 5:
		doWaveCycle2(rgb_wave, max30, max30 / 8, 6, max3);
		break; */
	case 6:
		doWaveCycle2(point, max30, max30 / 8, 35, 1);
		break;
	case 7:
		doWaveCycle2(0, max30 / 2, max30 / 16, 1000, 1);
		break;
	case 9:
		doStrobe2(max3, max30 + 40);
		break;
	case 10:
		_delay_ms(max30 * 5);
		break;
	case 11:
		doBlind2(blind, (double) max30 / 25.0, max3);
		break;
	case 12:
		doBlackout(max30 * 10);
		break;
	case 13:
		doLight(max30 * 5);
		break;
	case 15:
		doFlipRB(max3, max30 * 5);
		break;
	case 16:
		doFlipBG(max3, max30 * 5);
		break;
	default:
		break;
	}
}

int main(void) {
	DDRC = 0xFF;
	uint8_t j;

	/*for (j = 0; j < 40; j++) {
		blind_r[j] = round((double) YELLOW_R * ((double) j / 40.0));
		blind_g[j] = round((double) YELLOW_G * ((double) j / 40.0));
		blind_b[j] = round((double) YELLOW_B * ((double) j / 40.0));
	}*/

	for (j = 0; j < 33; j++) {
		point[j].r = 0;
		point[j].g = 0;
		point[j].b = 0;
	}
	point[33].r = 255;
	point[33].g = 255;
	point[33].b = 255;
	point[34].r = 255;
	point[34].g = 255;
	point[34].b = 255;

	for (j = 0; j < 35; j++) {
		black[j].r = 0;
		black[j].g = 0;
		black[j].b = 0;
		white[j].r = 255;
		white[j].g = 255;
		white[j].b = 255;
	}

	for (j = 0; j < 90; j++) {
		flip[j].r = 0;
		flip[j].g = 0;
		flip[j].b = 0;
	}
	for (j = 0; j < 18; j++) {
		flip[j].b = 255;
	}
	for (j = 18; j < 36; j++) {
		flip[j].r = 255;
	}
	for (j = 36; j < 54; j++) {
		flip[j].b = 255;
	}
	for (j = 54; j < 72; j++) {
		flip[j].g = 255;
	}
	for (j = 72; j < 90; j++) {
		flip[j].b = 255;
	}

	for (j = 0; j < 42; j++) {
		red_wave[j].g = 0;
		red_wave[j].b = 0;
	}
	for (j = 0; j < 42; j += 6) {
		red_wave[j].r = 2;
		red_wave[j + 1].r = 30;
		red_wave[j + 2].r = 110;
		red_wave[j + 3].r = 255;
		red_wave[j + 4].r = 110;
		red_wave[j + 5].r = 30;
	}

	for (j = 0; j < 42; j++) {
		blue_wave[j].r = 0;
		blue_wave[j].g = 0;
	}
	for (j = 0; j < 42; j += 6) {
		blue_wave[j].b = 110;
		blue_wave[j + 1].b = 255;
		blue_wave[j + 2].b = 110;
		blue_wave[j + 3].b = 30;
		blue_wave[j + 4].b = 2;
		blue_wave[j + 5].b = 30;
	}

	/*for (j = 0; j < 42; j++) {
	 green_wave[j].r = 0;
	 green_wave[j].b = 0;
	 }
	 for (j = 0; j < 42; j += 6) {
	 green_wave[j].g = 110;
	 green_wave[j + 1].g = 30;
	 green_wave[j + 2].g = 2;
	 green_wave[j + 3].g = 30;
	 green_wave[j + 4].g = 110;
	 green_wave[j + 5].g = 255;
	 }*/

	/*for (j = 0; j < 50; j += 10) {
		rgb_wave[j].r = 1;
		rgb_wave[j + 1].r = 15;
		rgb_wave[j + 2].r = 50;
		rgb_wave[j + 3].r = 100;
		rgb_wave[j + 4].r = 200;
		rgb_wave[j + 5].r = 255;
		rgb_wave[j + 6].r = 150;
		rgb_wave[j + 7].r = 90;
		rgb_wave[j + 8].r = 40;
		rgb_wave[j + 9].r = 10;
		rgb_wave[j].g = 100;
		rgb_wave[j + 1].g = 200;
		rgb_wave[j + 2].g = 255;
		rgb_wave[j + 3].g = 150;
		rgb_wave[j + 4].g = 90;
		rgb_wave[j + 5].g = 40;
		rgb_wave[j + 6].g = 10;
		rgb_wave[j + 7].g = 1;
		rgb_wave[j + 8].g = 15;
		rgb_wave[j + 9].g = 50;
		rgb_wave[j].b = 150;
		rgb_wave[j + 1].b = 90;
		rgb_wave[j + 2].b = 40;
		rgb_wave[j + 3].b = 10;
		rgb_wave[j + 4].b = 1;
		rgb_wave[j + 5].b = 15;
		rgb_wave[j + 6].b = 50;
		rgb_wave[j + 7].b = 100;
		rgb_wave[j + 8].b = 200;
		rgb_wave[j + 9].b = 255;
	}*/

	while (1) {
		PORTC ^= 0xFF;

		randomLight(randomRange(32), 2 + randomRange(3), 3 + randomRange(5),
				5 + randomRange(10), 15 + randomRange(30));
	}
}

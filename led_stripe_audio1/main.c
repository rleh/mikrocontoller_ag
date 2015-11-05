/*------------------------------------------------*/
/* FFTEST : A test program for FFT module         */

#ifndef F_CPU
#define F_CPU 14745600
#endif

#include <string.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "ffft.h"		/* Defs for using Fixed-point FFT module */
#include "mydefs.h"

#include <stdlib.h>

#include <avr/io.h>
#include <avr/iom16.h> // Eclipse makefile does not work :/
#include "ws2812_config.h"
#include "Light_WS2812/light_ws2812.h"

/*------------------------------------------------*/
/* Global variables                               */

int16_t capture[FFT_N];
complex_t bfly_buff[FFT_N];
uint16_t spektrum[FFT_N/2];
uint16_t pwm_setting[6];
uint16_t ws2812_setting[6];
uint8_t counter[6];
volatile uint8_t peak[6];

const uint16_t pwmtable_10[64]  PROGMEM = { 0, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 5,
											5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 15, 17,
											19, 21, 23, 26, 29, 32, 36, 40, 44, 49, 55,
											61, 68, 76, 85, 94, 105, 117, 131, 146, 162,
											181, 202, 225, 250, 279, 311, 346, 386, 430,
											479, 534, 595, 663, 739, 824, 918, 1023 };
const uint16_t pwmtable_8[64]  PROGMEM = { 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
											2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4,
											5, 5, 6, 7, 7, 8, 9, 10, 11, 12, 14,
											15, 17, 19, 21, 24, 26, 29, 33, 37, 41,
											45, 51, 57, 63, 70, 78, 87, 97, 108,
											120, 134, 149, 166, 185, 206, 230, 255 };

// LEDs
volatile struct cRGB led[35];

/*------------------------------------------------*/
/* Timer 1 Output COMPARE A Interrupt             */
 
ISR( TIMER1_COMPA_vect ) {
	static uint16_t pwm_cnt = 0;
	uint8_t tmp = 0;

    OCR1A += (uint16_t) T_PWM;

	if(pwm_setting[0] > pwm_cnt) tmp |= LED1;
	if(pwm_setting[1] > pwm_cnt) tmp |= LED2;
	if(pwm_setting[2] > pwm_cnt) tmp |= LED3;
	if(pwm_setting[3] > pwm_cnt) tmp |= LED4;
	if(pwm_setting[4] > pwm_cnt) tmp |= LED5;
	if(pwm_setting[5] > pwm_cnt) tmp |= LED6;

	LED_PORT = tmp;

	if(pwm_cnt == (PWM_STEPS - 1))
        pwm_cnt = 0;
    else
		pwm_cnt++;

	// muss aus Performancegründen leider mit in die ISR
	if((pwm_cnt == 512) || (pwm_cnt == 0)) {
		if(counter[0] == 0) 
			if(peak[0]) peak[0]--;									// LED's zurückfaden

		if(counter[1] == 0) 
			if(peak[1]) peak[1]--;

		if(counter[2] == 0) 
			if(peak[2]) peak[2]--;

		if(counter[3] == 0) 
			if(peak[3]) peak[3]--;

		if(counter[4] == 0) 
			if(peak[4]) peak[4]--;

		if(counter[5] == 0) 
			if(peak[5]) peak[5]--;

	}
}

/*------------------------------------------------*/
/* Capture waveform                               */

void capture_wave(int16_t *buffer, uint16_t count) {
	ADMUX = _BV(REFS0)|_BV(ADLAR)|_BV(MUX2)|_BV(MUX0);				// channel

	do {
		#if defined (__AVR_ATmega8__)
			ADCSRA = _BV(ADEN)|_BV(ADSC)|_BV(ADFR)|_BV(ADIF)|_BV(ADPS2)|_BV(ADPS1);
		#elif defined (__AVR_ATmega32__) || defined (__AVR_ATmega644__) || defined (__AVR_ATmega16__)
			ADCSRA = _BV(ADEN)|_BV(ADSC)|_BV(ADATE)|_BV(ADIF)|_BV(ADPS2)|_BV(ADPS1);
		#endif
		while(bit_is_clear(ADCSRA, ADIF));
		*buffer++ = ADC - 32768;
	} while(--count);

	ADCSRA = 0;
}

/*------------------------------------------------*/
/*                                                */

int main (void) {
	uint16_t s;

	LED_DIR  |=  (LED1 | LED2 | LED3 | LED4 | LED5 | LED6);
	LED_PORT &= ~(LED1 | LED2 | LED3 | LED4 | LED5 | LED6);

	TCCR1B = (1 << CS10);											// Timer läuft mit vollem Systemtakt
	TIMSK |= (1 << OCIE1A);											// Interrupt freischalten

	sei();

	for(;;) {
		capture_wave(capture, FFT_N);
		fft_input(capture, bfly_buff);
		fft_execute(bfly_buff);
		fft_output(bfly_buff, spektrum);

		// Array in benötigter Größe vor Wiederverwendung 'säubern'
		memset(capture, 0, 12);

		// Berechnung der einzelnen Bänder, ohne zunächst offensichtlichem System,
		// ergab sich so aus den Unzulänglichkeiten des AD-Wandlers,
		// (hab's versucht, mit rosa Rauschen so in etwa hinzubiegen)
		for (uint8_t n = 0; n < FFT_N / 2; n++) {
			s = spektrum[n];

			if(n == 0) {											// tiefstes Frequenzband
				if(s <= SCORE_MAX) capture[0] = s;
				else capture[0] = SCORE_MAX;
			}

			if((n >= 1)  && (n <= 3)) {
				if((capture[1] + s) <= SCORE_MAX) capture[1] += s;
				else capture[1] = SCORE_MAX;
			}

			if((n >= 4)  && (n <= 11)) {
				if((capture[2] + s) <= SCORE_MAX) capture[2] += s;
				else capture[2] = SCORE_MAX;
			}

			if((n >= 12) && (n <= 21)) {
				if((capture[3] + s) <= SCORE_MAX) capture[3] += s;
				else capture[3] = SCORE_MAX;
			}

			if((n >= 22) && (n <= 34)) {
				if((capture[4] + s) <= SCORE_MAX) capture[4] += s;
				else capture[4] = SCORE_MAX;
			}

			if((n >= 35) && (n <= 63)) {							// h�chstes Frequenzband
				if((capture[5] + s) <= SCORE_MAX) capture[5] += s;
				else capture[5] = SCORE_MAX;
			}
		}

		for(uint8_t y = 0; y < 6; y++) {
			if(capture[y] > THRESHOLD) capture[y] -= THRESHOLD;		// Schwellwert abziehen
			else capture[y] = 0;

			capture[y] /= 256;										// Ergebnis teilen

			if(capture[y] > 63) capture[y] = 63;					// auf 64 Stufen begrenzen (Murphy's Law)

			if(counter[y]) counter[y]--;							// vor dem zur�ckfaden der LED's etwas warten

			if(capture[y] >= peak[y]) {								// so 'ne Art PeakHold
				peak[y] = capture[y];
				counter[y] = 3;
			}

			cli();
			pwm_setting[y] = pgm_read_word(&pwmtable_10[peak[y]]);	// Helligkeitsstufe zuweisen
			ws2812_setting[y] = pgm_read_word(&pwmtable_8[peak[y]]);	// Helligkeitsstufe zuweisen
			sei();

			led[y].r = ws2812_setting[y];
			led[y].g = ws2812_setting[y];
			led[y].b = ws2812_setting[y];
		}
		ws2812_setleds(led, 6);
	}
}

/*
 * main.c
 *
 *  Created on: 07.07.2015
 *      Author: raphael
 */


#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/iom16.h>
#include <util/delay.h>

void init_adc();
void init_pwm();

void led_blink_init();
void led_chaser_init();
void led_blink_step();
void led_chaser_step();

volatile uint8_t temp = 0b11110000;
volatile uint8_t poti;
uint8_t pwm = 0;

uint8_t led_mode = 2; // invalid value -> led mode is evaluated in while loop
uint8_t led_chaser_dir = 1; // init value
uint8_t led_port = 0b00000001; // init value

int main() {
	init_adc();
	init_pwm();
	sei();

	DDRC = 0xFF; // Port C: output
	PORTC = 0x00; // Port C: leds on

	while(1) {
		// Calculate PWM
		pwm = (uint8_t)( (float)temp * (float)poti / 255.0 );
		// Switch LED mode?
		if(led_mode != (pwm > 200)) {
			led_mode = (pwm > 200);
			if(led_mode) {
				led_blink_init();
			}
			else {
				led_chaser_init();
			}
		}
		// Do a led animation step
		if(led_mode) {
			led_blink_step();
		}
		else {
			led_chaser_step();
		}
		// Set PWM
		OCR2 = pwm;
		// LED speed depends on temperature
		_delay_ms(300-OCR2);
	}
	return 0;
}


/* ADC
 * PA3: NTC
 * --> MUX1 | MUX2
 * PA7: Poti (1)
 * --> MUX1 | MUX2 | MUX3
 */
void init_adc() {
	// Enable; Interrupt;; Prescaler 128:
	ADCSRA = _BV(ADEN) | _BV(ADIF) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	// Vref = AVcc; Left adjust result;
	ADMUX = _BV(REFS0) | _BV(ADLAR);
	// Select channel 3 for temp sensor:
	ADMUX |= _BV(MUX0) | _BV(MUX1);
	// start conversation
	ADCSRA |= _BV(ADSC);
}

void read_adc() {
	// change ADC channel and write Value to variable
	if(ADMUX & _BV(MUX2)) {
		poti = ADCH;
		// delete MUX2 bit -> ADC channel 3
		ADMUX &= ~(_BV(MUX2));
	}
	else {
		temp = ADCH;
		// set MUX2 bit -> ADC channel 7
		ADMUX |= _BV(MUX2);
	}
	// start next conversation
	ADCSRA |= _BV(ADSC);
}

ISR(ADC_vect) {
	read_adc();
}

/*PWM
 * Timer2 -> Pin OC2/PD7
 * Frequency ~25kHz (PC fan)
 * ==> 14.7506MHz / (510*1) = 28.92kHz
 *  ==> prescaler 1
 */
void init_pwm() {
	// PD7 output
	DDRD |= (1 << PD7);
	// Enable Phase Correct PWM
	TCCR2 = _BV(WGM20);
	// Set OC2=PB7 on compare match whenup-counting. Clear OC2=PB7 on compare match when downcounting
	TCCR2 |= _BV(COM21) | _BV(COM21);
	// prescaler 1
	TCCR2 |= _BV(CS20);
	// initial compare value
	OCR2 = 0;
}

// Led animations:
void led_blink_init() {
	// leds off
	PORTC = 0xFF;
}

void led_chaser_init() {
	// empty
}

void led_blink_step() {
	// toggle leds
	PORTC ^= 0xFF;
}

void led_chaser_step() {
	if(led_chaser_dir) {
		led_port <<= 1;
		if(led_port == 0b10000000) {
			// toggle led_chaser_dir
			led_chaser_dir = 0;
		}
	}
	else {
		led_port >>= 1;
		if(led_port == 0b00000001) {
			// toggle led_chaser_dir
			led_chaser_dir = 1;
		}
	}
	PORTC = ~led_port;
}


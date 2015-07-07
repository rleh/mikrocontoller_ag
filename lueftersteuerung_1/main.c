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

volatile uint8_t temp = 0b11110000;
volatile uint8_t poti;


int main() {
	init_adc();
	init_pwm();
	sei();

	DDRC = 0xFF; // Port C: output
	PORTC = 0b01010101; // Port C: leds on

	while(1) {
		//PORTC = ~temp;
		OCR2 = (uint8_t)( (float)temp * (float)poti / 255.0 );
		PORTC = ~OCR2;
		_delay_ms(100);
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
	DDRD |= _BV(PD7);
	// Enable Phase Correct PWM
	TCCR2 = _BV(WGM20);
	// PB7 HIGH on compare match
	TCCR2 |= _BV(COM21);
	// prescaler 1 (nothing to do)
	// initial compare value
	OCR2 = 142;
}

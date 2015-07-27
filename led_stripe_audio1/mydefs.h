#ifndef __MYDEFS_H
#define __MYDEFS_H

#if defined (__AVR_ATmega644__) || defined (__AVR_ATmega168__)
#define TIMSK			TIMSK1
#endif

#define LED_PORT		PORTD
#define LED_DIR			DDRD

#define LED1			(1 << PD0)
#define LED2			(1 << PD1)
#define LED3			(1 << PD2)
#define LED4			(1 << PD5)
#define LED5			(1 << PD6)
#define LED6			(1 << PD7)

#define THRESHOLD		2000								// Schwellwert Anzeige

/*so werfen wir zwar fast ein Bit in der Auflösung weg,       
ist aber egal, später wird noch viel mehr 'entsorgt',   
der OPV wird vermutlich eh nicht viel weiter aussteuern können*/
#define SCORE_MAX		((INT16_MAX / 2) + THRESHOLD)

#define F_PWM			60UL								// PWM-Frequenz in Hz
#define PWM_STEPS		1024								// PWM-Schritte pro Zyklus

#define T_PWM			(F_CPU / (F_PWM * PWM_STEPS))		// Systemtakte pro PWM-Takt
#if (T_PWM < (93 + 5))
    #error T_PWM zu klein, F_CPU muss vergrösst werden oder F_PWM oder PWM_STEPS verkleinert werden
#endif

#endif


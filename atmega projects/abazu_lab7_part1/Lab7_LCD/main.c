/*	Author: abazu001
 *  Partner(s) Name:
 *	Lab Section: 23
 *	Assignment: Lab #7  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"

typedef enum States {init, wait, increment, decrement, reset, holdInc, holdDec} States;

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0;   // Current internal count of 1 ms ticks.

void TimerOn() {
    TCCR1B = 0x0B;

    OCR1A = 125;

    TIMSK1 = 0x02;

    TCNT1=0;

    _avr_timer_cntcurr = _avr_timer_M;

    SREG |= 0x80;

}

void TimerOff() {
    TCCR1B = 0x00;
}

void TimerISR() {
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
    // CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
    _avr_timer_cntcurr--;   // Count down to 0 rather than up to TOP
    if (_avr_timer_cntcurr == 0) {
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }

}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

int Tick(int state) {
    static unsigned char count;
    static unsigned char timeCntr;

    unsigned char A0 = ~PINA & 0x01;
    unsigned char A1 = ~PINA & 0x02;

    switch (state) {
        case init:
            state = wait;
            break;

        case wait:
            if (A0 && !A1) { state = increment; }
            else if (!A0 && A1) { state = decrement; }
            else if (A0 && A1) { state = reset; }
            else { state = wait; }
            break;

        case increment:
            if (A0 && !A1) { state = holdInc; }
            else if (A0 && A1) { state = reset; }
            else if (!A0 && !A1) { state = wait; }
            break;

        case decrement:
            if (!A0 && A1) { state = holdInc; }
            else if (A0 && A1) { state = reset; }
            else if (!A0 && !A1) { state = wait;}
            break;

        case reset:
            state = (A0 || A1) ? reset: wait;
            break;

        case holdInc:
            if (timeCntr >= 2) { state = increment; }
            else if (A0 && !A1) { state = holdInc; }
            else if (A0 && A1) {state = reset; }
            else if (!A0 && !A1) {state = wait; }
            break;

        default:
            state = init;
            break;

    }

    switch (state) {
        case init:
            count = 0;
            timeCntr = 0;
            break;

        case wait:
            timeCntr = 0;
			LCD_DisplayString(5, "wait");
            break;

        case increment:
			LCD_DisplayString(5, "inc");
            ++count;
            ++timeCntr;
            break;

        case decrement:

            break;

        case reset:

            break;

        case holdInc:
			LCD_DisplayString(5, "hInc");
			++timeCntr;
            if (timeCntr > 2) {
				timeCntr = 0;
			}
            break;
			
        case holdDec:

            break;
    }

	LCD_Cursor(1);
    LCD_WriteData(count + '0');
    LCD_Cursor(10);
    LCD_WriteData(timeCntr + '0');


    return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */

    TimerSet(250);
    TimerOn();
    LCD_init();

    States state = init;

    while (1) {
        state = Tick(state);
        while (!TimerFlag) {}
        TimerFlag = 0;
    }
    return 1;
}

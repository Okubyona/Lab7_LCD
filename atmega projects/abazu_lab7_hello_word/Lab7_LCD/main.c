/*
 * Lab7_LCD.c
 *
 * Created: 11/8/2019 12:33:27 PM
 * Author : Okubyona
 */ 

#include <avr/io.h>
#include "io.h"
#include "io.c"


int main(void) {
    DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	LCD_init();
	
	LCD_DisplayString(1, "Hello World");
    while (1) { continue;
    }
}


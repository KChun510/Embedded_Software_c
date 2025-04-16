/**
 * avr.c
 * Copyright (C) 2001-2020, Tony Givargis
 */

#include "avr.h"

void avr_wait(unsigned short msec)
{
	TCCR0 = 3; // Start timer with pre-scaling 8mHz / 64
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001048); // Pre-filling bucket 
		SET_BIT(TIFR, TOV0); // TIFR ( Time Interrupt Flag Reg  ), TOV0 ( Index 0, of TIFR. Flips to one when timer/ overflow hit )
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0; // Turn off timer
}
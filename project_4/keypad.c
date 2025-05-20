/*
 * keypad.c
 *
 * Created: 4/21/2025 1:15:56 PM
 *  Author: westcoast
 */ 
#include "keypad.h"

const uint8_t key_map[4][4] = {
		{ '1', '2', '3', 'A'},
		{ '4', '5', '6', 'B'},
		{ '7', '8', '9', 'C'},
		{ '*', '0', '#', 'D'}
};

int is_pressed(int r, int c){
	// Need to set col to LOW
	// So that when button pressed, row also reads LOW
	// C is going to be a val from 0-4. Need to add offset.
	// Add +4 to C, to access corresponding bit.
	unsigned char button_pressed = 0;
	CLR_BIT(PORTC, c);
	avr_wait(1); // Need to wait for low to settle before continue
	
	if(!GET_BIT(PINC, r)){
		button_pressed = key_map[r][c - 4];
	}
	SET_BIT(PORTC, c);
	return button_pressed;
}

// In the physical world, you want to set col for outer loop.
// Set the keypad col to LOW for a longer period of time.
int get_key(){
	unsigned char r,c;
	uint8_t pressed;
	for(c = 4; c < 8; ++c){
		for(r = 0; r < 4; ++r){
			pressed = is_pressed(r, c);
			if(pressed){
				return pressed;
			}
		}
	}
	return 0;
}

void keypad_init(){
		// Using A series pins for Keypad
		// Pins (5-8) col/output, (4-1) row/input
		DDRC = DDRC | 0xF0;	
		// Need row pins to be pulled up, and cols as HIGH
		PORTC = PORTC | 0xFF;
}
/*
 * project_2.c
 *
 * Created: 4/15/2025 9:14:00 PM
 * Author : westcoast
 */ 

#include <avr/io.h>
#include <stdbool.h>
#include "avr.h"

void lcd_pulse_enable(){
	// Enable Pin, set on pin 2
	SET_BIT(PORTD, 2);
	avr_wait(1);
	CLR_BIT(PORTD, 2);
	avr_wait(1);
}

void send_lcd_data(unsigned char data, bool instruc){
	if(instruc){
		CLR_BIT(PORTD, 0);
	}else{
		SET_BIT(PORTD, 0);
	}
	avr_wait(1);
	// Cap first and second nib. 
	// And align with data pins.
	unsigned char first_nib = ((data >> 4) & 0x0F) << 3;
	unsigned char sec_nib = (data & 0x0F) << 3;
	
	// First clear data line, and keep config settings
	PORTD = PORTD & 0X07;
	
	// Send First nibble
	PORTD = PORTD | first_nib;
	lcd_pulse_enable();
	// Clear bits for next nibble
	PORTD = PORTD & 0X07;
	// Send Second nibble
	PORTD = PORTD | sec_nib;
	avr_wait(1);
	lcd_pulse_enable();	
}

void init_lcd(){
	// Need to put it into "Instruction Mode, DataWrite"
	PORTD = 0X00;
	send_lcd_data(0x28, true); // Set to 4 bit mode
	send_lcd_data(0x0C, true); // Display ON
	send_lcd_data(0x06, true); // Entry mode
	send_lcd_data(0x01, true); // Clear screen
	avr_wait(1);
}

void check_push(){
	if(!(PINB & 0x02)){
		PORTB = 0x01;
		avr_wait(500);
		PORTB = 0x00;
		avr_wait(500);
	}else{
		PORTB = 0x00;
	}
}

int is_pressed(int r, int c){
	// Need to set col to LOW
	// So that when button pressed, row also reads LOW
	// C is going to be a val from 0-4. Need to add offset.
	// Add +4 to C, to access corresponding bit.
	unsigned char button_pressed = 0;
	CLR_BIT(PORTC, c);
	avr_wait(1); // Need to wait for low to settle before continue
	
	if(!GET_BIT(PINC, r)){
		button_pressed = 1;
	}
	SET_BIT(PORTC, c);
	return button_pressed;
}

// In the physical world, you want to set col for outer loop. 
// Set the keypad col to LOW for a longer period of time.
int get_key(){
	unsigned char r,c;
	for(c = 4; c < 8; ++c){
		for(r = 0; r < 4; ++r){
			if(is_pressed(r,c)){
				return 1;
			}
		}
	}
	return 0;
}

int main(void)
{
	// Using B series pins for push button and LED
    DDRB = DDRB | 0x01;
	// Using A series pins for Keypad
	// Pins (5-8) col/output, (4-1) row/input
	DDRC = DDRC | 0xF0;
	// D series, used for LCD.
	// All pins OUTPUT, except pin 8 unused.
	DDRD = DDRD | 0x7F;
	// Need row pins to be pulled up, and cols as HIGH
	PORTC = PORTC | 0xFF;
	
	init_lcd();
	
    while (1) {
		check_push();
		if(get_key()){
			send_lcd_data(0x4B, false);
			PORTB = 0x01;
			avr_wait(500);
			PORTB = 0x00;
			avr_wait(500);
		}
    }
	return 0;
}

// Use PORT for writing
// USE PIN for reading
/*
* Concept of Pull (up and down), applies to pins that are set as input.
* Writing to a pin thats set for input, changes its pull value
* Writing 1 = pull up (Pin set to HIGH)
* Writing 0 = pull down (Pin set to LOW)
*/
	
// DDR Convention:
/*
* Set pin to 0, for INPUT
* Set pin to 1, for OUTPUT	
*/
	


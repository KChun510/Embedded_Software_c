/*
 * project_2.c
 *
 * Created: 4/15/2025 9:14:00 PM
 * Author : westcoast
 */ 
#include <avr/io.h>
#include "avr.h"
#include "lcd.h"
#include "keypad.h"

unsigned char left = 0;
unsigned char right = 1;
unsigned char down = 1;
unsigned char up = 0;

void check_reset_lcd(){
	avr_wait(2000);
	if(get_key() == '*'){
		lcd_write_default();
	}
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

void move_lcd_cursor(uint8_t *key_pad_input){
	switch(*key_pad_input){
		case 'A':
			lcd_pos(&up, NULL);
			break;
		case 'B':
			lcd_pos(&down,NULL);
			break;
		case 'C':
			lcd_pos(NULL, &left);
			break;
		case 'D':
			lcd_pos(NULL, &right);
			break;
	}
}


int main(void)
{
	// Using B series pins for push button and LED
	DDRB = DDRB | 0x01;

	unsigned char key_pad_input;
	keypad_init();
	lcd_init();
	
    while (1) {
		key_pad_input = get_key();
		if(key_pad_input == '*'){
			check_reset_lcd();	
		}else if (key_pad_input >= 0x41 && key_pad_input <= 0x44){
			move_lcd_cursor(&key_pad_input);
			avr_wait(250);
		}
		else if(key_pad_input){
			lcd_put(key_pad_input);
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
	


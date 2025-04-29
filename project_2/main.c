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

unsigned char lcd_is_set = 0;

void check_reset_lcd(){
	avr_wait(2000);
	if(get_key() == '*'){
		reset_lcd();
		lcd_is_set = 0;
	}
}

// During set mode. 
// Pressing #, will change from AM to PM
// Holding #, will set the clock
void check_hash_lcd(){
	avr_wait(1000);
	if(get_key() == '#'){
		set_lcd();
		lcd_is_set = 1;
		avr_wait(500);
	}else{
		change_am_pm();
		avr_wait(500);
	}
}

void move_lcd_cursor(uint8_t *key_pad_input){
	switch(*key_pad_input){
		case 'A':
			lcd_cursor_pos(&up, NULL);
			break;
		case 'B':
			lcd_cursor_pos(&down,NULL);
			break;
		case 'C':
			lcd_cursor_pos(NULL, &left);
			break;
		case 'D':
			lcd_cursor_pos(NULL, &right);
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
		if(lcd_is_set){
			increment_cal();
		}
		if(key_pad_input == '*'){
			check_reset_lcd();		
		}else if (key_pad_input == '#'){
			check_hash_lcd();
		}else if (key_pad_input >= 0x41 && key_pad_input <= 0x44){
			move_lcd_cursor(&key_pad_input);
			avr_wait(250);
		}
		else if(key_pad_input){
			lcd_update(key_pad_input);
			avr_wait(250);
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
	


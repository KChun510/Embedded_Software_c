/*
 * project_2.c
 *
 * Created: 4/15/2025 9:14:00 PM
 * Author : westcoast
 */ 
#include <avr/io.h>
#include <stdlib.h>
#include "avr.h"
#include "lcd.h"
#include "keypad.h"
#include "speaker.h"
#include "music.h"


unsigned char left = 0;
unsigned char right = 1;
unsigned char down = 1;
unsigned char up = 0;
unsigned char lcd_is_set = 0;

// Note: Adjustments to the input cycle count must be adjusted, to a account for cycles
// Taken in loop.
void wait_cycle_count(uint16_t cycle_count){
	for(int i = 0; i < cycle_count; ++i){
		//125ns I.e: 1/8Mhz
		NOP();
	}
}

void check_reset_lcd(){
	avr_wait(2000);
	if(get_key() == '*'){
		reset_lcd();
		lcd_is_set = 0;
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

void play_note(const PlayingNote* note_data){
	NoteTiming res = get_note_timing(note_data);
	const uint16_t cycle_count = (res.TH * 8) / 6;
	for(int i = 0; i < res.K ; ++i){
		SET_BIT(PORTB, 2);
		wait_cycle_count(cycle_count);
		CLR_BIT(PORTB, 2);
		wait_cycle_count(cycle_count);
	}
}

void play_song(Track song){
	for(uint8_t i = 0; i < song.length; ++i){
		play_note(&song.song[i]);
	}
}

// During set mode.
// Pressing #, will change from AM to PM
// Holding #, will set the clock
void check_hash_lcd(unsigned char* game_start){
	*game_start = !*game_start;
	SET_BIT(PORTB, 0);
	avr_wait(500);
	CLR_BIT(PORTB, 0);
}

void check_button_press(unsigned char* game_live){
	*game_live = !*game_live;
	SET_BIT(PORTB, 0); // Test, pulse lcd
	avr_wait(500);
	CLR_BIT(PORTB, 0);
}

void flash_memo(char *memo_line, unsigned char game_level){
	for(uint8_t i = 0; i < 3; ++i){
		for(uint8_t j = 0; j < 16; ++j){
			lcd_update(memo_line[j]);
		}
		avr_wait(1000);
		lcd_clr();
		avr_wait(1000);
	}
	
	lcd_clr();
	for(uint8_t i = 0; i < 16; ++i){
		if(i <= (game_level - 1)){
			lcd_update(0xFF);
			}else{
			lcd_update(' ');
		}
	}
	
	avr_wait(500);
	lcd_cursor_pos(1,1);
}

int main(void)
{
	// Using B series pins for push button and LED
	DDRB = DDRB | 0x05;
	
	unsigned char game_level = 1;
	unsigned char game_live = 0;
	unsigned char key_pad_input;
	char key_opt[16] = {'1', '2', '3', '4', '5' , '6', '7', '8' , '9', '0', '#', '*', 'A', 'B', 'C', 'D'};


	srand(TCNT0);
	keypad_init();
	lcd_init();
	// Lcd on, but turn off cursor blink
	send_lcd_data(0x0C, true);
	
    while (1) {
		key_pad_input = get_key();
		// Use this for start game	
		if (key_pad_input == '#'){
			check_hash_lcd(&game_live);
		}
		// Need to put a cap on game_level, at 16 later.
		while(game_live){
			// Turn blink back on
			send_lcd_data(0x0F, true);
			lcd_clr();
			unsigned char user_input = 0;
			char memo_line[16] = {' ', ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
			char user_line[16] = {' ', ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
			for(uint8_t i = 0; i < game_level; ++i){
				memo_line[i] = key_opt[rand() % 16];			
			}
			
			flash_memo(&memo_line, game_level);
			
			while(user_input < game_level){
				key_pad_input = get_key();
				if(key_pad_input){
					user_line[user_input] = key_pad_input;
					lcd_update(key_pad_input);
					++user_input;
					avr_wait(250); 
				}
			}
			
			for(uint8_t i = 0; i < game_level; ++i){
				if(memo_line[i] != user_line[i]){
					game_live = 0;
				}	
			}
			// Level cleared
			if(game_live && game_level < 16){
				lcd_clr();
				char lvl_msg[16];
				sprintf(lvl_msg, "Level %u cleared", game_level);
				lcd_puts2(lvl_msg);
				play_song(music_track[0]);
				avr_wait(2000);
				++game_level;
			// All levels beat
			}else if(game_live && game_level == 16){
				lcd_clr();
				char lvl_msg[16];
				sprintf(lvl_msg, "You WIN!!!", game_level);
				lcd_puts2(lvl_msg);
				play_song(music_track[6]);
				avr_wait(2000);
				lcd_write_default();
				game_level = 1;
				game_live = 0;
			// Level Failed
			}else{ 			
				lcd_clr();
				char lvl_msg[16];
				sprintf(lvl_msg, "Level %u failed", game_level);
				lcd_puts2(lvl_msg);
				play_song(music_track[1]);
				avr_wait(2000);				
				lcd_write_default();
				game_level = 1;
			}
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
	


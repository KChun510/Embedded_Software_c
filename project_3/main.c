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

// During set mode.
// Pressing #, will change from AM to PM
// Holding #, will set the clock
void check_hash_lcd(uint8_t* play_pause){
	*play_pause = !(*play_pause);
	if(*play_pause){
		SET_BIT(PORTB, 0);
	}else{
		CLR_BIT(PORTB, 0);
	}
	avr_wait(500);
}

void play_song(Track* song){
	for(size_t note_i = 0; note_i < song->length; ++note_i){
			play_note(&song->song[note_i]);
	}
}

void check_button_press(Track* curr_song){
	//PlayingNote note_data = {A , W};
	SET_BIT(PORTB, 0);
	play_song(curr_song);
}

void change_song(uint8_t* curr_song_index, Track* curr_song, int8_t inc_or_dec){
	uint8_t song_index = *curr_song_index + inc_or_dec;
	if(song_index == 255){
		song_index = song_count - 1;
	}else if(song_index >= song_count){
		song_index = 0;
	}
	*curr_song = music_track[song_index];
	*curr_song_index = song_index;
}

int main(void)
{
	// Using B series pins for push button and LED
	DDRB = DDRB | 0x05;
	
	uint8_t curr_song_index = 0;
	Track curr_song = music_track[curr_song_index];
	uint8_t play_pause = 0;
	size_t note_i = 0;
	unsigned char key_pad_input;
	
	keypad_init();
	lcd_init(curr_song.song_name);
	
    while (1) {
		key_pad_input = get_key();
		if(!(PINB & 0X02)){
			check_button_press(&curr_song);
		}
		// Use this for Play/ Pause		
		else if (key_pad_input == '#'){
			check_hash_lcd(&play_pause);
		// Buttons A and B, used for left and right.
		}else if (key_pad_input == 0x41 || key_pad_input == 0x42){
			if(key_pad_input == 0x41){
				change_song(&curr_song_index, &curr_song, -1);
			}else{
				change_song(&curr_song_index, &curr_song, 1);
			}
			lcd_write_default(curr_song.song_name);
			avr_wait(250);
		}
		if(play_pause){
			if(note_i < curr_song.length){
				play_note(&curr_song.song[note_i]);
				++note_i;
			}else{
				note_i = 0;
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
	


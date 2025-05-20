/*
 * project_2.c
 *
 * Created: 4/15/2025 9:14:00 PM
 * Author : westcoast
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

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

volatile uint16_t adc_result = 0;
volatile uint16_t v_max = 0;
volatile uint16_t v_min = 1023;
volatile uint32_t v_sum = 0;
volatile uint16_t sample_count = 0;

void adc_init(void) {
	ADMUX = (1 << REFS0);  // AVCC as reference, ADC0 as input (PA0)
	ADCSRA = (1 << ADEN)                 // Enable ADC
	| (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler = 128
}

uint16_t read_adc(void) {
	ADCSRA |= (1 << ADSC);                // Start conversion
	while (ADCSRA & (1 << ADSC));         // Wait until complete
	return ADC;                           // Return 10-bit value
}

void timer1_init(void) {
	TCCR1B = (1 << WGM12) | (1 << CS12);  // CTC mode, prescaler = 256
	OCR1A = 15624;                        // 8MHz / 256 = 31,250 ? 31,250 * 0.5s = 15,625
	TIMSK |= (1 << OCIE1A);               // Enable Timer1 Compare Match A interrupt
}

ISR(TIMER1_COMPA_vect) {
	adc_result = read_adc();  // Read ADC0 (PA0)

	if (sample_count == 0) {
		v_max = v_min = adc_result;
		} else {
		if (adc_result > v_max) v_max = adc_result;
		if (adc_result < v_min) v_min = adc_result;
	}

	v_sum += adc_result;
	sample_count++;
}

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

void check_button_press(Track* curr_song){
	//PlayingNote note_data = {A , W};
	SET_BIT(PORTB, 0);
	
}

void adc_to_volt(uint16_t adc_data, char *buffer) {
	uint16_t millivolts = (adc_data * 500UL) / 1023;
	uint8_t volts = millivolts / 100;
	uint8_t hundredths = millivolts % 100;
	sprintf(buffer, "%u.%02u", volts, hundredths);  // Store result in caller's buffer
}


int main(void)
{
	// Using B series pins for push button and LED
	DDRB = DDRB | 0x05;
	keypad_init();
	adc_init();
	timer1_init();
	sei();
	unsigned char key_pad_input;
	lcd_init();
	
	char inst_volt_buff[8];
	char max_volt_buff[8];
	char min_volt_buff[8];
	char avg_volt_buff[8];
	
	char lcd_line0[16];
	char lcd_line1[16];
		
	uint16_t max_volt = 0;
	uint16_t min_volt = 1023;
	uint16_t avg_volt = 0;
	uint8_t sample_state = 1;
	
    while (1) {
		key_pad_input = get_key();
		if (key_pad_input == 'A') ++sample_state;
		if(sample_state == 1){
				max_volt = 0;
				min_volt = 1023;
				avg_volt = 0;
		}
		else if(sample_state == 2){
			if(adc_result > max_volt) max_volt = adc_result;
			if(adc_result < min_volt) min_volt = adc_result;
			avg_volt = (max_volt  + min_volt) / 2;
		}else if(sample_state > 2){
			sample_state = 1;
		}
		if(sample_state == 1){
			sprintf(max_volt_buff, "----");
			sprintf(min_volt_buff, "----");
			sprintf(avg_volt_buff, "----");
		}else{
			adc_to_volt(max_volt, max_volt_buff);
			adc_to_volt(min_volt, min_volt_buff);
			adc_to_volt(avg_volt, avg_volt_buff);
		}
		adc_to_volt(adc_result, inst_volt_buff);
		sprintf(lcd_line0, "in:%s max:%s", inst_volt_buff, max_volt_buff);
		sprintf(lcd_line1, "mi:%s avg:%s", min_volt_buff, avg_volt_buff);
		const char* lcd_lines[2] = {lcd_line0, lcd_line1};
		lcd_write_default(lcd_lines);
		avr_wait(200);
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
	


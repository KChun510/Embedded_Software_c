/*
 * lcd.c
 *
 * Created: 4/21/2025 11:46:18 AM
 *  Author: westcoast
 */ 
#include "avr.h"
#include "lcd.h"
#include <string.h>

const char line1[] PROGMEM = "MM/DD/YYYY   OFF";
const char line2am[] PROGMEM = "HH:MM:SSam    ST";
const char line2pm[] PROGMEM = "HH:MM:SSpm    ST";
const char line2mil[] PROGMEM = "HH:MM:SS    24hr";
char display_mat[2][50] = {"MM/DD/YYYY   OFF", "HH:MM:SSam    ST"};
uint8_t is_leap_year = 0;
uint8_t past_twelve = 0;

static uint8_t row0_col = 0x80;
static uint8_t row1_col = 0xC0;
uint8_t LCD_ROW_1 = 1;
uint8_t write_one = 1;
static uint8_t *curr_addr = &row0_col;


uint8_t i_month;
uint8_t i_day;
// NEED to split year into two halves. 2^8 = 256 < 2020
uint8_t i_year_first;
uint8_t i_year_second;
uint8_t i_hour;
uint8_t i_min;
uint8_t i_sec;

static inline void
sleep_700ns(void)
{
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	return;
}

void lcd_pulse_enable(){
	// Enable Pin, set on pin 2
	SET_BIT(PORTD, 2);
	avr_wait(1);
	CLR_BIT(PORTD, 2);
	avr_wait(1);
}

void print_hex(uint8_t byte){
	const char hex_chars[] = "0123456789ABCDEF";
	lcd_put(hex_chars[(byte >> 4) & 0x0F]);
	lcd_put(hex_chars[byte & 0x0F]);
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
	PORTD = (PORTD & 0X07) | first_nib;
	lcd_pulse_enable();
	
	// Clear bits for next nibble
	PORTD = (PORTD & 0X07) | sec_nib;
	lcd_pulse_enable();
}

void lcd_write_default(){
	row0_col = 0x80;
	row1_col = 0xC0;
	curr_addr = &row0_col;
	lcd_clr();
	lcd_puts2(display_mat[0]);
	lcd_cursor_pos(&LCD_ROW_1, NULL);
	lcd_puts2(display_mat[1]);
}


void lcd_init(void)
{
	// D series, used for LCD.
	// All pins OUTPUT, except pin 8 unused.
	DDRD = DDRD | 0x7F;
	// Need to put it into "Instruction Mode, DataWrite"
	PORTD = 0X00;
	// Data sheet Req, wait 15ms> on power up.
	avr_wait(16);
	// Init sequence for 4 bit mode
	send_lcd_data(0x30, true);
	// Wait time according to spec
	avr_wait(5);
	send_lcd_data(0x30, true);
	avr_wait(1);
	send_lcd_data(0x30, true);
	avr_wait(1);

	// Send 0x20 to switch to 4-bit mode
	send_lcd_data(0x2, true);

	send_lcd_data(0x28, true); // Function set: 4-bit, 2-line
	send_lcd_data(0x06, true); // Entry Mode Set: decrement, no shift (DO THIS LAST)
	send_lcd_data(0x0F, true); // Display ON (cursor on/off as you want)

	lcd_write_default();
}

void lcd_clr(void)
{
	send_lcd_data(0x01, true);
	avr_wait(2);
}

uint8_t check_cursor_pos(){
	//Invalid_cols to modify: 2 & 5
	if(((*curr_addr & 0x0F) == 2) || ((*curr_addr & 0x0F) == 5) || ((*curr_addr & 0x0F) > ((*curr_addr & 0xF0) == 0x80 ?  0x09 : 0x07))){
		return 0;
	} 
	return 1;
}

void lcd_cursor_pos(uint8_t* shift_r, uint8_t* shift_col)
{
	if(shift_r != NULL){
		curr_addr = (*shift_r == 0) ? &row0_col : &row1_col;
	}

	if(shift_col != NULL){
		if((*curr_addr & 0xF0) == 0x80){
			*curr_addr = (uint8_t)((int)*curr_addr + ((*shift_col == 0) ? -1 : 1));
			if(*curr_addr < 0x80) *curr_addr = 0x80;
			if(*curr_addr > 0x89) *curr_addr = 0x89;
		}else{
			*curr_addr = (uint8_t)((int)*curr_addr + ((*shift_col == 0) ? -1 : 1));
			if(*curr_addr < 0xC0) *curr_addr = 0xC0;
			if(*curr_addr > 0xC7) *curr_addr = 0xC7;
		}
	}
	send_lcd_data(*curr_addr, true);
}

void lcd_put(char c)
{
	send_lcd_data(c, false);
}

// Reads from prgm mem/ flash mem
void lcd_puts1(const char *s)
{
	char c;
	while ((c = pgm_read_byte(s++)) != 0) {
		send_lcd_data(c, false);
	}
}

// Reads from DDRAM
void lcd_puts2(const char *s)
{
	char c;
	while ((c = *(s++)) != 0) {
		send_lcd_data(c, false);
	}
}

uint8_t validate_input(uint8_t row, uint8_t col, char c){
	// Two sets of rules, for r0 and r1
	uint8_t i = c - '0';
	switch(row){
		case 0:
			if(col == 0){
				if((i == 0 || i == 1)){
					if(display_mat[0][1] != 'M' &&  i == 1 && ((display_mat[0][1] - '0') >= 3)){
						return 0;
					}
				}
			}
			if(col == 1){
				if(display_mat[0][0] == 'M') return 0;
				uint8_t num_concat = (display_mat[0][0] - '0') * 10 + i;			
				if(num_concat >= 13)return 0;
			}
			if(col == 3 || col == 4){
				uint8_t num_concat = (display_mat[0][0] - '0') * 10 + (display_mat[0][1] - '0');
				if(num_concat >= 1 && num_concat <= 12){
					// Months with 30 days
					if(num_concat == 4 || num_concat == 6 || num_concat == 9 || num_concat == 11){
						if((col == 3) && (i > 3)) return 0;
						if((col == 3) && (display_mat[0][4] >= 0 && display_mat[0][4] != 'D')) return 0;
						if((col == 4) && (display_mat[0][3] - '0') == 3 && i != 0) return 0;
					}
					// Special case Feb
					else if(num_concat == 2){
						if((col == 3) && (i > 2)) return 0;
					}
					// Months with 31 days
					else{
						if((col == 3) && (i > 3)) return 0;
						if((col == 3) &&(display_mat[0][4] >= 1 && display_mat[0][4] != 'D')) return 0;
						if(col == 4 && display_mat[0][4] - '0' == 3 && c > 1) return 0;
					}
				}else{
					 return 0;
				}
			}
			break;
		case 1:
			if(col == 0 && i > 1) return 0;
			if(col == 0 && (display_mat[1][1] != 'H') && (display_mat[1][1] - '0') > 2) return 0;
			if(col == 1){
				if((i > 2 && (display_mat[1][0] - '0' == 1)) || display_mat[1][0] == 'H') return 0;
			}
			if(col == 3 && i > 6) return 0;
			if(col == 4 ){
				if(((display_mat[1][3] - '0' == 6) && (i > 0)) || (display_mat[1][3] == 'M')) return 0;
			}
			break;
	}
	return 1;
}

// Used for updating, from main/user control.
void lcd_update(char c){
	// addr of row 1: 0xC0
	// addr of row 0: 0x80
	//char display_mat[2][50] = {"MM/DD/YYYY   OFF", "HH:MM:SSam    ST"};
	uint8_t curr_row = ((*curr_addr & 0xF0) ==  0x80) ? 0 : 1;
	uint8_t curr_col = *curr_addr & 0x0F;
	if(check_cursor_pos()){
		if(validate_input(curr_row, curr_col, c)){
			*curr_addr += 1;
			display_mat[curr_row][curr_col] = c;
			lcd_put(c);
		}
	}
}

uint8_t check_leap_year(){
	char i_year[5];
	for(uint8_t i = 6; i <= 9; ++i){
		i_year[i - 6] = display_mat[0][i];
	};
	uint8_t numer_year = (i_year[0] - '0') * 1000 + (i_year[1] - '0') * 100 + (i_year[2] - '0') * 10 + (i_year[3] - '0');

	if(numer_year % 4 != 0) return 0;
	else if (numer_year % 100 != 0) return 1;
	else if (numer_year % 400 != 0) return 0;
	else return 1;
}

void reset_lcd(){
	display_mat[0][13] = 'O';
	display_mat[0][14] = 'F';
	display_mat[0][15] = 'F';
	/*
	strcpy(display_mat[0], line1);
	strcpy(display_mat[1], line2am);
	*/
	lcd_write_default();
	send_lcd_data(0x0F, true);
	
	// Reset vars to default
	i_month = ((display_mat[0][0] - '0') * 10) + (display_mat[0][1] - '0');
	i_day = ((display_mat[0][3] - '0') * 10) + (display_mat[0][4] - '0');
	i_year_first = (display_mat[0][6] - '0') * 10 + (display_mat[0][7] - '0');
	i_year_second = (display_mat[0][8] - '0') * 10 + (display_mat[0][9] - '0');
	i_hour = ((display_mat[1][0] - '0') * 10) + (display_mat[1][1] - '0');
	i_min = ((display_mat[1][3] - '0') * 10) + (display_mat[1][4] - '0');
	i_sec = ((display_mat[1][6] - '0') * 10) + (display_mat[1][7] - '0');
	
}

void change_am_pm(){
	if (display_mat[1][8] == 'a'){
		display_mat[1][8] = 'p';
	}else{
		display_mat[1][8] = 'a';
	}
	lcd_write_default();
}

void update_lcd_vals(uint8_t num, uint8_t row, uint8_t col){
	uint8_t row_addr = (row == 0) ? (0x80 + col) : (0xC0 + col);
	send_lcd_data(row_addr, true);
	if(num >= 100){
		send_lcd_data((num / 100) + '0', false);
		num %= 100;
		send_lcd_data((num / 10) + '0', false); 
		send_lcd_data((num % 10) + '0', false);
	}else if (num >= 10){
		send_lcd_data((num / 10) + '0', false);
		send_lcd_data((num % 10) + '0', false);
	} else {
		send_lcd_data('0', false);
		send_lcd_data(num + '0', false);
	}
}

uint8_t check_valid_set(){
	uint8_t max_col = 10;
	for(uint8_t i = 0; i < 2; ++i){
		if(i == 1) max_col -= 2;
		for(uint8_t j = 0; j < max_col; ++j){
			if(j == 2 || j == 5) ++j;
			if(!((display_mat[i][j] >= 0x30) && (display_mat[i][j] <= 0x39))){
				return 0;
			}
		}
	}
	return 1;
}

uint8_t set_lcd(){
	if(check_valid_set()){
		display_mat[0][13] = ' ';
		display_mat[0][14] = 'O';
		display_mat[0][15] = 'N';
		check_leap_year();
		lcd_write_default();
		send_lcd_data(0x0C, true);
		// Set global vars, on this set call.
		// So that "increment_cal" can be called/ externally controlled by another clock.
	
		i_month = ((display_mat[0][0] - '0') * 10) + (display_mat[0][1] - '0');
		i_day = ((display_mat[0][3] - '0') * 10) + (display_mat[0][4] - '0');
		i_year_first = (display_mat[0][6] - '0') * 10 + (display_mat[0][7] - '0');
		i_year_second = (display_mat[0][8] - '0') * 10 + (display_mat[0][9] - '0');
		i_hour = ((display_mat[1][0] - '0') * 10) + (display_mat[1][1] - '0');
		i_min = ((display_mat[1][3] - '0') * 10) + (display_mat[1][4] - '0');
		i_sec = ((display_mat[1][6] - '0') * 10) + (display_mat[1][7] - '0');
		is_leap_year = check_leap_year();
		past_twelve = 0;
		return 1;
	}
	return 0;
}

void date_inc(){
	uint8_t max_day;
	if(i_month == 4 || i_month == 6 || i_month == 9 || i_month == 11){
		max_day = 30;
	}
	else if(i_month == 2){
		max_day = (is_leap_year ? 29 : 28);
	}
	else{
		max_day = 31;
	}
	if(i_day > max_day){
		++i_month;
		i_day = 0x01;
		update_lcd_vals(i_month, 0, 0);
		update_lcd_vals(i_day, 0, 3);
	}
	if(i_month > 12){
		++i_year_second;
		i_month = 0x01;
		if(i_year_second > 99){
			++i_year_first;
			i_year_second = 0x00;
		}
		update_lcd_vals(i_month, 0, 0);
		update_lcd_vals(i_year_first, 0, 6);
		update_lcd_vals(i_year_second, 0, 8);
	}
}

void increment_cal(){
		if(i_sec >= 60){
			++i_min;
			update_lcd_vals(i_min, 1, 3);
			i_sec = 0x00;
		}
		if(i_min >= 60){
			++i_hour;
			i_min = 0x00;
			update_lcd_vals(i_hour, 1, 0);
			update_lcd_vals(i_min, 1, 3);
		}
		if(i_hour >= 12 && !past_twelve){
			if(display_mat[1][8] == 'a'){
				change_am_pm();
				}else if(display_mat[1][8] == 'p'){
				++i_day;
				change_am_pm();
				update_lcd_vals(i_day, 0, 3);
				date_inc();
			}
			i_min = 0x00;
			i_sec = 0x00;
			update_lcd_vals(i_sec, 1, 6);
			update_lcd_vals(i_min, 1, 3);
			update_lcd_vals(i_hour, 1, 0);
			past_twelve = 1;
		}
		if(i_hour >= 13){
			i_hour = 0x01;
			update_lcd_vals(i_hour, 1, 0);
			past_twelve = 0;
		}
		avr_wait(1000);
		++i_sec;
		update_lcd_vals(i_sec, 1, 6);
}
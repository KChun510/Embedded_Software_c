/*
 * lcd.c
 *
 * Created: 4/21/2025 11:46:18 AM
 *  Author: westcoast
 */ 
#include "avr.h"
#include "lcd.h"

const char line1[] PROGMEM = "MM/DD/YYYY   OFF";
const char line2am[] PROGMEM = "HH:MM:SSam    ST";
const char line2pm[] PROGMEM = "HH:MM:SSpm    ST";
const char line2mil[] PROGMEM = "HH:MM:SS    24hr";

static uint8_t row0_col = 0x80;
static uint8_t row1_col = 0xC0;
uint8_t LCD_ROW_1 = 1;
uint8_t write_one = 1;
static uint8_t *curr_addr = &row0_col;

static inline void
sleep_700ns(void)
{
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
}

void 
lcd_pulse_enable(){
	// Enable Pin, set on pin 2
	SET_BIT(PORTD, 2);
	avr_wait(1);
	CLR_BIT(PORTD, 2);
	avr_wait(1);
}

void 
send_lcd_data(unsigned char data, bool instruc){
	if(instruc){
		CLR_BIT(PORTD, 0);
		}else{
		SET_BIT(PORTD, 0);
	}
	sleep_700ns();
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



void
lcd_write_default(){
	row0_col = 0x80;
	row1_col = 0xC0;
	curr_addr = &row0_col;
	lcd_clr();
	lcd_puts1(line1);
	lcd_pos(&LCD_ROW_1, NULL);
	lcd_puts1(line2am);
}


void
lcd_init(void)
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

void
lcd_clr(void)
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

void
lcd_pos(uint8_t* shift_r, uint8_t* shift_col)
{
	// addr of row 1: 0xC0
	// addr of row 0: 0x80
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

void
lcd_put(char c)
{
	if(check_cursor_pos()){
		*curr_addr += 1;
		send_lcd_data(c, false);
	}
}


// Reads from prgm mem/ flash mem
void
lcd_puts1(const char *s)
{
	char c;
	while ((c = pgm_read_byte(s++)) != 0) {
		send_lcd_data(c, false);
	}
}

// Reads from DDRAM
void
lcd_puts2(const char *s)
{
	char c;
	while ((c = *(s++)) != 0) {
		send_lcd_data(c, false);
	}
}

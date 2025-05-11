/*
 * lcd.h
 *
 * Created: 4/21/2025 11:45:17 AM
 *  Author: westcoast
 */ 
#ifndef _LCD_H
#define _LCD_H
#include <stdbool.h>

void get_lcd_data(void);

void send_lcd_data(unsigned char data, bool instruc);

void lcd_init(const char first_song[]);

void lcd_clr(void);

void lcd_cursor_pos(uint8_t* shift_r, uint8_t* shift_col);

void lcd_write_default(const char song_name[]);

void lcd_put(char c);

void lcd_puts1(const char *s);

void lcd_puts2(const char *s);

void lcd_update(char c);

uint8_t set_lcd();

void reset_lcd();

void change_am_pm();

void increment_cal();

#endif /* _LCD_H */
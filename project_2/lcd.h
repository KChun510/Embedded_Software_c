/*
 * lcd.h
 *
 * Created: 4/21/2025 11:45:17 AM
 *  Author: westcoast
 */ 
#ifndef _LCD_H
#define _LCD_H
#include <stdbool.h>

void send_lcd_data(unsigned char data, bool instruc);

void lcd_init(void);

void lcd_clr(void);

void lcd_pos(uint8_t* shift_r, uint8_t* shift_col);

void lcd_write_default();

void lcd_put(char c);

void lcd_puts1(const char *s);

void lcd_puts2(const char *s);

#endif /* _LCD_H */
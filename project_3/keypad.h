/*
 * keypad.h
 *
 * Created: 4/21/2025 1:16:37 PM
 *  Author: westcoast
 */ 

#ifndef _KEYPAD_H
#define _KEYPAD_H
#include "avr.h"

int get_key();
int is_pressed(int r, int c);
void keypad_init();

#endif

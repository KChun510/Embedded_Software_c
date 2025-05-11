/*
 * speaker.c
 *
 * Created: 5/10/2025 3:40:43 PM
 *  Author: westcoast
 */ 
#include "speaker.h"

const uint16_t period_us[12] = {
	4545, 4290, 4048, 3822, 3609, 3405, 
	3215, 3038, 2865, 2703, 2551, 2497
};

const uint16_t duration_ms[4] = {
	2000, 1000, 500, 250
};

NoteTiming get_note_timing(const PlayingNote* note_data){
	NoteTiming result;
	uint16_t period = period_us[note_data->note];
	result.TH = period >> 1;
	result.K = (1000UL * duration_ms[note_data->duration] ) / period;
	return result;
};
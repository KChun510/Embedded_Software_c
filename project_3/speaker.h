/*
 * speaker.h
 *
 * Created: 5/10/2025 3:40:56 PM
 *  Author: westcoast
 */ 
#ifndef _SPEAKER_H
#define _SPEAKER_H
#include "avr.h"

typedef enum {
	A, As, B, C, Cs, D, Ds, Ee, F, Fs, G, Gs
} Note;

typedef enum {
	W, H, Q, E
} Duration;

// Using this type def
// PlayingNote myNote = { A, W };
// Pass by pointer play_note(&myNote);
typedef struct {
	Note note;
	Duration duration;
} PlayingNote;

typedef struct {
	uint16_t TH;
	uint16_t K;
} NoteTiming;

NoteTiming get_note_timing(const PlayingNote* note_data);

#endif

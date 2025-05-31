/*
 * music.h
 *
 * Created: 5/11/2025 11:08:08 AM
 *  Author: westcoast
 */ 
#ifndef _MUSIC_H
#define _MUSIC_H
#include "speaker.h"
typedef struct {
	char song_name[16];
	const PlayingNote* song;
	size_t length;
} Track;
extern uint8_t song_count;
extern const PlayingNote basic_drum[];
extern const PlayingNote doom_beat[];
extern const PlayingNote trap_kick[];
extern const PlayingNote kick_snare[];
extern const PlayingNote techno[];
extern Track music_track[]; 
#endif


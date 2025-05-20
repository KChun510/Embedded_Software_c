/*
 * music.c
 *
 * Created: 5/11/2025 11:07:52 AM
 *  Author: westcoast
 */ 
#include "music.h"
uint8_t song_count = 5;
const PlayingNote basic_drum[] = {
	{A, Q}, {A, Q}, {Ee, Q}, {A, Q},
	{C, Q}, {Ee, Q}, {G, Q}, {Ee, Q}
};
const PlayingNote doom_beat[] = {
	{Ee, Q}, {Ee, Q}, {G, E}, {Ee, E},
	{D, Q}, {C, Q}, {Ee, H}
};
const PlayingNote trap_kick[] = {
	{C, Q}, {C, E}, {C, E}, {D, Q}, {G, H},
	{C, Q}, {D, Q}, {Ee, Q}, {D, Q}
};
const PlayingNote kick_snare[] = {
	{C, H}, {G, Q}, {C, H}, {G, Q},
	{D, Q}, {Ee, Q}, {D, H}
};
const PlayingNote techno[] = {
	{A, E}, {A, E}, {C, E}, {D, E},
	{A, E}, {C, E}, {D, E}, {Ee, E},
	{F, Q}, {G, Q}
};
Track music_track[] = {{"   basic_drum   ", basic_drum, 8} , { "   dooms_beat   ", doom_beat, 7} , {"   trap_kick   ", trap_kick, 9} , {"   kick_snare   ", kick_snare, 7}, {"     techno     ",techno, 10}};



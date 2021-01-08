#include "config.hpp"
#include <stdio.h>

#define NONE 0
#define SINE 1
#define SAW  2
#define SQR  3
#define TRI  4

char* osc_types[] = { "OFF", "SINE", "SAW", "SQR", "TRI" };

void display_config()
{
	printf(
	"     ___  _______  __   __  __    _  _______  __   __\n"
	"    |   ||       ||  | |  ||  |  | ||       ||  | |  |\n"
	"    |   ||  _____||  |_|  ||   |_| ||_     _||  |_|  |\n"
	"    |   || |_____ |       ||       |  |   |  |       |\n"
	" ___|   ||_____  ||_     _||  _    |  |   |  |       |\n"
	"|       | _____| |  |   |  | | |   |  |   |  |   _   |\n"
	"|_______||_______|  |___|  |_|  |__|  |___|  |__| |__|\n");
	printf("num_oscillators: %d | num_voices %d\n", NUM_OSCILLATORS, MAX_VOICES);
	for (int i = 0; i < NUM_OSCILLATORS; i++) {
		printf("oscillator %d: %s\n", i + 1, osc_types[i]);
	} 
	printf("envelope: %f", ENV_ATTACK);
}
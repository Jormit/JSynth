// Entire Synth structure defined here.

#ifndef CONFIGS
#define CONFIGS

// Oscillator types
#define NONE 0
#define SINE 1
#define SAW  2
#define SQR  3
#define TRI  4

// Oscillator settings
#define NUM_OSCILLATORS 2
#define MAX_VOICES 4
const int OSC_TYPES[NUM_OSCILLATORS] = { SAW, SAW };
const float OSC_TUNE[NUM_OSCILLATORS] = { 1.0f, 1.02f };

// Amplitude envelope
#define ENV_ATTACK 0.1
#define ENV_DECAY 1_s
#define ENV_SUSTAIN_LVL -12_dB
#define ENV_SUSTAIN_RATE 5_s
#define ENV_RELEASE_RATE 1_s
#define ENVELOPE_PARAM {ENV_ATTACK, ENV_DECAY, ENV_SUSTAIN_LVL, ENV_SUSTAIN_RATE, ENV_RELEASE_RATE}

// MIDI settings
#define MIDI_DEVICE 1
#define MIDI_DEBUG 1
#endif


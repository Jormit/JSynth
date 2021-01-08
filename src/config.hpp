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

// Filter settings
#define FILTER_CUTOFF 3000
#define FILTER_Q 0.707
#define FILTER_ENV_MOD 0

// Envelope 1
#define ENV1_ATTACK 0.1
#define ENV1_DECAY 1_s
#define ENV1_SUSTAIN_LVL -12_dB
#define ENV1_SUSTAIN_RATE 5_s
#define ENV1_RELEASE_RATE 1_s
#define ENV1_PARAM {ENV1_ATTACK, ENV1_DECAY, ENV1_SUSTAIN_LVL, ENV1_SUSTAIN_RATE, ENV1_RELEASE_RATE}

// Envelope 2
#define ENV2_ATTACK 0.1
#define ENV2_DECAY 1_s
#define ENV2_SUSTAIN_LVL -12_dB
#define ENV2_SUSTAIN_RATE 5_s
#define ENV2_RELEASE_RATE 1_s
#define ENV2_PARAM {ENV1_ATTACK, ENV1_DECAY, ENV1_SUSTAIN_LVL, ENV1_SUSTAIN_RATE, ENV1_RELEASE_RATE}

// MIDI settings
#define MIDI_DEVICE 1
#define MIDI_DEBUG 0
#endif


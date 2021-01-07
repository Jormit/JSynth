#ifndef CONFIGS
#define CONFIGS

#define NUM_OSCILLATORS 2
#define MAX_VOICES 4
#define NONE 0
#define SINE 1
#define SAW  2
#define SQR  3
#define TRI  4

#define ENV_ATTACK 100_ms
#define ENV_DECAY 1_s
#define ENV_SUSTAIN_LVL -12_dB
#define ENV_SUSTAIN_RATE 5_s
#define ENV_RELEASE_RATE 1_s
#define ENVELOPE_PARAM {ENV_ATTACK, ENV_DECAY, ENV_SUSTAIN_LVL, ENV_SUSTAIN_RATE, ENV_RELEASE_RATE}

#define MIDI_DEVICE 1
#define MIDI_DEBUG 1
#endif


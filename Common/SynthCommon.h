//#######################################################################
// Module:     SynthCommon.h
// Descrption: Common terms for all modules
// Creator:    markeby
// Date:       11/11/2023
//#######################################################################
#pragma once

// D/A converter maximum value
#define DA_MAX          4095

// number of synth oscillaotor/filter/control channels
#define VOICE_COUNT         8

// number of mixer channels in each oscillator
#define OSC_MIXER_COUNT     5

// number of total envelopes to process
#define ENVELOPE_COUNT      OSC_MIXER_COUNT

// number of MIDI channels possible
#define MAX_MIDI            16

// number of noise sources
#define SOURCE_CNT_NOISE    4

// number of LFO sources
#define SOURCE_CNT_LFO      3

// maximum number of entries on mapping page
#define MAP_COUNT           (VOICE_COUNT / 2)
#define GROUP_COUNT         3

// Number of devices per filter
#define FILTER_DEVICES      2

// Number of filter control inputs available
#define FILTER_CONTROLS     4

// Number of filter outputs
#define FILTER_OUTPUTS      4

// Number of modululation multiplex inputs
#define NUM_MOD_MUX_IN      3

// Number of configurations that can be stored
#define MAX_LOAD_SAVE       20

// Detent multiplier for midi at 10mSec intervals
#define TIME_MULT           10

// Maximum value from MIDI
#define MAX_MVAL            127

#define MIDI_MULTIPLIER     32.245
#define MIDI_INV_MULTIPLIER 0.03101256

// Scaler for 100% on midi interface
#define PERS_SCALER         0.7874
#define PRS_SCALER          0.007874
#define PRS_UNSCALER        127.1



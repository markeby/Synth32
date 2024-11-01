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
#define CHAN_COUNT      8

// number of mixer channels in each oscillator
#define OSC_MIXER_COUNT 5

// number of analog channels for noise VCF and VCA
#define FILTER_ANALOG_COUNT 2

// number of total envelopes to process
#define ENVELOPE_COUNT      OSC_MIXER_COUNT
//+ FILTER_ANALOG_COUNT

// Detent multiplier for midi at 10mSec intervals
#define TIME_MULT           10

// Maximum value from MIDI
#define MAX_MVAL            127

#define MIDI_MULTIPLIER     32.245
#define MIDI_INV_MULTIPLIER 0.03101256

// Scaler for 100% on midi interface
#define PERS_SCALER     0.7874
#define PRS_SCALER      0.007874
#define PRS_UNSCALER    127.1

// Scaler for plus/minus pitch bend
#define BEND_SCALER     0.00305


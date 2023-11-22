//#######################################################################
// Module:     SynthCommon.h
// Descrption: Common terms for all modules
// Creator:    markeby
// Date:       11/11/2023
//#######################################################################
#pragma once

// number of synth oscillaotor/filter/control channels
#define CHAN_COUNT      6

// number of mixer channels in each oscillator
#define OSC_MIXER_COUNT 5

// Detent multiplier for midi
#define TIME_MULT       10

// Scaler for 100% on midi interface
#define PERS_SCALER     0.7874
#define PRS_SCALER      0.007874
#define PRS_UNSCALER    127.1

// Scaler for plus/minus pitch bend
#define BEND_SCALER     0.00305

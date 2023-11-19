// config.h
//
// Project settings here (defines, numbers, etc.)

#pragma once

#include <Streaming.h>
#include "SynthCommon.h"

//#####################################
// Usefull constants
//#####################################
#define MICRO_TO_MILLI(x) ((x) * 0.001)
#define MILLI_TO_MICRO(x) ((x) * 1000)

//#####################################
// In case I get confused with Python
//#####################################
#define False false
#define True  true

//#####################################
//   debug controls
//#####################################
extern bool DebugInterface;
extern bool DebugGraphics;

//#################################################
//   Alarms and alerts
//#################################################
#define HEARTBEAT_PIN      17
#define RED_PIN             4
#define GREEN_PIN          16

//#################################################
//    Serial 1 MIDI echo in port
//#################################################
#define RXD1    35
#define TXD1    22

//#################################################
//   Global system variables
//#################################################
extern bool     SystemError;
extern float    DeltaTime;
extern uint64_t RunTime;
extern float    AverageDeltaTime;

//#################################################
//    Synth specific constants
//#################################################
#define MIDI_PORT           0                           // sometime referred to as cable number
#define OSC_MIXER_COUNT     5
#define FULL_KEYS           128
#define DA_RANGE            4096
#define NOTES_PER_OCTAVE    12

#define SM_FADER            16
#define SM_CONTROL          16
#define SM_SWITCH           24
#define SM_MOD              2

#define MAXDA       4095

//#################################################
//    Midi control mapping
//#################################################
typedef struct
    {
    uint8_t     Channel;
    const char* desc;
    void       (*CallBack)(uint8_t chan, float control);
    float       Scaler;
    }  MIDI_VALUE_MAP;

typedef struct
    {
    uint8_t     Channel;
    const char* desc;
    void       (*CallBack)(uint8_t ch, uint8_t state);
    } MIDI_SWITCH_MAP;


//#################################################
//  Synth interfaces
//#################################################
extern MIDI_VALUE_MAP       FaderMapArray[];
extern MIDI_VALUE_MAP       KnobMapArray[];
extern MIDI_VALUE_MAP       PitchMapArray[];
extern MIDI_SWITCH_MAP      SwitchMapArray[];


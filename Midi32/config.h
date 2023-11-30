// config.h
//
// Project settings here (defines, numbers, etc.)

#pragma once

#include <Streaming.h>

#include "I2Cdevices.h"

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
extern bool DebugMidi;
extern bool DebugDtoA;
extern bool DebugOsc;
extern bool DebugSynth;
extern bool AnalogDiagEnabled;
extern int  AnalogDiagDevice;

//#################################################
//   Alarms and alerts
//#################################################
#define HEARTBEAT_PIN       2
#define BEEP_PIN            15

//#################################################
//    Serial 1 MIDI echo out port
//#################################################
#define RXD1    35
#define TXD1    32

//#################################################
//   Global system variables
//#################################################
extern bool     SynthActive;

extern bool     SystemError;
extern bool     SystemFail;
extern float    DeltaTime;
extern uint64_t RunTime;
extern float    AverageDeltaTime;

//#################################################
//    Synth specific constants
//#################################################
#define MIDI_PORT           0        // sometime referred to as cable number
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
    byte        Channel;
    const char* desc;
    void       (*CallBack)(byte chan, byte data);
    }  MIDI_VALUE_MAP;

typedef struct
    {
    byte        Channel;
    const char* desc;
    void       (*CallBack)(byte ch, byte state);
    } MIDI_SWITCH_MAP;


//#################################################
//  Synth interfaces
//#################################################
extern MIDI_VALUE_MAP       FaderMapArray[];
extern MIDI_VALUE_MAP       KnobMapArray[];
extern MIDI_VALUE_MAP       PitchMapArray[];
extern MIDI_SWITCH_MAP      SwitchMapArray[];

//#################################################
//  I2C bus interfaces
//#################################################
extern  I2C_INTERFACE_C I2cDevices;


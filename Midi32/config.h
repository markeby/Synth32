// config.h
//
// Project settings here (defines, numbers, etc.)

#pragma once

#include <Streaming.h>

#include "I2Cdevices.h"

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
//    DIN MIDI-2     Pinout
//#################################################
#define RXD2                16      // U2RRXD
#define TXD2                13

//#################################################
//   Global system variables
//#################################################
extern bool          SynthActive;

extern bool          SystemError;
extern bool          SystemFail;
extern int           DeltaTime;
extern unsigned long RunTime;
extern bool          UsbOnline;

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

//#################################################
//  I2C bus interfaces
//#################################################
extern  I2C_INTERFACE_C I2cDevices;


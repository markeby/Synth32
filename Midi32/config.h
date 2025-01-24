// config.h
//
// Project settings here (defines, numbers, etc.)

#pragma once

#include <Streaming.h>

#include "../Common/SynthCommon.h"
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
extern bool DebugI2C;
extern bool DebugOsc;
extern bool DebugSynth;
extern bool DebugDisp;
extern bool AnalogDiagEnabled;

//#################################################
//   Alarms and alerts
//#################################################
#define HEARTBEAT_PIN       2
#define BEEP_PIN            15

//#################################################
//    Serial 1 MIDI echo out port
//#################################################
#define RXD1        35
#define TXD1        32

//#################################################
//    Serial 2 MIDI echo out port
//#################################################
#define RXD2        16
#define TXD2        17

//#################################################
//    Display message parameters and I2C
//#################################################
#define MSG_ADRS                0x51
#define MSG_SCL                 25
#define MSG_SDA                 26
#define RESET_STROBE_IO         27
#define DISPLAY_SETTLE_TIME     200000

//#################################################
//  Synth I2C interface starting indexes
//#################################################
#define START_OSC_ANALOG        0
#define START_MULT_DIGITAL      88
#define START_NOISE_DIGITAL     104
#define START_LFO_DIGITAL       120
#define D_A_COUNT               88

//#################################################
//   Global system variables
//#################################################
extern bool     SynthActive;

extern bool     SystemError;
extern bool     SystemFail;
extern float    DeltaTimeMilli;
extern float    DeltaTimeMicro;
extern float    LongestTimeMilli;
extern float    DeltaTimeMilliAvg;
extern uint64_t RunTime;

//#################################################
//    Synth specific constants
//#################################################
#define MIDI_PORT           0        // sometime referred to as cable number
#define FULL_KEYS           128
#define DA_RANGE            4096
#define MAX_DA              (DA_RANGE - 1)
#define NOTES_PER_OCTAVE    12

//#################################################
//    Midi control mapping
//#################################################
typedef struct
    {
    short       Index;
    const char* Desc;
    void       (*CallBack)(short chan, short data);
    }  MIDI_MAP;

typedef struct
    {
    short       Index;
    bool        State;
    const char* Desc;
    void       (*CallBack)(short chan, bool state);
    }  MIDI_BUTTON_MAP;

typedef struct
    {
    short       Index;
    byte        Color;
    const char* Desc;
    void       (*CallBack)(short chan, short data);
    }  MIDI_XL_MAP;

typedef struct
    {
    short       Index;
    const char* Desc;
    void        (*CallBack)(short chan, short data);
    int         Value;
    }  MIDI_ENCODER_MAP;

typedef byte  LED_NOTE_MAP;

//#################################################
//  Synth interfaces
//#################################################
extern MIDI_MAP             FaderMapArray[];
extern MIDI_ENCODER_MAP     KnobMapArray[];
extern MIDI_BUTTON_MAP      SwitchMapArray[];
#define SIZE_CL_MAP         56
#define SIZE_S_LED          24
extern MIDI_XL_MAP          XlMapArray[SIZE_CL_MAP];
extern LED_NOTE_MAP         SendA[];
extern LED_NOTE_MAP         SendB[];
extern LED_NOTE_MAP         PanDevice[];

//#################################################
//  I2C bus interfaces
//#################################################
extern  I2C_INTERFACE_C I2cDevices;



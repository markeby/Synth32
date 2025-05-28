//#######################################################################
// Module:     Config.h
// Descrption: Configuration data for synth
// Creator:    markeby
// Date:       2/16/2025
//#######################################################################
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
#define PITCH_BEND_CENTER   2048

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

//#################################################
//    Synthesizer configuration class
//#################################################
class SYNTH_VOICE_CONFIG_C
    {
private:
    // Parameters for one pair of MIDI Channels of each group

    typedef struct
        {
        float   MaxLevel;
        float   SustainLevel;
        float   AttackTime;
        float   DecayTime;
        float   ReleaseTime;
        }  ENVELOPE_T;
    struct
        {
        short       MapVoiceMidi;
        short       OutputEnable;
        short       MapVoiceNoise;
        bool        RampDirection;
        float       PulseWidth;
        ENVELOPE_T  Env[OSC_MIXER_COUNT];
        }   Cs;

    String              Name;

public:
    bool    SelectedEnvelope[OSC_MIXER_COUNT];

         SYNTH_VOICE_CONFIG_C (void);
    void Save                 (const char* name);
    void Load                 (const char* name);

    inline void   SetName            (String& name)             { this->Name = name;}
    inline String Getname            (void)                     { return (this->Name); }
    inline void   SetOutputEnable    (short data)               { this->Cs.OutputEnable = data; }
    inline short  GetOutputEnable    (void)                     { return (this->Cs.OutputEnable); }
    inline void   SetVoiceMidi       (short data)               { this->Cs.MapVoiceMidi = data; }
    inline short  GetVoiceMidi       (void)                     { return (this->Cs.MapVoiceMidi); }
    inline void   SetVoiceNoise      (short data)               { this->Cs.MapVoiceNoise = data; }
    inline short  GetVoiceNoise      (void)                     { return (this->Cs.MapVoiceNoise); }
    inline void   SetRampDirection   (bool data)                { this->Cs.RampDirection = data; }
    inline bool   GetRampDirection   (void)                     { return (this->Cs.RampDirection); }
    inline void   SetPulseWidth      (float data)               { this->Cs.PulseWidth = data; }
    inline float  GetPulseWidth      (void)                     { return (this->Cs.PulseWidth); }
    inline void   SetMaxLevel        (byte index, float data)   { this->Cs.Env[index].MaxLevel = data; }
    inline float  GetMaxLevel        (byte index)               { return (this->Cs.Env[index].MaxLevel); }
    inline void   SetSustainLevel    (byte index, float data)   { this->Cs.Env[index].SustainLevel = data; }
    inline float  GetSustainLevel    (byte index)               { return (this->Cs.Env[index].SustainLevel); }
    inline void   SetAttackTime      (byte index, float data)   { this->Cs.Env[index].AttackTime = data; }
    inline float  GetAttackTime      (byte index)               { return (this->Cs.Env[index].AttackTime); }
    inline void   SetDecayTime       (byte index, float data)   { this->Cs.Env[index].DecayTime = data; }
    inline float  GetDecayTime       (byte index)               { return (this->Cs.Env[index].DecayTime); }
    inline void   SetReleaseTime     (byte index, float data)   { this->Cs.Env[index].ReleaseTime = data; }
    inline float  GetReleaseTime     (byte index)               { return (this->Cs.Env[index].ReleaseTime); }
    };

class SYNTH_CONFIG_C
    {
private:
    struct
        {
        short   SoftFrequency;
        short   LfoMidi[4];
        bool    SoftMixerLFO[OSC_MIXER_COUNT];
        struct
            {
            short   Frequency;
            short   PulseWidth;
            bool    RampDir;
            bool    Select[SOURCE_CNT_LFO];
            } CfgLFO[2];
        } Cs;

    String          Name;

public:
    SYNTH_VOICE_CONFIG_C  Voice[VOICE_COUNT];

         SYNTH_CONFIG_C (void);
    void Save           (short num);
    void Load           (short num);

    inline void   SetName            (String& name)                         { this->Name = name; }
    inline String Getname            (void)                                 { return (this->Name); }
    inline void   SetSoftFreq        (short data)                           { this->Cs.SoftFrequency = data; }
    inline short  GetSoftFreq        (void)                                 { return (this->Cs.SoftFrequency); };
    inline void   SetModMidi         (byte index, short data)               { this->Cs.LfoMidi[index] = data; }
    inline short  GetModMidi         (byte index)                           { return (this->Cs.LfoMidi[index]); };
    inline void   SetModSoftMixer    (short index, bool data)               { this->Cs.SoftMixerLFO[index] = data; }
    inline bool   GetModSoftMixer    (short unit)                           { return (this->Cs.SoftMixerLFO[unit]); }
    inline void   SetSelect          (short unit, short index, bool data)   { this->Cs.CfgLFO[unit].Select[index] = data; }
    inline bool   GetSelect          (short unit, short index)              { return (this->Cs.CfgLFO[unit].Select[index]); }
    inline void   SetRampDir         (short unit, bool data)                { this->Cs.CfgLFO[unit].RampDir = data; }
    inline bool   GetRampDir         (short unit)                           { return (this->Cs.CfgLFO[unit].RampDir); }
    inline void   SetFrequency       (short unit, short data)               { this->Cs.CfgLFO[unit].Frequency = data; }
    inline short  GetFrequency       (short unit)                           { return (this->Cs.CfgLFO[unit].Frequency); }
    inline void   SetPulseWidth      (short unit, short data)               { this->Cs.CfgLFO[unit].PulseWidth = data; }
    inline short  GetPulseWidth      (short unit)                           { return (this->Cs.CfgLFO[unit].PulseWidth); }
    };


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
#include "Debug.h"

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
    }  G49_FADER_MIDI_MAP;

typedef struct
    {
    short       Index;
    bool        State;
    const char* Desc;
    void       (*CallBack)(short chan, bool state);
    }  G49_BUTTON_MIDI_MAP;

typedef struct
    {
    short       Index;      // Value passed into function
    byte        Color;      // Default color code for Novation device
    const char* Desc;
    void       (*CallBack)(short chan, short data);
    }  XL_MIDI_MAP;

typedef struct
    {
    short       Index;
    const char* Desc;
    void        (*CallBack)(short chan, short data);
    int         Value;
    }  G49_ENCODER_MIDI_MAP;

typedef byte  LED_NOTE_MAP;

//#################################################
// Novation LaunchControl X1 Control mapping
//#################################################
#define XL_MIDI_MAP_SIZE        56
#define XL_MIDI_MAP_PAGES       6

#define XL_MIDI_MAP_OSC         0
#define XL_MIDI_MAP_FLT         1
#define XL_MIDI_MAP_LFO         2
#define XL_MIDI_MAP_MAPPING     3
#define XL_MIDI_MAP_LOADSAVE    4
#define XL_MIDI_MAP_SPARE       5

#define XL_BUTTON_START         24
#define XL_BUTTON_END           40
#define XL_BUTTON_COUNT         (XL_BUTTON_END - XL_BUTTON_START)

//#################################################
//  Synth interfaces
//#################################################
extern  G49_FADER_MIDI_MAP      FaderMidiMapArray[];
extern  G49_ENCODER_MIDI_MAP    KnobMidiMapArray[];
extern  G49_BUTTON_MIDI_MAP     SwitchMidiMapArray[];
extern  XL_MIDI_MAP             XL_MidiMapArray[XL_MIDI_MAP_PAGES][XL_MIDI_MAP_SIZE];

//#################################################
//  I2C bus interfaces
//#################################################
extern  I2C_INTERFACE_C I2cDevices;

//#################################################
//    Synthesizer configuration storage
//#################################################
class SYNTH_VOICE_CONFIG_C
    {
private:
    // Parameters for one pair of MIDI Channels of each group

    typedef struct
        {
        float   MaxLevel;
        float   MinLevel;
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
        float       PulseWidth;
        bool        RampDirection;
        byte        OutputMask;
        byte        FilterCtrl[FILTER_DEVICES];
        ENVELOPE_T  OscEnv[OSC_MIXER_COUNT];
        ENVELOPE_T  FltEnv[FILTER_DEVICES];
        }   Cs;

    String  Name;
    byte    ButtonStateOsc[XL_BUTTON_COUNT];
    byte    ButtonStateFlt[XL_BUTTON_COUNT];

public:
    bool    SelectedOscEnvelope[OSC_MIXER_COUNT];
    bool    SelectedFltEnvelope[FILTER_DEVICES];

         SYNTH_VOICE_CONFIG_C           (void);
    void Save                           (const char* name);
    void Load                           (const char* name);
    void InitButtonsXL                  (void);

    inline byte*  GetButtonStateOsc     (void)                      { return (this->ButtonStateOsc); }
    inline byte*  GetButtonStateFlt     (void)                      { return (this->ButtonStateFlt); }
    inline void   SetName               (String& name)              { this->Name = name;}
    inline String Getname               (void)                      { return (this->Name); }
    inline void   SetVoiceMidi          (short data)                { this->Cs.MapVoiceMidi = data; }
    inline short  GetVoiceMidi          (void)                      { return (this->Cs.MapVoiceMidi); }
    inline void   SetVoiceNoise         (short data)                { this->Cs.MapVoiceNoise = data; }
    inline short  GetVoiceNoise         (void)                      { return (this->Cs.MapVoiceNoise); }
    inline void   SetRampDirection      (bool data)                 { this->Cs.RampDirection = data; }
    inline bool   GetRampDirection      (void)                      { return (this->Cs.RampDirection); }
    inline void   SetPulseWidth         (float data)                { this->Cs.PulseWidth = data; }
    inline float  GetPulseWidth         (void)                      { return (this->Cs.PulseWidth); }
    inline void   SetOscMaxLevel        (byte index, float data)    { this->Cs.OscEnv[index].MaxLevel = data; }
    inline float  GetOscMaxLevel        (byte index)                { return (this->Cs.OscEnv[index].MaxLevel); }
    inline void   SetOscSustainLevel    (byte index, float data)    { this->Cs.OscEnv[index].SustainLevel = data; }
    inline float  GetOscSustainLevel    (byte index)                { return (this->Cs.OscEnv[index].SustainLevel); }
    inline void   SetOscAttackTime      (byte index, float data)    { this->Cs.OscEnv[index].AttackTime = data; }
    inline float  GetOscAttackTime      (byte index)                { return (this->Cs.OscEnv[index].AttackTime); }
    inline void   SetOscDecayTime       (byte index, float data)    { this->Cs.OscEnv[index].DecayTime = data; }
    inline float  GetOscDecayTime       (byte index)                { return (this->Cs.OscEnv[index].DecayTime); }
    inline void   SetOscReleaseTime     (byte index, float data)    { this->Cs.OscEnv[index].ReleaseTime = data; }
    inline float  GetOscReleaseTime     (byte index)                { return (this->Cs.OscEnv[index].ReleaseTime); }

    inline void   SetFltStart           (byte index, float data)    { this->Cs.FltEnv[index].MinLevel = data; }
    inline float  GetFltStart           (byte index)                { return (this->Cs.FltEnv[index].MinLevel); }
    inline void   SetFltEnd             (byte index, float data)    { this->Cs.FltEnv[index].MaxLevel = data; }
    inline float  GetFltEnd             (byte index)                { return (this->Cs.FltEnv[index].MaxLevel); }
    inline void   SetFltSustainLevel    (byte index, float data)    { this->Cs.FltEnv[index].SustainLevel = data; }
    inline float  GetFltSustainLevel    (byte index)                { return (this->Cs.FltEnv[index].SustainLevel); }
    inline void   SetFltAttackTime      (byte index, float data)    { this->Cs.FltEnv[index].AttackTime = data; }
    inline float  GetFltAttackTime      (byte index)                { return (this->Cs.FltEnv[index].AttackTime); }
    inline void   SetFltDecayTime       (byte index, float data)    { this->Cs.FltEnv[index].DecayTime = data; }
    inline float  GetFltDecayTime       (byte index)                { return (this->Cs.FltEnv[index].DecayTime); }
    inline void   SetFltReleaseTime     (byte index, float data)    { this->Cs.FltEnv[index].ReleaseTime = data; }
    inline float  GetFltReleaseTime     (byte index)                { return (this->Cs.FltEnv[index].ReleaseTime); }
    inline void   SetOutputMask         (byte chanmap)              { this->Cs.OutputMask = chanmap; }
    inline byte   GetOutputMask         (void)                      { return (this->Cs.OutputMask); }
    inline void   SetFltCtrl            (byte index, int data)      { this->Cs.FilterCtrl[index] = data; }
    inline byte   GetFltCtrl            (byte index)                { return (this->Cs.FilterCtrl[index]); }
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
            bool    ModLevelAlt;
            bool    Select[SOURCE_CNT_LFO];
            } CfgLFO[2];
        } Cs;

    String          Name;
    byte    ButtonStateLfo[XL_BUTTON_COUNT];

public:
    SYNTH_VOICE_CONFIG_C  Voice[VOICE_COUNT];

         SYNTH_CONFIG_C (void);
    void Save                       (short num);
    void Load                       (short num);
    void InitButtonsXL              (void);

    inline byte*  GetButtonStateLfo (void)                                  { return (ButtonStateLfo); }
    inline void   SetName           (String& name)                          { this->Name = name; }
    inline String Getname           (void)                                  { return (this->Name); }
    inline void   SetSoftFreq       (short data)                            { this->Cs.SoftFrequency = data; }
    inline short  GetSoftFreq       (void)                                  { return (this->Cs.SoftFrequency); };
    inline void   SetModMidi        (byte index, short data)                { this->Cs.LfoMidi[index] = data; }
    inline short  GetModMidi        (byte index)                            { return (this->Cs.LfoMidi[index]); };
    inline void   SetModSoftMixer   (short index, bool data)                { this->Cs.SoftMixerLFO[index] = data; }
    inline bool   GetModSoftMixer   (short unit)                            { return (this->Cs.SoftMixerLFO[unit]); }
    inline void   SetSelect         (short unit, short index, bool data)    { this->Cs.CfgLFO[unit].Select[index] = data; }
    inline bool   GetSelect         (short unit, short index)               { return (this->Cs.CfgLFO[unit].Select[index]); }
    inline void   SetRampDir        (short unit, bool data)                 { this->Cs.CfgLFO[unit].RampDir = data; }
    inline bool   GetRampDir        (short unit)                            { return (this->Cs.CfgLFO[unit].RampDir); }
    inline void   SetFrequency      (short unit, short data)                { this->Cs.CfgLFO[unit].Frequency = data; }
    inline short  GetFrequency      (short unit)                            { return (this->Cs.CfgLFO[unit].Frequency); }
    inline void   SetPulseWidth     (short unit, short data)                { this->Cs.CfgLFO[unit].PulseWidth = data; }
    inline short  GetPulseWidth     (short unit)                            { return (this->Cs.CfgLFO[unit].PulseWidth); }
    inline void   SetModLevelAlt    (short unit, bool data)                 { this->Cs.CfgLFO[unit].ModLevelAlt = data; }
    inline bool   GetModLevelAlt    (short unit)                            { return (this->Cs.CfgLFO[unit].ModLevelAlt); }
    };


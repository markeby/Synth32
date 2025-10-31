//#######################################################################
// Module:     Config.h
// Descrption: Configuration data for synth
// Creator:    markeby
// Date:       2/16/2025
//#######################################################################
#pragma once
#include <Streaming.h>
#include <ArduinoJson.h>

#include <SynthCommon.h>
#include "I2Cdevices.h"
#include "Debug.h"

//#####################################
// Debug controls
//#####################################
#define DEBUG_SYNTH           1
//#define DEBUG_MIDI_MSG
//#define DEBUG_FUNC

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
extern bool DebugSeq;

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
#define KEYS_FULL           128
#define KEYS_FIRST          12
#define KEYS_LAST           108
#define KEYS_SYNTH          ((KEYS_LAST - KEYS_FIRST) + 1)
#define DA_RANGE            4096
#define MAX_DA              (DA_RANGE - 1)
#define NOTES_PER_OCTAVE    12
#define PITCH_BEND_CENTER   2048

//#################################################
//    Midi control mapping
//#################################################
typedef struct
    {
    short       Index;      // Value passed into function
    byte        Color;      // Default color code for Novation device
    const char* Desc;
    void       (*CallBack)(short chan, short data);
    }  XL_MIDI_MAP;


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
#define XL_MIDI_MAP_TUNING      5

#define XL_BUTTON_START         24
#define XL_BUTTON_END           40
#define XL_BUTTON_COUNT         (XL_BUTTON_END - XL_BUTTON_START)

//#################################################
//  Synth interfaces
//#################################################
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
        byte    Damper;
        float   MaxLevel;
        float   MinLevel;
        float   SustainLevel;
        float   AttackTime;
        float   DecayTime;
        float   ReleaseTime;
        }  ENVELOPE_T;
    struct
        {
        byte        MapVoiceMidi;
        short       OutputEnable;
        short       MapVoiceNoise;
        float       PulseWidth;
        bool        RampDirection;
        byte        OutputMask;
        byte        FilterCtrl[FILTER_DEVICES];
        ENVELOPE_T  OscEnv[OSC_MIXER_COUNT];
        ENVELOPE_T  FltEnv[FILTER_DEVICES];
        }   Cs;

    String Name;
    bool   Mute;

    void   CreateEnvJSON        (JsonVariant cfg, ENVELOPE_T& env);
    void   LoadEnvJSON          (JsonVariant cfg, ENVELOPE_T& env);

public:
    bool   SelectedOscEnvelope[OSC_MIXER_COUNT];
    bool   SelectedFltEnvelope[FILTER_DEVICES];

    void   CreateJSON           (JsonVariant cfg);
    void   LoadJSON             (JsonVariant cfg);

           SYNTH_VOICE_CONFIG_C (void);
    void   Save                 (const char* name);
    void   Load                 (const char* name);

    void   SetMute              (bool state)               { Mute = state; }
    bool   IsMute               (void)                     { return (Mute); }
    void   SetDamperMode        (byte index, byte mode)    { Cs.OscEnv[index].Damper = mode; }
    byte   GetDamperMode        (byte index)               { return (Cs.OscEnv[index].Damper); }
    void   SetName              (String& name)             { Name = name;}
    String Getname              (void)                     { return (Name); }
    void   SetVoiceMidi         (byte data)                { Cs.MapVoiceMidi = data; }
    byte   GetVoiceMidi         (void)                     { return (Cs.MapVoiceMidi); }
    void   SetVoiceNoise        (short data)               { Cs.MapVoiceNoise = data; }
    short  GetVoiceNoise        (void)                     { return (Cs.MapVoiceNoise); }
    void   SetRampDirection     (bool data)                { Cs.RampDirection = data; }
    bool   GetRampDirection     (void)                     { return (Cs.RampDirection); }
    void   SetPulseWidth        (float data)               { Cs.PulseWidth = data; }
    float  GetPulseWidth        (void)                     { return (Cs.PulseWidth); }
    void   SetOscMaxLevel       (byte index, float data)   { Cs.OscEnv[index].MaxLevel = data; }
    float  GetOscMaxLevel       (byte index)               { return (Cs.OscEnv[index].MaxLevel); }
    void   SetOscSustainLevel   (byte index, float data)   { Cs.OscEnv[index].SustainLevel = data; }
    float  GetOscSustainLevel   (byte index)               { return (Cs.OscEnv[index].SustainLevel); }
    void   SetOscAttackTime     (byte index, float data)   { Cs.OscEnv[index].AttackTime = data; }
    float  GetOscAttackTime     (byte index)               { return (Cs.OscEnv[index].AttackTime); }
    void   SetOscDecayTime      (byte index, float data)   { Cs.OscEnv[index].DecayTime = data; }
    float  GetOscDecayTime      (byte index)               { return (Cs.OscEnv[index].DecayTime); }
    void   SetOscReleaseTime    (byte index, float data)   { Cs.OscEnv[index].ReleaseTime = data; }
    float  GetOscReleaseTime    (byte index)               { return (Cs.OscEnv[index].ReleaseTime); }

    void   SetFltStart          (byte index, float data)   { Cs.FltEnv[index].MinLevel = data; }
    float  GetFltStart          (byte index)               { return (Cs.FltEnv[index].MinLevel); }
    void   SetFltEnd            (byte index, float data)   { Cs.FltEnv[index].MaxLevel = data; }
    float  GetFltEnd            (byte index)               { return (Cs.FltEnv[index].MaxLevel); }
    void   SetFltSustainLevel   (byte index, float data)   { Cs.FltEnv[index].SustainLevel = data; }
    float  GetFltSustainLevel   (byte index)               { return (Cs.FltEnv[index].SustainLevel); }
    void   SetFltAttackTime     (byte index, float data)   { Cs.FltEnv[index].AttackTime = data; }
    float  GetFltAttackTime     (byte index)               { return (Cs.FltEnv[index].AttackTime); }
    void   SetFltDecayTime      (byte index, float data)   { Cs.FltEnv[index].DecayTime = data; }
    float  GetFltDecayTime      (byte index)               { return (Cs.FltEnv[index].DecayTime); }
    void   SetFltReleaseTime    (byte index, float data)   { Cs.FltEnv[index].ReleaseTime = data; }
    float  GetFltReleaseTime    (byte index)               { return (Cs.FltEnv[index].ReleaseTime); }
    void   SetOutputMask        (byte chanmap)             { Cs.OutputMask = chanmap; }
    byte   GetOutputMask        (void)                     { return (Cs.OutputMask); }
    void   SetFltCtrl           (byte index, int data)     { Cs.FilterCtrl[index] = data; }
    byte   GetFltCtrl           (byte index)               { return (Cs.FilterCtrl[index]); }
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

public:
    SYNTH_VOICE_CONFIG_C  Voice[VOICE_COUNT];

            SYNTH_CONFIG_C      (void);
            bool   Begin        (void);
    void    Save                (short num);
    void    Load                (short num);
    void    CreateJSON          (JsonDocument& cfg, short num);
    void    LoadJSON            (JsonDocument& cfg, short num);

    void   SetSoftFreq          (short data)                            { Cs.SoftFrequency = data; }
    short  GetSoftFreq          (void)                                  { return (Cs.SoftFrequency); };
    void   SetModMidi           (byte index, short data)                { Cs.LfoMidi[index] = data; }
    short  GetModMidi           (byte index)                            { return (Cs.LfoMidi[index]); };
    void   SetModSoftMixer      (short index, bool data)                { Cs.SoftMixerLFO[index] = data; }
    bool   GetModSoftMixer      (short unit)                            { return (Cs.SoftMixerLFO[unit]); }
    void   SetSelect            (short unit, short index, bool data)    { Cs.CfgLFO[unit].Select[index] = data; }
    bool   GetSelect            (short unit, short index)               { return (Cs.CfgLFO[unit].Select[index]); }
    void   SetRampDir           (short unit, bool data)                 { Cs.CfgLFO[unit].RampDir = data; }
    bool   GetRampDir           (short unit)                            { return (Cs.CfgLFO[unit].RampDir); }
    void   SetFrequency         (short unit, short data)                { Cs.CfgLFO[unit].Frequency = data; }
    short  GetFrequency         (short unit)                            { return (Cs.CfgLFO[unit].Frequency); }
    void   SetPulseWidth        (short unit, short data)                { Cs.CfgLFO[unit].PulseWidth = data; }
    short  GetPulseWidth        (short unit)                            { return (Cs.CfgLFO[unit].PulseWidth); }
    void   SetModLevelAlt       (short unit, bool data)                 { Cs.CfgLFO[unit].ModLevelAlt = data; }
    bool   GetModLevelAlt       (short unit)                            { return (Cs.CfgLFO[unit].ModLevelAlt); }
    };


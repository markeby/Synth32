//#######################################################################
// Module:     SynthFront.h
// Descrption: Interface to the synth channels
// Creator:    markeby
// Date:       8/25/2023
//#######################################################################
#pragma once

#include "Envelope.h"

namespace SYNTH_FRONT
    {
    void  KeyDown (byte channel, byte key, byte velocity);
    void  KeyUp (byte channel, byte key, byte velocity);

        /*! Enumeration of MIDI types */
    enum MidiType: byte
        {
        InvalidType           = 0x00,    ///< For notifying errors
        NoteOff               = 0x80,    ///< Channel Message - Note Off
        NoteOn                = 0x90,    ///< Channel Message - Note On
        AfterTouchPoly        = 0xA0,    ///< Channel Message - Polyphonic AfterTouch
        ControlChange         = 0xB0,    ///< Channel Message - Control Change / Channel Mode
        ProgramChange         = 0xC0,    ///< Channel Message - Program Change
        AfterTouchChannel     = 0xD0,    ///< Channel Message - Channel (monophonic) AfterTouch
        PitchBend             = 0xE0,    ///< Channel Message - Pitch Bend
        SystemExclusive       = 0xF0,    ///< System Exclusive
        SystemExclusiveStart  = SystemExclusive,   ///< System Exclusive Start
        TimeCodeQuarterFrame  = 0xF1,    ///< System Common - MIDI Time Code Quarter Frame
        SongPosition          = 0xF2,    ///< System Common - Song Position Pointer
        SongSelect            = 0xF3,    ///< System Common - Song Select
        Undefined_F4          = 0xF4,
        Undefined_F5          = 0xF5,
        TuneRequest           = 0xF6,    ///< System Common - Tune Request
        SystemExclusiveEnd    = 0xF7,    ///< System Exclusive End
        Clock                 = 0xF8,    ///< System Real Time - Timing Clock
        Undefined_F9          = 0xF9,
        Tick                  = Undefined_F9, ///< System Real Time - Timing Tick (1 tick = 10 milliseconds)
        Start                 = 0xFA,    ///< System Real Time - Start
        Continue              = 0xFB,    ///< System Real Time - Continue
        Stop                  = 0xFC,    ///< System Real Time - Stop
        Undefined_FD          = 0xFD,
        ActiveSensing         = 0xFE,    ///< System Real Time - Active Sensing
        SystemReset           = 0xFF,    ///< System Real Time - System Reset
        };

    typedef struct ACTION_C
        {
        byte MidiValue;
        byte MidiType;
        byte MidiStatus;
        MIDI_VALUE_MAP* Params;
        } ACTION_TC;

    } // end namespace SYNTH_FRONT

//#################################################
//    Synthesizer front end class
//#################################################
class   SYNTH_FRONT_C
    {
private:
    byte                  DownKey;
    byte                  DownVelocity;
    bool                  DownTrigger;
    byte                  UpKey;
    byte                  UpVelocity;
    bool                  UpTrigger;
    uint16_t              LastOp;
    int                   NoiseColorDev;
    bool                  SelectedEnvelope[ENVELOPE_COUNT];
    int                   InTuning;
    int                   SetTuning;
    int                   InTuning2;
    int                   SetTuning2;
    uint16_t              TuningLevel[ENVELOPE_COUNT+1];
    bool                  InDispReset;
    byte                  NoiseFilterSetting;     // 0 - 3
    uint64_t              DispMessageTimer;
    MIDI_VALUE_MAP*       FaderMap;
    MIDI_VALUE_MAP*       KnobMap;
    MIDI_SWITCH_MAP*      SwitchMap;
    ENVELOPE_GENERATOR_C  EnvADSL;

    typedef struct
        {
        byte    BaseLevel;
        byte    MaxLevel;
        byte    AttackTime;
        byte    DecayTime;
        byte    SustainLevel;
        byte    SustainTime;
        byte    ReleaseTime;
        }
    MIDI_ADSR_T;

    MIDI_ADSR_T     MidiAdsr[ENVELOPE_COUNT];
    MIDI_ADSR_T     Noise[2];
    bool            NoiseFilterBits[2];

    String Selected (void);

public:
          SYNTH_FRONT_C      (MIDI_VALUE_MAP* fader_map, MIDI_VALUE_MAP* knob_map, MIDI_SWITCH_MAP* switch_map);
    void  Begin              (int osc_d_a, int noise_d_a, int noise_dig);
    void  DispMessageHandler (byte cmd);
    void  Loop               (void);
    void  Controller         (byte chan, byte type, byte value);
    void  PitchBend          (byte chan, int value);
    void  ChannelSetSelect   (byte chan, bool state);
    void  SetReverse         (bool data);
    void  StartTuning        (int setting);
    void  StartTuning2       (int setting);
    void  SelectWaveLFO      (byte ch, byte state);
    void  FreqSelectLFO      (byte ch, byte data);
    void  LFOrange           (bool up);
    void  SetLevelLFO        (byte data);
    void  SetMaxLevel        (byte ch, byte data);
    void  SetMBaselevel      (byte ch, byte data);
    void  SetAttackTime      (byte data);
    void  SetDecayTime       (byte data);
    void  SetSustainLevel    (byte ch, byte data);
    void  SetSustainTime     (byte data);
    void  SetReleaseTime     (byte data);
    void  DISP32UpdateAll    (void);
    void  NoiseFilter        (byte bit, bool state);
    void  NoiseColor         (byte val);
    void  SetNoiseFilterMin  (byte data);
    void  SetNoiseFilterMax  (byte data);

    //#######################################################################
    inline void  KeyDown (byte chan, byte key, byte velocity)
        {
        DownKey      = key;
        DownVelocity = velocity;
        DownTrigger  = true;
        }

    //#######################################################################
    inline void  KeyUp (byte chan, byte key, byte velocity)
        {
        UpKey      = key;
        UpVelocity = velocity;
        UpTrigger  = true;
        }

    //#######################################################################
    inline int IsInTuning (void)
        {
        return (InTuning);
        }

    };

//#################################################

extern SYNTH_FRONT_C SynthFront;        // Synth front end class


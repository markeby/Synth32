//#######################################################################
// Module:     SynthFront.h
// Descrption: Interface to the synth channels
// Creator:    markeby
// Date:       8/25/2023
//#######################################################################
#pragma once

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
    byte                DownKey;
    byte                DownVelocity;
    bool                DownTrigger;
    byte                UpKey;
    byte                UpVelocity;
    bool                UpTrigger;
    uint16_t            LastOp;
    bool                SelectWaveShapeVCO[OSC_MIXER_COUNT];
    int                 InTuning;
    int                 SetTuning;
    int                 SetDeviceIndex;
    bool                InDispReset;
    uint64_t            DispMessageTimer;
    MIDI_VALUE_MAP*     FaderMap;
    MIDI_VALUE_MAP*     KnobMap;
    MIDI_SWITCH_MAP*    SwitchMap;

    typedef struct
        {
        byte    MaxLevel;
        byte    AttackTime;
        byte    DecayTime;
        byte    SustainLevel;
        byte    SustainTime;
        byte    ReleaseTime;
        }
    MIDI_ADSR_T;

    MIDI_ADSR_T     MidiAdsr[OSC_MIXER_COUNT];

    String Selected (void);

public:
          SYNTH_FRONT_C         (int first_device, MIDI_VALUE_MAP* fader_map, MIDI_VALUE_MAP* knob_map, MIDI_SWITCH_MAP* switch_map);
    void  Begin                 (void);
    void  DispMessageHandler    (byte cmd);
    void  Loop                  (void);
    void  Controller            (byte chan, byte type, byte value);
    void  PitchBend             (byte chan, int value);
    void  OscChannelSelect      (byte chan, bool state);
    void  SetSawReverse         (bool data);
    void  SelectWaveLFO         (byte ch, byte state);
    void  FreqSelectLFO         (byte ch, byte data);
    void  LFOrange              (bool up);
    void  SetLevelLFO           (byte data);
    void  SetOscMaxLevel        (byte ch, byte data);
    void  SetOscAttackTime      (byte data);
    void  SetOscDecayTime       (byte data);
    void  SetOscSustainLevel    (byte ch, byte data);
    void  SetOscSustainTime     (byte data);
    void  SetOscReleaseTime     (byte data);
    void  DISP32UpdateAll       (void);

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
    inline void StartTuning (int setting)
        {
        SetTuning = setting;
        }
    };

//#################################################

extern SYNTH_FRONT_C SynthFront;        // Synth front end class


//#######################################################################
// Module:     SynthFront.h
// Descrption: Interface to the synth channels
// Creator:    markeby
// Date:       8/25/2023
//#######################################################################
#pragma once

#define CHAN_COUNT 6    // number of synth oscillaotor/filter/control channels

namespace SYNTH_FRONT
    {
    void  KeyDown (byte channel, byte key, byte velocity);
    void  KeyUp (byte channel, byte key, byte velocity);

        /*! Enumeration of MIDI types */
    enum MidiType: uint8_t
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
    uint8_t             DownKey;
    uint8_t             DownVelocity;
    bool                DownTrigger;
    uint8_t             UpKey;
    uint8_t             UpVelocity;
    bool                UpTrigger;
    uint16_t            LastOp;
    bool                SelectWaveShapeVCO[OSC_MIXER_COUNT];
    int                 InTuning;
    int                 SetTuning;
    int                 SetDeviceIndex;

    MIDI_VALUE_MAP*     FaderMap;
    MIDI_VALUE_MAP*     KnobMap;
    MIDI_SWITCH_MAP*    SwitchMap;
    MIDI_VALUE_MAP*     PitchMap;

    String Selected (void);

public:
          SYNTH_FRONT_C     (int first_device, MIDI_VALUE_MAP* fader_map, MIDI_VALUE_MAP* knob_map, MIDI_VALUE_MAP* pitch_map, MIDI_SWITCH_MAP* switch_map);
    void  Begin             (void);
    void  DisplayCommanded  (byte cmd);
    void  Loop              (void);
    void  Controller        (byte chan, byte type, byte value);
    void  PitchBend         (byte chan, int value);
    void  ChannelSelect     (uint8_t chan, bool state);
    void  SelectWaveLFO     (uint8_t ch, uint8_t state);
    void  FreqSelectLFO     (uint8_t ch, float val);
    void  PitchBend         (float val);
    void  LFOrange          (bool up);
    void  SetLevelLFO       (float data);
    void  SetMaxLevel       (uint8_t wave, float data);
    void  SetAttack         ( float data);
    void  SetDecay          (float data);
    void  SetSustainLevel   (uint8_t ch, float data);
    void  SetSustainTime    (float data);
    void  SetRelease        (float data);

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


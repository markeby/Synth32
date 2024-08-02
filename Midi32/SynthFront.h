//#######################################################################
// Module:     SynthFront.h
// Descrption: Interface to the synth channels
// Creator:    markeby
// Date:       8/25/2023
//#######################################################################
#pragma once

#include "SineWave.h"
#include "Envelope.h"
#include "I2Cmessages.h"

namespace SYNTH_FRONT
    {
    void  KeyDown (uint8_t channel, uint8_t key, uint8_t velocity);
    void  KeyUp (uint8_t channel, uint8_t key, uint8_t velocity);

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
        uint8_t MidiValue;
        uint8_t MidiType;
        uint8_t MidiStatus;
        MIDI_VALUE_MAP* Params;
        } ACTION_TC;

    } // end namespace SYNTH_FRONT

//#################################################
//    Synthesizer front end class
//#################################################
class   SYNTH_FRONT_C
    {
private:
    uint8_t               DownKey;
    uint8_t               DownVelocity;
    bool                  DownTrigger;
    uint8_t               UpKey;
    uint8_t               UpVelocity;
    bool                  UpTrigger;
    uint16_t              LastOp;
    int                   NoiseColorDev;
    bool                  SelectedEnvelope[ENVELOPE_COUNT];
    uint8_t               NoiseFilterSetting;     // 0 - 3
    uint64_t              DispMessageTimer;
    bool                  SawToothDirection;
    byte                  PulseWidth;
    MIDI_VALUE_MAP*       FaderMap;
    MIDI_VALUE_MAP*       KnobMap;
    MIDI_SWITCH_MAP*      SwitchMap;
    ENVELOPE_GENERATOR_C  EnvADSL;
    SINEWAVE_C            SineWave;
    uint16_t              TuningLevel[ENVELOPE_COUNT+1];
    bool                  TuningOn[CHAN_COUNT];
    bool                  SetTuning;

    typedef struct
        {
        uint8_t    BaseLevel;
        uint8_t    MaxLevel;
        uint8_t    AttackTime;
        uint8_t    DecayTime;
        uint8_t    SustainLevel;
        uint8_t    SustainTime;
        uint8_t    ReleaseTime;
        }
    MIDI_ADSR_T;

    MIDI_ADSR_T     MidiAdsr[ENVELOPE_COUNT];
    MIDI_ADSR_T     Noise[2];
    bool            NoiseFilterBits[2];

    String Selected (void);

public:
          SYNTH_FRONT_C      (MIDI_VALUE_MAP* fader_map, MIDI_VALUE_MAP* knob_map, MIDI_SWITCH_MAP* switch_map);
    void  Begin              (int osc_d_a, int noise_d_a, int noise_dig);
    void  Loop               (void);
    void  Controller         (uint8_t chan, uint8_t type, uint8_t value);
    void  PitchBend          (uint8_t chan, int value);
    void  ChannelSetSelect   (uint8_t chan, bool state);
    void  SawtoothDirection  (bool data);
    void  SetPulseWidth      (byte data);
    void  Tuning             (void);
    void  StartTuning        (void);
    void  SelectWaveLFO      (uint8_t ch, uint8_t state);
    void  FreqSelectLFO      (uint8_t ch, uint8_t data);
    void  LFOrange           (bool up);
    void  SetLevelLFO        (uint8_t data);
    void  SetMaxLevel        (uint8_t ch, uint8_t data);
    void  SetMBaselevel      (uint8_t ch, uint8_t data);
    void  SetAttackTime      (uint8_t data);
    void  SetDecayTime       (uint8_t data);
    void  SetSustainLevel    (uint8_t ch, uint8_t data);
    void  SetSustainTime     (uint8_t data);
    void  SetReleaseTime     (uint8_t data);
    void  NoiseFilter        (uint8_t bit, bool state);
    void  NoiseColor         (uint8_t val);
    void  SetNoiseFilterMin  (uint8_t data);
    void  SetNoiseFilterMax  (uint8_t data);
    void  DisplayUpdate      (void);

    //#######################################################################
    inline bool IsInTuning (void)
        {
        return (this->SetTuning);
        }

    //#######################################################################
    inline void KeyDown (uint8_t chan, uint8_t key, uint8_t velocity)
        {
        this->DownKey      = key;
        this->DownVelocity = velocity;
        this->DownTrigger  = true;
        }

    //#######################################################################
    inline void KeyUp (uint8_t chan, uint8_t key, uint8_t velocity)
        {
        this->UpKey      = key;
        this->UpVelocity = velocity;
        this->UpTrigger  = true;
        }
    };

//#################################################

extern SYNTH_FRONT_C SynthFront;        // Synth front end class


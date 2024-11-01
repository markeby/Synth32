//#######################################################################
// Module:     FrontEnd.h
// Descrption: Interface to the synth channels
// Creator:    markeby
// Date:       8/25/2023
//#######################################################################
#pragma once

#include "LFOosc.h"
#include "SoftLFO.h"
#include "SynthChannel.h"
#include "Envelope.h"

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
    int                   NoiseColorDev;
    bool                  SelectedEnvelope[ENVELOPE_COUNT];
    uint8_t               NoiseFilterSetting;     // 0 - 3
    uint64_t              DispMessageTimer;
    bool                  SawToothDirection;
    byte                  PulseWidth;
    MIDI_MAP*             FaderMap;
    MIDI_MAP*             KnobMap;
    MIDI_MAP*             SwitchMap;
    MIDI_XL_MAP*          XlMap;
    ENVELOPE_GENERATOR_C  EnvADSL;
    uint16_t              TuningLevel[ENVELOPE_COUNT+1];
    bool                  TuningOn[CHAN_COUNT];
    bool                  SetTuning;
    byte                  ClearEntryRed;
    byte                  ClearEntryRedL;
    SYNTH_CHANNEL_C*      pChan[CHAN_COUNT];
    SYNTH_LFO_C           Lfo;

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

    String Selected (void);

public:
          SYNTH_FRONT_C      (MIDI_MAP* fader_map, MIDI_MAP* knob_map, MIDI_MAP* switch_map, MIDI_XL_MAP* xl_map);
    void  Begin              (int osc_d_a);
    void  ResetXL            (void);
    void  Loop               (void);
    void  Clear              (void);
    void  Controller         (uint8_t chan, uint8_t type, uint8_t value);
    void  PitchBend          (uint8_t chan, int value);
    void  ChannelSetSelect   (uint8_t chan, bool state);
    void  SawtoothDirection  (bool data);
    void  SetPulseWidth      (byte data);
    void  Tuning             (void);
    void  TuningAdjust       (bool up);
    void  StartTuning        (void);
    void  SelectWaveVCA      (uint8_t ch, uint8_t state);
    void  SelectWaveVCF      (uint8_t ch, uint8_t state);
    void  FreqLFO            (byte ch, byte data);

    void  SetLevelLFO        (uint8_t data);
    void  SetMaxLevel        (uint8_t ch, uint8_t data);
    void  SetMBaselevel      (uint8_t ch, uint8_t data);
    void  SetAttackTime      (uint8_t data);
    void  SetDecayTime       (uint8_t data);
    void  SetSustainLevel    (uint8_t ch, uint8_t data);
    void  SetSustainTime     (uint8_t data);
    void  SetReleaseTime     (uint8_t data);
    void  DisplayUpdate      (void);
    void  SaveAllSettings    (void);

    //#######################################################################
    inline void SetClearKeyRed (byte key)
        {
        ClearEntryRed = key;
        }

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


//#######################################################################
// Module:     FrontEnd.h
// Descrption: Interface to the synth channels
// Creator:    markeby
// Date:       8/25/2023
//#######################################################################
#pragma once

#include "LFOosc.h"
#include "SoftLFO.h"
#include "Channel.h"
#include "Envelope.h"
#include "multiplex.h"
#include "Noise.h"


enum {
     ZONE0 = 0,
     ZONE1,
     ZONE2
     };

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
    uint64_t              DispMessageTimer;
    bool                  UpTrigger;

    MIDI_MAP*             FaderMap;
    MIDI_MAP*             KnobMap;
    MIDI_MAP*             SwitchMap;
    MIDI_XL_MAP*          XlMap;
    ENVELOPE_GENERATOR_C  EnvADSL;
    MULTIPLEX_C*          pMultiplexer;
    SYNTH_LFO_C           Lfo;
    NOISE_C*              pNoise;

    CHANNEL_C*            pChan[CHAN_COUNT];
    byte                  Zone[3];

    bool                  SetTuning;
    byte                  ClearEntryRed;
    byte                  ClearEntryRedL;
    uint16_t              TuningLevel[ENVELOPE_COUNT+1];
    bool                  TuningOn[CHAN_COUNT];
    bool                  TuningChange;

    String Selected         (void);
    void   ShowChannelXL    (int val);

public:
    byte  CurrentZone;
    byte  ZoneCount;

          SYNTH_FRONT_C      (MIDI_MAP* fader_map, MIDI_MAP* knob_map, MIDI_MAP* switch_map, MIDI_XL_MAP* xl_map);
    void  Begin              (int osc_d_a, int mult_digital, int noise_digital);
    void  ResetXL            (void);
    void  Loop               (void);
    void  Clear              (void);
    void  DualZone           (byte chan, bool state);
    void  Controller         (byte chan, byte type, byte value);

    // FrontEndLFO.cpp
    void  PitchBend          (byte chan, int value);
    void  SelectWaveVCA      (byte ch, byte state);
    void  SelectWaveVCF      (byte ch, byte state);
    void  SetLevelLFO        (byte data);
    void  FreqLFO            (byte ch, byte data);

    // FrontEndOscCtrl.cpp
    void  ChannelSetSelect   (byte chan, bool state);
    void  SetMaxLevel        (byte ch, byte data);
    void  SetAttackTime      (byte data);
    void  SetDecayTime       (byte data);
    void  SetSustainLevel    (byte ch, byte data);
    void  SetSustainTime     (byte data);
    void  SetReleaseTime     (byte data);
    void  SawtoothDirection  (bool data);
    void  SetPulseWidth      (byte data);
    void  SetNoise           (byte ch, bool state);
    void  SaveAllSettings    (void);


    void  Tuning             (void);
    void  TuningAdjust       (bool up);
    void  StartTuning        (void);

    void  DisplayUpdate      (int zone);

    //#######################################################################
    MULTIPLEX_C* Multiplex (void);
    NOISE_C*     Noise     (void);

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
    inline void KeyDown (byte chan, byte key, byte velocity)
        {
        this->DownKey      = key;
        this->DownVelocity = velocity;
        this->DownTrigger  = true;
        }

    //#######################################################################
    inline void KeyUp (byte chan, byte key, byte velocity)
        {
        this->UpKey      = key;
        this->UpVelocity = velocity;
        this->UpTrigger  = true;
        }
    };

//#################################################

extern SYNTH_FRONT_C SynthFront;        // Synth front end class


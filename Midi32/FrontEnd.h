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

    typedef struct
        {
        byte    Channel;
        byte    Key;
        byte    Velocity;
        bool    Trigger;
        } KEY_T;

    KEY_T                 Down;
    KEY_T                 Up;
    uint64_t              DispMessageTimer;

    MIDI_MAP*             FaderMap;
    MIDI_BUTTON_MAP*      ButtonMap;
    MIDI_ENCODER_MAP*     KnobMap;
    MIDI_XL_MAP*          XlMap;
    ENVELOPE_GENERATOR_C  EnvADSL;
    MULTIPLEX_C*          pMultiplexer;
    SYNTH_LFO_C           Lfo[2];
    NOISE_C*              pNoise;

    CHANNEL_C*            pChan[CHAN_COUNT];
    byte                  Zone[3];
    byte                  ZoneBase;

    bool                  SetTuning;
    byte                  ClearEntryRed;
    byte                  ClearEntryRedL;
    uint16_t              TuningLevel[ENVELOPE_COUNT+1];
    bool                  TuningOn[CHAN_COUNT];
    bool                  TuningChange;

    String Selected             (void);
    void   ShowChannelXL        (int val);

public:
    byte  CurrentZone;
    byte  ZoneCount;

          SYNTH_FRONT_C         (MIDI_MAP* fader_map, MIDI_ENCODER_MAP* knob_map, MIDI_BUTTON_MAP* button_map, MIDI_XL_MAP* xl_map);
    void  Begin                 (int osc_d_a, int mult_digital, int noise_digital, int lfo_digital);
    void  ResetXL               (void);
    void  Loop                  (void);
    void  Clear                 (void);
    void  DualZone              (short chan, bool state);
    void  Controller            (byte short, byte type, byte value);

    //#######################################################################
    // FrontEndLFO.cpp
    void  PitchBend             (short chan, short value);
    void  ToggleSelectModVCA    (short ch);
    void  FreqLFO               (short ch, short data);
    void  ToggleSelectWaveVCO   (short ch);
    void  ToggleRampSlope       (void);
    void  SetLevelLFO           (short data);

    // FrontEndOscCtrl.cpp
    void  ChannelSetSelect      (short chan, bool state);
    void  SetMaxLevel           (short ch, short data);
    void  SetAttackTime         (short data);
    void  SetDecayTime          (short data);
    void  SetSustainLevel       (short ch, short data);
    void  SetReleaseTime        (short data);
    void  SawtoothDirection     (bool data);
    void  SetPulseWidth         (short data);
    void  SetNoise              (short ch, bool state);
    void  SaveAllSettings       (void);


    void  Tuning                (void);
    void  TuningAdjust          (bool up);
    void  StartTuning           (void);

    void  DisplayUpdate         (int zone);

    //#######################################################################
    MULTIPLEX_C* Multiplex      (void);
    NOISE_C*     Noise          (void);

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
        this->Down.Channel  = chan;
        this->Down.Key      = key;
        this->Down.Velocity = velocity;
        this->Down.Trigger  = true;
        }

    //#######################################################################
    inline void KeyUp (byte chan, byte key, byte velocity)
        {
        this->Up.Channel  = chan;
        this->Up.Key      = key;
        this->Up.Velocity = velocity;
        this->Up.Trigger  = true;
        }
    };

//#################################################

extern SYNTH_FRONT_C SynthFront;        // Synth front end class


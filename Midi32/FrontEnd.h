//#######################################################################
// Module:     FrontEnd.h
// Descrption: Interface to the synth channels
// Creator:    markeby
// Date:       8/25/2023
//#######################################################################
#pragma once

#include "Config.h"
#include "LFOosc.h"
#include "SoftLFO.h"
#include "Voice.h"
#include "Envelope.h"
#include "multiplex.h"
#include "Noise.h"

//#################################################
//    Synthesizer front end class
//#################################################
class   SYNTH_FRONT_C
    {
private:

    typedef struct
        {
        byte    Port;
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

    bool                  MapSelectMode;
    short                 CurrentMapSelected;
    SYNTH_CONFIG_C        SynthConfig[VOICE_COUNT];
    short                 CurrentMidiSelected;

    VOICE_C*              pVoice[VOICE_COUNT];
    short                 PitchBendOffset;
    byte                  ModulationVCA[VOICE_COUNT];

    bool                  SetTuning;
    bool                  TuningBender;
    byte                  ClearEntryRed;
    byte                  ClearEntryRedL;
    uint16_t              TuningLevel[ENVELOPE_COUNT+1];
    bool                  TuningOn[VOICE_COUNT];
    bool                  TuningChange;


    String Selected       (void);
    void   ShowVoiceXL    (int val);

public:
    byte  CurrentZone;
    byte  ZoneCount;

         SYNTH_FRONT_C              (MIDI_MAP* fader_map, MIDI_ENCODER_MAP* knob_map, MIDI_BUTTON_MAP* button_map, MIDI_XL_MAP* xl_map);
    void Begin                      (int osc_d_a, int mult_digital, int noise_digital, int lfo_digital);
    void ResetXL                    (void);
    void ResetUSB                   (void);

    void Loop                       (void);
    void Clear                      (void);
    void Controller                 (byte short, byte type, byte value);

    void PageAdvance                (void);

    //#######################################################################
    // FrontEndLFO.cpp
    void SelectModVCA               (byte ch, bool state);
    void FreqLFO                    (short ch, short data);

    void     PitchBend              (short value);
    uint16_t GetBenderOffset        (void)
        { return (this->PitchBendOffset); }
    void     SelectModVCO           (short ch, bool state)
        { this->Lfo[0].SetWave (ch, state);   this->Lfo[1].SetWave (ch, state);  }
    void     SetLevelLFO            (short data)
        { this->Lfo[0].SetLevel (data);   this->Lfo[1].SetLevel (data);   }
    void     SetRampDir             (bool state)
        { this->Lfo[0].SetRampDir (state);  this->Lfo[1].SetRampDir (state);}

    void MidiMapMode                (void);
    bool GetMidiMapMode             (void)
        { return (this->MapSelectMode); }
    void MapModeBump                (short down);
    void ChangeMapSelect            (short right);
    bool GetMapSelect               (void)
        { return (this->MapSelectMode); }
    void  ResolveMapAllocation      (void);
    void  SaveDefaultConfig         (void);

    // FrontEndOscCtrl.cpp
    void VoiceComponentSetSelected  (short chan, bool state);
    void SetMaxLevel                (short ch, short data);
    void SetAttackTime              (short data);
    void SetDecayTime               (short data);
    void SetSustainLevel            (short ch, short data);
    void SetReleaseTime             (short data);
    void ToggleRampDirection        (void);
    void SetPulseWidth              (short data);
    void SetNoise                   (short ch, bool state);

    void Tuning                     (void);
    void TuningAdjust               (bool up);
    void TuningBump                 (bool state);
    void StartTuning                (void);
    void SaveTuning                 (void);

    //#######################################################################
    MULTIPLEX_C* Multiplex          (void);
    NOISE_C*     Noise              (void);

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
        this->Down.Port  = chan;
        this->Down.Key      = key;
        this->Down.Velocity = velocity;
        this->Down.Trigger  = true;
        }

    //#######################################################################
    inline void KeyUp (byte chan, byte key, byte velocity)
        {
        this->Up.Port  = chan;
        this->Up.Key      = key;
        this->Up.Velocity = velocity;
        this->Up.Trigger  = true;
        }
    };

//#################################################

extern SYNTH_FRONT_C SynthFront;        // Synth front end class


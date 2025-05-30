//#######################################################################
// Module:     FrontEnd.h
// Descrption: Interface to the synth channels
// Creator:    markeby
// Date:       8/25/2023
//#######################################################################
#pragma once

#include "Config.h"
#include "../Common/DispMessages.h"
#include "LFOosc.h"
#include "SoftLFO.h"
#include "Voice.h"
#include "Envelope.h"

//#################################################
//    Synthesizer front end class
//#################################################
class   SYNTH_FRONT_C
    {
private:

    typedef struct
        {
        byte    Key;
        byte    Velocity;
        byte    Trigger;        // not zero triggers and contains the midi channel
        } KEY_T;

    KEY_T                 Down;
    KEY_T                 Up;
    uint64_t              DispMessageTimer;

    MIDI_MAP*             FaderMap;
    MIDI_BUTTON_MAP*      ButtonMap;
    MIDI_ENCODER_MAP*     KnobMap;
    MIDI_XL_MAP*          XlMap;
    ENVELOPE_GENERATOR_C  EnvADSL;
    SYNTH_LFO_C           Lfo[2];
    VOICE_C*              pVoice[VOICE_COUNT];

    bool                  ResolutionMode;
    bool                  MapSelectMode;
    SYNTH_CONFIG_C        SynthConfig;
    short                 CurrentMidiSelected;
    short                 CurrentMapSelected;
    short                 CurrentVoiceSelected;


    bool                  SetTuning;
    byte                  ClearEntryRed;
    byte                  ClearEntryRedL;
    uint16_t              TuningLevel[ENVELOPE_COUNT+1];
    bool                  TuningOn[VOICE_COUNT];
    bool                  TuningChange;
    ushort                CalibrationReference;
    short                 CalibrationBaseDigital;
    short                 CalibrationPhase;
    short                 CalibrationLFO;
    ushort                CalibrationAtoD;

    String Selected                 (void);
    void   ShowVoiceXL              (int val);
    void   NonOscPageSelect         (DISP_MESSAGE_N::PAGE_C page);
    void   UpdateMapModeDisplay     (int sel);
public:

         SYNTH_FRONT_C              (MIDI_MAP* fader_map, MIDI_ENCODER_MAP* knob_map, MIDI_BUTTON_MAP* button_map, MIDI_XL_MAP* xl_map);
    void Begin                      (short osc_d_a, short mux_digital, short noise_digital, short lfo_analog, short lfo_digital, short mod_mux_digital, short start_a_d);
    void ResetXL                    (void);
    void ResetUSB                   (void);

    void Loop                       (void);
    void Clear                      (void);
    void Controller                 (short mchan, byte type, byte value);

    void PageAdvance                (void);

    //#######################################################################
    // FrontEndLFO.cpp
    void SelectModVCA               (byte ch, bool state);
    void FreqLFO                    (short ch, short data);
    void PitchBend                  (byte mchan, short value);

    void     SetLevelLFO            (short index, byte mchan, short data)
        { this->Lfo[index].SetLevelMidi (mchan, data); }
    void     SelectModVCO           (short index, short ch, bool state)
        { if ( !this->MapSelectMode )   this->Lfo[index].SetWave (ch, state);  this->SynthConfig.SetSelect (index, ch, state); }
    void     SetModRampDir          (short index, bool state)
        { if ( !this->MapSelectMode )   this->Lfo[index].SetRampDir (state);  this->SynthConfig.SetRampDir (index, state); }

    void MidiMapMode                (void);
    bool GetMidiMapMode             (void)
        { return (this->MapSelectMode); }
    void MapModeBump                (short down);
    void ChangeMapSelect            (short right);
    bool GetMapSelect               (void)
        { return (this->MapSelectMode); }
    void ResolveMapAllocation       (void);
    void SaveDefaultConfig          (void);

    //#######################################################################
    // FrontEndOscCtrl.cpp
    void VoiceSelectedCheck             (void);
    void ResetVoiceComponentSetSelected (void);
    void VoiceComponentSetSelected      (short chan, bool state);
    void SetMaxLevel                    (short ch, short data);
    void SetAttackTime                  (short data);
    void SetDecayTime                   (short data);
    void SetSustainLevel                (short ch, short data);
    void SetReleaseTime                 (short data);
    void ToggleRampDirection            (void);
    void SetPulseWidth                  (short data);

    //#######################################################################
    // FrontEndTuning.cpp
    void Tuning                         (void);
    void TuningAdjust                   (bool up);
    void TuningBump                     (bool state);
    void StartTuning                    (void);
    void SaveTuning                     (void);
    void Calibration                    (ushort val);
    void StartCalibration               (void);

    //#######################################################################
    inline void SetClearKeyRed (byte key)
        {
        this->ClearEntryRed = key;
        }

    //#######################################################################
    inline bool IsInTuning (void)
        {
        return (this->SetTuning);
        }

    //#######################################################################
    inline void KeyDown (byte mchan, byte key, byte velocity)
        {
        this->Down.Key      = key;
        this->Down.Velocity = velocity;
        this->Down.Trigger  = mchan;
        }

    //#######################################################################
    inline void KeyUp (byte mchan, byte key, byte velocity)
        {
        this->Up.Key      = key;
        this->Up.Velocity = velocity;
        this->Up.Trigger  = mchan;
        }
    };

//#################################################

extern SYNTH_FRONT_C SynthFront;        // Synth front end class


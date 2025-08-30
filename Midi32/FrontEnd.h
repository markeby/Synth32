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
#include "Novation.h"

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

    G49_FADER_MIDI_MAP         *G49MidiMapFader;
    G49_BUTTON_MIDI_MAP        *G49MidiMapButton;
    G49_ENCODER_MIDI_MAP       *G49MidiMapEcoder;
    XL_MIDI_MAP                (*pMidiMapXL)[XL_MIDI_MAP_SIZE];
    ENVELOPE_GENERATOR_C  EnvADSL;
    SYNTH_LFO_C           Lfo[2];
    VOICE_C*              pVoice[VOICE_COUNT];

    bool                  ResolutionMode;
    bool                  MapSelectMode;
    bool                  LoadSaveMode;
    SYNTH_CONFIG_C        SynthConfig;
    short                 CurrentMidiSelected;
    short                 CurrentMapSelected;
    short                 CurrentVoiceSelected;
    short                 CurrentFilterSelected;
    short                 CurrentDisplayPage;
    short                 LoadSaveSelection;
    NOVATION_XL_C         LaunchControl;
    bool                  SetTuning;
    uint16_t              TuningLevel[ENVELOPE_COUNT+1];
    uint16_t              TuningFlt[FILTER_DEVICES];
    byte                  TuningOutputSelect;
    bool                  TuningChange;
    ushort                CalibrationReference;
    short                 CalibrationBaseDigital;
    short                 CalibrationPhase;
    short                 CalibrationLFO;
    ushort                CalibrationAtoD;

    void   UpdateMapModeDisplay     (int sel);
public:

         SYNTH_FRONT_C              (G49_FADER_MIDI_MAP* g49map_fader, G49_ENCODER_MIDI_MAP* g49map_knob, G49_BUTTON_MIDI_MAP *g49map_button, XL_MIDI_MAP (*xl_map)[XL_MIDI_MAP_SIZE]);
    void Begin                      (short voice, short mux_digital, short noise_digital, short lfo_control, short mod_mux_digital, short start_a_d);
    void MidiCommandConfiguration   (void);
    void Loop                       (void);
    void Clear                      (void);
    void Controller                 (short mchan, byte type, byte value);

    void PageAdvance                (void);
    void TemplateSelect             (byte index);
    void TemplateRefresh            (void)
        { LaunchControl.TemplateRefresh (); }

    //#######################################################################
    // FrontEndLFO.cpp
    void UpdateLfoDisplay           (void);
    void UpdateModButtons           (void);
    void SelectModVCA               (byte ch);
    void SelectModVCA               (byte ch, bool state);
    void FreqLFO                    (short ch, short data);
    void PitchBend                  (byte mchan, short value);
    void SelectModVCO               (short index, short ch);
    void SelectModVCO               (short index, short ch, bool state);
    void ToggleModRampDir           (short index);
    void SetModRampDir              (short index, bool state);
    void ToggleModLevelAlt          (short index);
    void SetModLevelAlt             (short index, bool state);
    void SetLevelLFO                (short index, byte mchan, short data)
        { Lfo[index].SetLevelMidi (mchan, data); }

    //#######################################################################
    // FrontEndMapping.cpp
    void MidiMapMode                (void);
    bool GetLoadSaveMode            (void)
        { return (LoadSaveMode); }
    bool GetMidiMapMode             (void)
        { return (MapSelectMode); }
    void MapModeBump                (short down);
    void ChangeMapSelect            (short right);
    bool GetMapSelect               (void)
        { return (MapSelectMode); }
    void ResolveMapAllocation       (void);
    void SaveDefaultConfig          (void);
    void LoadDefaultConfig          (void);
    void LoadSelectedConfig         (void);
    void SaveSelectedConfig         (void);
    void LoadSaveBump               (short down);
    void OpenLoadSavePage           (void);

    //#######################################################################
    // FrontEndOscCtrl.cpp
    void UpdateOscDisplay           (void);
    void UpdateOscButtons           (void);
    void UpdateFltDisplay           (void);
    void UpdateFltButtons           (void);
    void VoiceLevelSelect           (short ch, bool state);
    void SetLevel                   (short ch, short data);
    void SetAttackTime              (short ch, short data);
    void SetDecayTime               (short ch, short data);
    void SetReleaseTime             (short ch, short data);
    void SetSustainLevel            (short ch, short data);
    void ToggleRampDirection        (short ch);
    void SetPulseWidth              (short data);
    void FltStart                   (short ch, short data);
    void FltEnd                     (short ch, short data);
    void SelectFilter               (short index);
    void FreqCtrlModeAdv            (short index);

    //#######################################################################
    // FrontEndTuning.cpp
    void Tuning                     (void);
    void TuningAdjust               (bool up);
    void SetTuningLevel             (short ch, short data);
    void SetTuningFilter            (short ch, short data);
    void TuningBump                 (bool state);
    void StartTuning                (void);
    void SaveTuning                 (void);
    void Calibration                (ushort val);
    void StartCalibration           (void);

    inline void TuningOutputBitFlip (int bit)
        { TuningOutputSelect ^= 1 << bit; TuningChange = true; }
    inline bool IsInTuning (void)
        { return (SetTuning); }

    //#######################################################################
    inline void KeyDown (byte mchan, byte key, byte velocity)
        {
        Down.Key      = key;
        Down.Velocity = velocity;
        Down.Trigger  = mchan;
        }

    //#######################################################################
    inline void KeyUp (byte mchan, byte key, byte velocity)
        {
        Up.Key      = key;
        Up.Velocity = velocity;
        Up.Trigger  = mchan;
        }
    };

//#################################################

extern SYNTH_FRONT_C SynthFront;        // Synth front end class


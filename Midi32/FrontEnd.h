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
    void Initialize                 (void)
        { this->LaunchControl.Begin (pMidiMapXL);  this->LoadDefaultConfig (); }
    void Loop                       (void);
    void Clear                      (void);
    void Controller                 (short mchan, byte type, byte value);

    void PageAdvance                (void);
    void PageSelectedCheck          (void);
    void TemplateSelect             (byte index);

    //#######################################################################
    // FrontEndLFO.cpp
    void SelectModVCA               (byte ch, bool state);
    void FreqLFO                    (short ch, short data);
    void PitchBend                  (byte mchan, short value);
    void SetLevelLFO                (short index, byte mchan, short data)
        { this->Lfo[index].SetLevelMidi (mchan, data); }
    void SelectModVCO               (short index, short ch, bool state)
        { if ( !this->MapSelectMode )   this->Lfo[index].SetWave (ch, state);  this->SynthConfig.SetSelect (index, ch, state); }
    void SetModRampDir              (short index, bool state)
        { if ( !this->MapSelectMode )   this->Lfo[index].SetRampDir (state);  this->SynthConfig.SetRampDir (index, state); }

    //#######################################################################
    // FrontEndMapping.cpp
    void MidiMapMode                (void);
    bool GetLoadSaveMode            (void)
        { return (LoadSaveMode); }
    bool GetMidiMapMode             (void)
        { return (this->MapSelectMode); }
    void MapModeBump                (short down);
    void ChangeMapSelect            (short right);
    bool GetMapSelect               (void)
        { return (this->MapSelectMode); }
    void ResolveMapAllocation       (void);
    void SaveDefaultConfig          (void);
    void LoadDefaultConfig          (void);
    void LoadSelectedConfig         (void);
    void SaveSelectedConfig         (void);
    void LoadSaveBump               (short down);
    void OpenLoadSavePage           (void);

    //#######################################################################
    // FrontEndOscCtrl.cpp
    void VoiceLevelSelect               (short ch, bool state);
    void SetLevel                       (short ch, short data);
    void SetAttackTime                  (short ch, short data);
    void SetDecayTime                   (short ch, short data);
    void SetReleaseTime                 (short ch, short data);
    void SetSustainLevel                (short ch, short data);
    void ToggleRampDirection            (short ch);
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
    inline bool IsInTuning (void)
        { return (this->SetTuning); }

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


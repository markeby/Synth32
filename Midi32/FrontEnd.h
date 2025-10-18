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
#include "Novation.h"

//#################################################
//    Synthesizer front end class
//#################################################
struct KEY_S
    {
    byte    Key;            // Key that is hit when Trigger occurs.
    byte    Velocity;       // Velocity of key that triggers this.
    byte    Trigger;        // not zero triggers with the MIDI channel that triggers it.

    KEY_S () : Key(0), Velocity (0), Trigger (0) {}
    };

typedef struct KEY_S KEY_T;

//#######################################################################
extern KEY_T                 Down;
extern KEY_T                 Up;

extern ENV_GENERATOR_C    EnvADSL;
extern SYNTH_LFO_C             Lfo[2];
extern VOICE_C*                pVoice[VOICE_COUNT];
extern bool                    VoiceMute[MAP_COUNT]; ;

extern short                   VolumeMaster;
extern short                   VolumeOscMaster;
extern short                   VolumeFltMaster;
extern short                   VolumeSprMaster;

extern bool                    ResolutionMode;
extern bool                    MapSelectMode;
extern bool                    LoadSaveMode;
extern SYNTH_CONFIG_C          SynthConfig;
extern bool                    KaptureMidiKeyboard;
extern short                   CurrentMidiSelected;
extern short                   CurrentMapSelected;
extern short                   CurrentVoiceSelected;
extern short                   CurrentFilterSelected;
extern short                   CurrentConfigSelected;
extern short                   CurrentDisplayPage;

extern short                   LoadSaveSelection;
extern NOVATION_XL_C           LaunchControl;

extern bool                    SetTuning;
extern uint16_t                TuningLevel[ENVELOPE_COUNT+1];
extern uint16_t                TuningFlt[FILTER_DEVICES];
extern byte                    TuningOutputSelect;
extern bool                    TuningChange;
extern uint64_t                TuningSelectionTime;
extern ushort                  CalibrationReference;
extern short                   CalibrationBaseDigital;
extern short                   CalibrationPhase;
extern short                   CalibrationLFO;
extern ushort                  CalibrationAtoD;



void   UpdateMapModeDisplay     (int sel);

//#######################################################################
// FrontEnd.cpp
void InitializeSynth            (short voice, short mixer, short noise_digital, short lfo_control, short mod_mux_digital, short start_a_d);
void SystemExDebug              (byte* array, unsigned size);
void LoopSynth                  (void);

inline void KeyDown (byte mchan, byte key, byte velocity)
    { Down.Key = key; Down.Velocity = velocity; Down.Trigger = mchan; }
inline void KeyUp (byte mchan, byte key, byte velocity)
    { Up.Key = key; Up.Velocity = velocity; Up.Trigger = mchan; }

void MasterVolume               (short md);
void ClearSynth                 (void);
void ControllerNovation         (short mchan, byte type, byte value);

void PageAdvance                (void);
void TemplateSelect             (byte index);

inline void TemplateRefresh (void)
    { LaunchControl.TemplateRefresh (); }
inline void KeyboardKapture (bool state)
    { KaptureMidiKeyboard = state; }
inline bool KeyboardKapture (void)
    { return (KaptureMidiKeyboard); }

//#######################################################################
// FrontEnd_Control.cpp
void InitMidiControl            (void);

//#######################################################################
// FrontEnd_Seq.cpp
void InitMidiKeyboard           (void);
void ControlChangeKeyboard      (short mchan, byte type, byte value);

//#######################################################################
// FrontEnd_Seq.cpp
void InitMidiSequence           (void);
void ControllerSequence         (short mchan, byte type, byte value);

//#######################################################################
// FrontEnd_LFO.cpp
void UpdateLfoDisplay           (void);
void UpdateModButtons           (void);
void SelectModVCA               (byte ch);
void SelectModVCA               (byte ch, bool state);
void FreqLFO                    (short ch, short data);
void PitchBender                (byte mchan, short value);
void SelectModVCO               (short index, short ch);
void SelectModVCO               (short index, short ch, bool state);
void ToggleModRampDir           (short index);
void SetModRampDir              (short index, bool state);
void ToggleModLevelAlt          (short index);
void SetModLevelAlt             (short index, bool state);
inline void SetLevelLFO                (short index, byte mchan, short data)
    { Lfo[index].SetLevelMidi (mchan, data); }

//#######################################################################
// FrontEnd_Mapping.cpp
void MidiMapMode                (void);
inline bool GetLoadSaveMode (void)
    { return (LoadSaveMode); }
inline bool GetMidiMapMode (void)
    { return (MapSelectMode); }
inline bool GetMapSelect (void)
    { return (MapSelectMode); }
inline byte CurrentMidi (void)
    { return (CurrentMidiSelected & 0xFF); }

void MapModeBump                (short down);
void ChangeMapSelect            (short right);
void ResolveMapAllocation       (void);
void SaveDefaultConfig          (void);
void LoadDefaultConfig          (void);
void LoadSelectedConfig         (void);
void SaveSelectedConfig         (void);
void LoadSaveBump               (short down);
void OpenLoadSavePage           (void);

//#######################################################################
// FrontEnd_Voice.cpp
void VoiceDamperToggle          (short ch);
void MuteVoicesReset            (void);
void MuteVoiceToggle            (void);
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
// FrontEnd_Tuning.cpp
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


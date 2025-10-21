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
extern KEY_T            Down;
extern KEY_T            Up;

extern ENV_GENERATOR_C  EnvADSL;                    // Envelope generator creation class
extern SYNTH_LFO_C      Lfo[2];                     // Array of LFO interfaces
extern VOICE_C*         VoiceArray[VOICE_COUNT];    // Pointers to each of the voice module devices

extern bool             ResolutionMode;
extern bool             MapSelectMode;
extern bool             LoadSaveMode;
extern SYNTH_CONFIG_C   SynthConfig;
extern byte             SelectedMidi;
extern short            SelectedMap;
extern short            SelectedVoice;
extern short            SelectedFilter;
extern short            SelectedConfig;
extern short            CurrentDisplayPage;         // Current index of page on display

extern NOVATION_XL_C    LaunchControl;              // Novation LaunchControl XL control and interface module
                                                    //   (incomming commands on Midi_0)
extern bool             SetTuning;                  // Tuning mode is activated and no other mode can be used.
extern short            CalibrationBaseDigital;     // I2c base address for the digital controlled analog switches.
extern ushort           CalibrationAtoD;            // I2c interface index to the analog to digital interface.

//#######################################################################
inline void KeyDown (byte mchan, byte key, byte velocity)
    { Down.Key = key; Down.Velocity = velocity; Down.Trigger = mchan; }

inline void KeyUp (byte mchan, byte key, byte velocity)
    { Up.Key = key; Up.Velocity = velocity; Up.Trigger = mchan; }

//#######################################################################
// FrontEnd.cpp
void InitializeSynth            (short voice, short mixer, short noise_digital, short lfo_control, short mod_mux_digital, short start_a_d);
void SystemExDebug              (byte* array, unsigned size);
void LoopSynth                  (void);
void MasterVolume               (short md);
void ClearSynth                 (void);
void PageAdvance                (void);

//#######################################################################
// MIDI_Control.cpp
void InitMidiControl            (void);

//#######################################################################
// MIDI_Keyboard.cpp
void InitMidiKeyboard           (void);

//#######################################################################
// MIDI_Sequencer.cpp
void InitMidiSequence           (void);

//#######################################################################
// Control_LFO.cpp
void UpdateLfoDisplay           (void);
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

//#######################################################################
// Mapping.cpp
void MidiMapMode                (void);
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
// Control_Voice.cpp
void VoiceDamperToggle          (short ch);
void MuteVoicesReset            (void);
void MuteVoiceToggle            (void);
void StartOscDisplay           (void);
void StartFltDisplay           (void);
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
// Tuning.cpp
void Tuning                     (void);
void StartTuning                (void);
void TuningAdjust               (bool up);
void SetTuningLevel             (short ch, short data);
void SetTuningFilter            (short ch, short data);
void TuningBump                 (bool state);
void SaveTuning                 (void);
void StartCalibration           (void);
void TuningOutputBitFlip        (int bit);


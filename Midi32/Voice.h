//#######################################################################
// Module:     Voice.h
// Descrption: Synth channel instance class
// Creator:    markeby
// Date:       8/1/2022
//#######################################################################
#pragma once
#include "Osc.h"
#include "Filter4.h"

#define SELECT_OSC  0
#define SELECT_FTL  1

//#################################################
//    Synthesizer channel complete voice instance
//#################################################
class VOICE_C
    {
private:
    byte    Key;
    int32_t ActiveTimer;
    int     Number;
    byte    Midi;
    byte    UseCount;

    OSC_C   Osc;                    // oscillator class
    bool    RampDirection;
    float   PulseWidthSet;
    short   DigitalOutOscillator;

    FLT4_C  Flt;

    short   ModMux[NUM_MOD_MUX_IN];

    byte    NoiseSource;
    short   NoiseDigitalIO;

    byte    TuningOn;

public:
            VOICE_C             (short num, short osc_d_a, short mixer, short mod_mux_digital, short noise_digitinal, ENV_GENERATOR_C& envgen);
    void    Begin               (void);
    void    Loop                (void);
    void    NoteSet             (byte mchan, byte key, byte velocity);
    bool    NoteClear           (byte mchan, byte key);
    void    SetModMux           (byte select);
    void    NoiseReset          (void);
    void    NoiseSelect         (byte color);
    void    TuningAdjust        (bool up);

    //#######################################################################
    void     Clear              (void)                              { Osc.Clear (); }
    uint32_t IsActive           (void)                              { return (ActiveTimer); }
    void     SetMidi            (byte mchan)                        { Midi = mchan; }
    byte     GetMidi            (void)                              { return (Midi); }
    void     Mute               (bool state)                        { Osc.Mute (state); }
    void     SetSoftLFO         (byte fn, bool sel)                 { Osc.SetSoftLFO (fn, sel); }

    void     SetDamperMode      (byte fn, bool sel)                 { Osc.SetDamperMode (fn, sel); }
    void     SetOscAttackTime   (byte fn, float time)               { Osc.SetAttackTime (fn, time); }
    float    GetOscAttackTime   (byte fn)                           { return (Osc.GetAttackTime   (fn)); }
    void     SetOscDecayTime    (byte fn, float time)               { Osc.SetDecayTime (fn, time); }
    float    GetOscDecayTime    (byte fn)                           { return (Osc.GetDecayTime    (fn)); }
    void     SetOscReleaseTime  (byte fn, float time)               { Osc.SetReleaseTime  (fn, time); }
    float    GetOscReleaseTime  (byte fn)                           { return (Osc.GetReleaseTime  (fn)); }
    void     SetOscSustainLevel (byte fn, float level_percent)      { Osc.SetSustainLevel (fn, level_percent); }
    float    GetOscSustainLevel (byte fn)                           { return (Osc.GetSustainLevel (fn)); }
    void     SetMaxLevel        (byte fn, float level_percent)      { Osc.SetLevel (fn, level_percent); }
    float    GetMaxLevel        (byte fn)                           { return (Osc.GetLevel (fn)); }
    void     SetRampDirection   (bool data)                         { Osc.SetRampDirection (data);  RampDirection = data; }
    bool     GetRampDirection   (void)                              { return (RampDirection); }
    void     SetPulseWidth      (float percent)                     { Osc.PulseWidth (percent);  PulseWidthSet = percent; }
    float    GetPulseWidth      (void)                              { return (PulseWidthSet); }

    void     SetFltAttackTime   (byte fn, float time)               { Flt.SetAttackTime (fn, time); }
    float    GetFltAttackTime   (byte fn)                           { return (Flt.GetAttackTime (fn)); }
    void     SetFltDecayTime    (byte fn, float time)               { Flt.SetDecayTime (fn, time); }
    float    GetFltDecayTime    (byte fn)                           { return (Flt.GetDecayTime (fn)); }
    void     SetFltReleaseTime  (byte fn, float time)               { Flt.SetReleaseTime (fn, time); }
    float    GetFltReleaseTime  (byte fn)                           { return (Flt.GetReleaseTime (fn)); }
    void     SetFltSustain      (byte fn, float level_percent)      { Flt.SetSustainLevel (fn, level_percent); }
    float    GetFltSustain      (byte fn)                           { return (Flt.GetSustainLevel (fn)); }
    void     SetFltStart        (byte fn, float level_percent)      { Flt.SetStart (fn, level_percent); }
    float    GetFltStart        (byte fn)                           { return (Flt.GetStart (fn)); }
    void     SetFltEnd          (byte fn, float level_percent)      { Flt.SetEnd (fn, level_percent); }
    float    GetFltEnd          (byte fn)                           { return (Flt.GetEnd (fn)); }
    void     SetOutputMask      (byte bitmap);
    byte     GetOutputMask      (void)                              { return (Flt.GetOutMap ()); }
    void     SetFltCtrl         (byte fn, int value)                { Flt.SetCtrl (fn, value); }
    void     SetFltCtrl         (int value)                         { Flt.SetCtrl (0, value); Flt.SetCtrl (1, value); }
    int      GetFltCtrl         (byte fn)                           { return ((int)Flt.GetCtrl (fn)); }

    void     SetTuningNote      (byte note)                         { Osc.SetTuningNote   (note); }
    void     SetTuningVolume    (byte select, uint16_t level)       { Osc.SetTuningVolume (select, level); }
    void     SetTuningFlt       (byte fn, uint16_t level)           { Flt.SetTuning (fn, level); }
    bool     TuningState        (void)                              { return (TuningOn); }
    void     TuningState        (bool state)                        { TuningOn = state; SetTuningNote (Osc.LastNote ()); }
    ushort*  GetBankAddr        (void)                              { return (Osc.GetBankAddr ()); }
    short    LastDA             (void)                              { return (Osc.LastDA ()); }
    };


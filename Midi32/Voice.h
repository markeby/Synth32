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
    short   DigitalOutFilter;

    short   ModMux[NUM_MOD_MUX_IN];

    byte    NoiseSource;
    short   NoiseDigitalIO;

    byte    TuningOn;

public:
            VOICE_C             (short num, short osc_d_a, short mux_digital, short mod_mux_digital, short noise_digitinal, ENVELOPE_GENERATOR_C& envgen);
    void    Begin               (void);
    void    Loop                (void);
    void    NoteSet             (byte mchan, byte key, byte velocity);
    bool    NoteClear           (byte mchan, byte key);
    void    SetMux              (bool bypass);
    void    SetModMux           (byte select);
    void    NoiseReset          (void);
    void    NoiseSelect         (byte color);
    void    TuningAdjust        (bool up);

    //#######################################################################
    inline void     Clear               (void)                              { Osc.Clear (); }
    inline uint32_t IsActive            (void)                              { return (ActiveTimer); }
    inline void     SetMidi             (byte mchan)                        { Midi = mchan; }
    inline byte     GetMidi             (void)                              { return (Midi); }
    inline void     SetSoftLFO          (byte fn, bool sel)                 { Osc.SetSoftLFO (fn, sel); }

    inline void     SetOscAttackTime    (byte fn, float time)               { Osc.SetAttackTime (fn, time); }
    inline float    GetOscAttackTime    (byte fn)                           { return (Osc.GetAttackTime   (fn)); }
    inline void     SetOscDecayTime     (byte fn, float time)               { Osc.SetDecayTime (fn, time); }
    inline float    GetOscDecayTime     (byte fn)                           { return (Osc.GetDecayTime    (fn)); }
    inline void     SetOscReleaseTime   (byte fn, float time)               { Osc.SetReleaseTime  (fn, time); }
    inline float    GetOscReleaseTime   (byte fn)                           { return (Osc.GetReleaseTime  (fn)); }
    inline void     SetOscSustainLevel  (byte fn, float level_percent)      { Osc.SetSustainLevel (fn, level_percent); }
    inline float    GetOscSustainLevel  (byte fn)                           { return (Osc.GetSustainLevel (fn)); }
    inline void     SetMaxLevel         (byte fn, float level_percent)      { Osc.SetLevel (fn, level_percent); }
    inline float    GetMaxLevel         (byte fn)                           { return (Osc.GetLevel (fn)); }
    inline void     SetRampDirection    (bool data)                         { Osc.SetRampDirection (data);  RampDirection = data; }
    inline bool     GetRampDirection    (void)                              { return (RampDirection); }
    inline void     SetPulseWidth       (float percent)                     { Osc.PulseWidth (percent);  PulseWidthSet = percent; }
    inline float    GetPulseWidth       (void)                              { return (PulseWidthSet); }

    inline void     SetFltAttackTime    (byte fn, float time)               { Flt.SetAttackTime (fn, time); }
    inline float    GetFltAttackTime    (byte fn)                           { return (Flt.GetAttackTime (fn)); }
    inline void     SetFltDecayTime     (byte fn, float time)               { Flt.SetDecayTime (fn, time); }
    inline float    GetFltDecayTime     (byte fn)                           { return (Flt.GetDecayTime (fn)); }
    inline void     SetFltReleaseTime   (byte fn, float time)               { Flt.SetReleaseTime (fn, time); }
    inline float    GetFltReleaseTime   (byte fn)                           { return (Flt.GetReleaseTime (fn)); }
    inline void     SetFltSustain       (byte fn, float level_percent)      { Flt.SetSustainLevel (fn, level_percent); }
    inline float    GetFltSustain       (byte fn)                           { return (Flt.GetSustainLevel (fn)); }
    inline void     SetFltStart         (byte fn, float level_percent)      { Flt.SetStart (fn, level_percent); }
    inline float    GetFltStart         (byte fn)                           { return (Flt.GetStart (fn)); }
    inline void     SetFltEnd           (byte fn, float level_percent)      { Flt.SetEnd (fn, level_percent); }
    inline float    GetFltEnd           (byte fn)                           { return (Flt.GetEnd (fn)); }
    inline void     SetFltOut           (byte fmap)                         { Flt.SetOutMap (fmap); }
    inline byte     GetFltOut           (void)                              { return (Flt.GetOutMap ()); }
    inline void     SetFltCtrl          (byte fn, int value)                { Flt.SetCtrl (fn, value); }
    inline int      GetFltCtrl          (byte fn)                           { return ((int)Flt.GetCtrl (fn)); }

    inline void     SetTuningNote       (byte note)                         { Osc.SetTuningNote   (note); }
    inline void     SetTuningVolume     (byte select, uint16_t level)       { Osc.SetTuningVolume (select, level); }
    inline bool     TuningState         (void)                              { return (TuningOn); }
    inline void     TuningState         (bool state)                        { TuningOn = state; SetTuningNote (Osc.LastNote ()); }
    inline ushort*  GetBankAddr         (void)                              { return (Osc.GetBankAddr ()); }
    };


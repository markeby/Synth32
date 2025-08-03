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
    OSC_C*  pOsc;                   // oscillator class
    FLT4_C* pFlt;
    byte    Key;
    int32_t ActiveTimer;
    int     Number;
    byte    Midi;
    byte    UseCount;
    bool    RampDirection;
    float   PulseWidthSet;
    short   ModMux[NUM_MOD_MUX_IN];

    short   DigitalOutOscillator;
    short   DigitalOutFilter;

    byte    NoiseSource;
    short   NoiseDigitalIO;

    byte    TuningOn;

public:
            VOICE_C             (short num, short osc_d_a, short mux_digital, short mod_mux_digital, short noise_digitinal, ENVELOPE_GENERATOR_C& envgen);
    void    Begin               (void);
    void    Loop                (void);
    void    NoteSet             (byte mchan, byte key, byte velocity);
    bool    NoteClear           (byte mchan, byte key);
    void    SetMux              (byte select);
    void    SetModMux           (byte select);
    void    NoiseReset          (void);
    void    NoiseSelect         (byte color);
    void    TuningAdjust        (bool up);

    //#######################################################################
    inline void     Clear               (void)                              { this->pOsc->Clear (); }
    inline uint32_t IsActive            (void)                              { return (this->ActiveTimer); }
    inline void     SetMidi             (byte mchan)                        { this->Midi = mchan; }
    inline byte     GetMidi             (void)                              { return (this->Midi); }
    inline void     SetSoftLFO          (byte wave, bool sel)               { pOsc->SetSoftLFO (wave, sel); }

    inline void     SetOscAttackTime    (byte wave, float time)             { pOsc->SetAttackTime (wave, time); }
    inline float    GetOscAttackTime    (byte wave)                         { return (pOsc->GetAttackTime   (wave)); }
    inline void     SetOscDecayTime     (byte wave, float time)             { pOsc->SetDecayTime (wave, time); }
    inline float    GetOscDecayTime     (byte wave)                         { return (pOsc->GetDecayTime    (wave)); }
    inline void     SetOscReleaseTime   (byte wave, float time)             { pOsc->SetReleaseTime  (wave, time); }
    inline float    GetOscReleaseTime   (byte wave)                         { return (pOsc->GetReleaseTime  (wave)); }
    inline void     SetOscSustainLevel  (byte wave, float level_percent)    { pOsc->SetSustainLevel (wave, level_percent); }
    inline float    GetOscSustainLevel  (byte wave)                         { return (pOsc->GetSustainLevel (wave)); }
    inline void     SetMaxLevel         (byte wave, float level_percent)    { pOsc->SetLevel (wave, level_percent); }
    inline float    GetMaxLevel         (byte wave)                         { return (pOsc->GetLevel (wave)); }
    inline void     SetRampDirection    (bool data)                         { this->pOsc->SetRampDirection (data);  this->RampDirection = data; }
    inline bool     GetRampDirection    (void)                              { return (this->RampDirection); }
    inline void     SetPulseWidth       (float percent)                     { this->pOsc->PulseWidth (percent);  this->PulseWidthSet = percent; }
    inline float    GetPulseWidth       (void)                              { return (this->PulseWidthSet); }

    inline void     SetFltAttackTime    (byte fn, float time)               { pFlt->SetAttackTime (fn, time); }
    inline float    GetFltAttackTime    (byte fn)                           { return (pFlt->GetAttackTime (fn)); }
    inline void     SetFltDecayTime     (byte fn, float time)               { pFlt->SetDecayTime (fn, time); }
    inline float    GetFltDecayTime     (byte fn)                           { return (pFlt->GetDecayTime (fn)); }
    inline void     SetFltReleaseTime   (byte fn, float time)               { pFlt->SetReleaseTime (fn, time); }
    inline float    GetFltReleaseTime   (byte fn)                           { return (pFlt->GetReleaseTime (fn)); }
    inline void     SetFltSustain       (byte fn, float level_percent)      { pFlt->SetSustainLevel (fn, level_percent); }
    inline float    GetFltSustain       (byte fn)                           { return (pFlt->GetSustainLevel (fn)); }
    inline void     SetFltStart         (byte fn, float level_percent)      { pFlt->SetStart (fn, level_percent); }
    inline float    GetFltStart         (byte fn)                           { return (pFlt->GetStart (fn)); }
    inline void     SetFltEnd           (byte fn, float level_percent)      { pFlt->SetEnd (fn, level_percent); }
    inline float    GetFltEnd           (byte fn)                           { return (pFlt->GetEnd (fn)); }

    inline void     SetTuningNote       (byte note)                         { pOsc->SetTuningNote   (note, this->TuningOn); }
    inline void     SetTuningVolume     (byte select, uint16_t level)       { pOsc->SetTuningVolume (select, level); }
    inline bool     TuningState         (void)                              { return (TuningOn); }
    inline void     TuningState         (bool state)                        { TuningOn = state; this->SetTuningNote (pOsc->LastNote ()); }
    inline ushort*  GetBankAddr         (void)                              { return (pOsc->GetBankAddr ()); }
    };


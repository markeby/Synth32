//#######################################################################
// Module:     Voice.h
// Descrption: Synth channel instance class
// Creator:    markeby
// Date:       8/1/2022
//#######################################################################
#pragma once
#include "Osc.h"

#define SELECT_OSC  0
#define SELECT_FTL  1

//#################################################
//    Synthesizer channel complete voice instance
//#################################################
class VOICE_C
    {
private:
    OSC_C*  pOsc;                   // oscillator class
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

public:
             VOICE_C                (short num, short osc_d_a, short mux_digital, short mod_mux_digital, short noise_digitinal, ENVELOPE_GENERATOR_C& envgen);
    void     Begin                  (void);
    void     Loop                   (void);
    void     NoteSet                (byte mchan, byte key, byte velocity);
    bool     NoteClear              (byte mchan, byte key);
    void     SetMux                 (byte select);
    void     SetModMux              (byte select);
    void     NoiseReset             (void);
    void     NoiseSelect            (byte color);

    //#######################################################################
    inline void     Clear               (void)                              { this->pOsc->Clear (); }
    inline void     SetRampDirection    (bool data)                         { this->pOsc->SetRampDirection (data);  this->RampDirection = data; }
    inline bool     GetRampDirection    (void)                              { return (this->RampDirection); }
    inline void     SetPulseWidth       (float percent)                     { this->pOsc->PulseWidth (percent);  this->PulseWidthSet = percent; }
    inline float    GetPulseWidth       (void)                              { return (this->PulseWidthSet); }
    inline void     SetSoftLFO          (byte wave, bool sel)               { pOsc->SetSoftLFO      (wave, sel); }
    inline void     SetAttackTime       (byte wave, float time)             { pOsc->SetAttackTime   (wave, time); }
    inline void     SetDecayTime        (byte wave, float time)             { pOsc->SetDecayTime    (wave, time); }
    inline void     SetReleaseTime      (byte wave, float time)             { pOsc->SetReleaseTime  (wave, time); }
    inline void     SetSustainLevel     (byte wave, float level_percent)    { pOsc->SetSustainLevel (wave, level_percent); }
    inline void     SetMaxLevel         (byte wave, float level_percent)    { pOsc->SetMaxLevel     (wave, level_percent); }
    inline float    GetMaxLevel         (byte wave)                         { return (pOsc->GetMaxLevel     (wave)); }
    inline float    GetSustainLevel     (byte wave)                         { return (pOsc->GetSustainLevel (wave)); }
    inline float    GetAttackTime       (byte wave)                         { return (pOsc->GetAttackTime   (wave)); }
    inline float    GetDecayTime        (byte wave)                         { return (pOsc->GetDecayTime    (wave)); }
    inline float    GetReleaseTime      (byte wave)                         { return (pOsc->GetReleaseTime  (wave)); }
    inline void     SetTuningNote       (byte note)                         { pOsc->SetTuningNote (note); }
    inline void     SetTuningVolume     (byte select, uint16_t level)       { pOsc->SetTuningVolume (select, level); }
    inline void     TuningAdjust        (bool up)                           { pOsc->TuningAdjust (up); }
    inline ushort*  GetBankAddr         (void)                              { return (pOsc->GetBankAddr ()); }
    inline uint32_t IsActive            (void)                              { return (this->ActiveTimer); }
    inline void     SetMidi             (byte mchan)                        { this->Midi = mchan; }
    inline byte     GetMidi             (void)                              { return (this->Midi); }
    };


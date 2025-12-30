//#######################################################################
// Module:     Voice.h
// Descrption: Synth channel instance class
// Creator:    markeby
// Date:       8/1/2022
//#######################################################################
#pragma once
#include "config.h"
#include "Osc.h"
#include "Filter4.h"
#include "FilterLP.h"

#define SELECT_OSC  0
#define SELECT_FTL  1

//#################################################
//    Synthesizer channel complete voice instance
//#################################################
class VOICE_C
    {
private:
    byte    _Key;
    int32_t _ActiveTimer;
    int     _Number;
    byte    _Midi;
    byte    _UseCount;
    byte    _ChannelAfterTouch;     // 0 to 127 but does nothing now.

    OSC_C   _Osc;                    // oscillator class
    bool    _RampDirection;
    float   _PulseWidthSet;
    short   _DigitalOutOscillator;
    short   _DigitalOutLpFilter;

    FLT4_C  _Flt4;
    LPF_C   _FltLP;

    short   _ModMux[NUM_MOD_MUX_IN];

    byte    _NoiseSource;
    short   _NoiseDigitalIO;

    byte    _TuningOn;

public:
            VOICE_C              (short num, short osc_d_a, short mixer, short mod_mux_digital, short noise_digitinal, ENV_GENERATOR_C& envgen);
    void    Begin                (void);
    void    Loop                 (void);
    void    NoteSet              (byte mchan, byte key, byte velocity);
    bool    NoteClear            (byte mchan, byte key);
    void    SetModMux            (byte select);
    void    NoiseReset           (void);
    void    NoiseSelect          (byte color);
    void    TuningAdjust         (bool up);

    //#######################################################################
    void     Clear               (byte mchan)                       { if ( mchan == _Midi ) _Osc.Clear (); }
    uint32_t IsActive            (void)                             { return (_ActiveTimer); }
    void     SetMidi             (byte mchan)                       { _Midi = mchan; }
    byte     GetMidi             (void)                             { return (_Midi); }
    void     Mute                (bool state)                       { _Osc.Mute (state); }
    void     SetSoftLFOtoVCA     (byte fn, bool sel)                { _Osc.SetSoftLFO (fn, sel); }
    void     SetSoftLFOtoVCF     (bool sel)                         { _Flt4.SetSoftLFO (sel); }

    void     Expression          (float level)                      { _Osc.Expression (level); }
    void     Expression          (byte mchan, float level)          { if ( mchan == _Midi ) _Osc.Expression (level); }
    void     Damper              (byte mchan, bool state)           { if ( mchan == _Midi ) _Osc.Damper (state); }
    void     ChannelAfterTouch   (byte level)                       { _ChannelAfterTouch = level; }
    void     SetDamperMode       (byte fn, DAMPER mode)             { _Osc.SetDamperMode (fn, mode); }
    void     SetOscAttackTime    (byte fn, float time)              { _Osc.SetAttackTime (fn, time); }
    float    GetOscAttackTime    (byte fn)                          { return (_Osc.GetAttackTime   (fn)); }
    void     SetOscDecayTime     (byte fn, float time)              { _Osc.SetDecayTime (fn, time); }
    float    GetOscDecayTime     (byte fn)                          { return (_Osc.GetDecayTime    (fn)); }
    void     SetOscReleaseTime   (byte fn, float time)              { _Osc.SetReleaseTime  (fn, time); }
    float    GetOscReleaseTime   (byte fn)                          { return (_Osc.GetReleaseTime  (fn)); }
    void     SetOscSustainLevel  (byte fn, float level_percent)     { _Osc.SetSustainLevel (fn, level_percent); }
    float    GetOscSustainLevel  (byte fn)                          { return (_Osc.GetSustainLevel (fn)); }
    void     SetMaxLevel         (byte fn, float level_percent)     { _Osc.SetLevel (fn, level_percent); }
    float    GetMaxLevel         (byte fn)                          { return (_Osc.GetLevel (fn)); }
    void     SetRampDirection    (bool data)                        { _Osc.SetRampDirection (data);  _RampDirection = data; }
    bool     GetRampDirection    (void)                             { return (_RampDirection); }
    void     SetPulseWidth       (float percent)                    { _Osc.PulseWidth (percent);  _PulseWidthSet = percent; }
    float    GetPulseWidth       (void)                             { return (_PulseWidthSet); }

    void     SetOutputMask       (byte bitmap);
    byte     GetOutputMask       (void)                             { return (_Flt4.GetOutMap ()); }

    void     SetFlt4AttackTime   (float time)                       { _Flt4.SetAttackTime (time); }
    void     SetFlt4DecayTime    (float time)                       { _Flt4.SetDecayTime ( time); }
    void     SetFlt4ReleaseTime  (float time)                       { _Flt4.SetReleaseTime (time); }
    void     SetFlt4Sustain      (float level_percent)              { _Flt4.SetSustainLevel (level_percent); }
    void     SetFlt4Start        (float level_percent)              { _Flt4.SetStart (level_percent); }
    void     SetFlt4End          (float level_percent)              { _Flt4.SetEnd (level_percent); }
    void     SetFlt4Q            (float level_percent)              { _Flt4.SetQ (level_percent); }
    void     SetFlt4Ctrl         (int value)                        { _Flt4.SetCtrl (value); }

    void     SetFltLpAttackTime  (float time)                       { _FltLP.SetAttackTime (time); }
    void     SetFltLpDecayTime   (float time)                       { _FltLP.SetDecayTime ( time); }
    void     SetFltLpReleaseTime (float time)                       { _FltLP.SetReleaseTime (time); }
    void     SetFltLpSustain     (float level_percent)              { _FltLP.SetSustainLevel (level_percent); }
    void     SetFltLpStart       (float level_percent)              { _FltLP.SetStart (level_percent); }
    void     SetFltLpEnd         (float level_percent)              { _FltLP.SetEnd (level_percent); }
    void     SetFltLpQ           (float level_percent)              { _FltLP.SetQ (level_percent); }
    void     SetFltLpCtrl        (int value)                        { _FltLP.SetCtrl (value); }
    void     SetFltLpPole        (byte value)                       { _FltLP.SetPole (value); }
    void     SetFltLpModeQ       (byte value)                       { _FltLP.SetModeQ (value); }

    void     SetTuningNote       (byte note)                        { _Osc.SetTuningNote   (note); }
    void     SetTuningVolume     (byte select, uint16_t level)      { _Osc.SetTuningVolume (select, level); }
    void     SetTuningFlt        (bool qsel, uint16_t level)        { _Flt4.SetTuning(qsel, level); _FltLP.SetTuning(qsel, level); }
    bool     TuningState         (void)                             { return (_TuningOn); }
    void     TuningState         (bool state)                       { _TuningOn = state; SetTuningNote (_Osc.LastNote ()); }
    short    LastDA              (void)                             { return (_Osc.LastDA ()); }
    };


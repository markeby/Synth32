//#######################################################################
// Module:     Filter4.h
// Descrption: Controls for 3 pole variable state filter with 4 outputs
// Creator:    markeby
// Date:       7/26/2025
//#######################################################################
#pragma once
#include "Config.h"
#include "Envelope.h"

//#######################################################################
class   LPF_C
    {
private:
    ENVELOPE_C*         _Envelope;
    ENV_CTRL_E          _ControlSrc;
    bool                _Valid;          // Completed init and good for use
    byte                _Number;
    short               _FreqIO;
    short               _QuIO;
    short               _TuningQ;
    float               _Q;
    short               _QcompMuxIO1;
    short               _QcompMuxIO2;
    short               _StageIO;
    byte                _CurrentStage;
    byte                _CurrentModeQ;

    void    ClearState  (void);

public:
                LPF_C           (void);
    void        Begin           (short num, short first_device, byte& usecount);
    void        Clear           (void);
    void        NoteSet         (byte key, byte velocity);
    void        NoteClear       (void);

    void        SetSoftLFO      (bool sel)                  { _Envelope->SetSoftLFO (sel); }
    void        SetAttackTime   (float time)                { _Envelope->SetTime  (ESTATE::ATTACK, time); }
    float       GetAttackTime   (void)                      { return (_Envelope->GetTime  (ESTATE::ATTACK)); }
    void        SetDecayTime    (float time)                { _Envelope->SetTime  (ESTATE::DECAY, time); }
    float       GetDecayTime    (void)                      { return (_Envelope->GetTime  (ESTATE::DECAY)); }
    void        SetReleaseTime  (float time)                { _Envelope->SetTime  (ESTATE::RELEASE, time); }
    float       GetReleaseTime  (void)                      { return (_Envelope->GetTime  (ESTATE::RELEASE)); }
    void        SetSustainLevel (float level_percent)       { _Envelope->SetLevel (ESTATE::SUSTAIN, level_percent); }
    float       GetSustainLevel (void)                      { return (_Envelope->GetLevel (ESTATE::SUSTAIN)); }
    void        SetCtrl         (byte value)                { _ControlSrc = (ENV_CTRL_E)value; }
    void        SetStart        (float level_percent);
    float       GetStart        (void)                      { return (_Envelope->GetLevel (ESTATE::START)); }
    void        SetEnd          (float level_percent)       { _Envelope->SetLevel (ESTATE::ATTACK, level_percent); }
    float       GetEnd          (void)                      { return (_Envelope->GetLevel (ESTATE::ATTACK)); }
    void        SetQ            (float level_percent);
    float       GetQ            (void)                      { return (_Q); }
    void        SetPole         (byte value);
    void        SetModeQ        (byte value);
    void        SetTuning       (bool qsel, uint16_t level);
    };


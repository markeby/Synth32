//#######################################################################
// Module:     Filter4.h
// Descrption: Controls for 3 pole variable state filter with 4 outputs
// Creator:    markeby
// Date:       7/26/2025
//#######################################################################
#pragma once
#include "Config.h"
#include "Envelope.h"

class   FLT4_C
    {
private:
    ENVELOPE_C*         _Envelope;
    ENV_CTRL_E          _ControlSrc;
    bool                _Valid;          // Completed init and good for use
    byte                _Number;
    short               _FreqIO;
    short               _QuIO;
    float               _Q;
    short               _OutSwitch[FILTER_OUTPUTS];
    byte                _OutMap;

    void    ClearState  (void);

public:
                    FLT4_C              (void);
    void            Begin               (short num, short first_device, byte& usecount);
    void            Clear               (void);
    void            NoteSet             (byte key, byte velocity);
    void            NoteClear           (void);

    void            SetSoftLFO          (bool sel)                 { _Envelope->SetSoftLFO (sel); }
    void            SetAttackTime       (float time)               { _Envelope->SetTime  (ESTATE::ATTACK, time); }
    float           GetAttackTime       (void)                     { return (_Envelope->GetTime  (ESTATE::ATTACK)); }
    void            SetDecayTime        (float time)               { _Envelope->SetTime  (ESTATE::DECAY, time); }
    float           GetDecayTime        (void)                     { return (_Envelope->GetTime  (ESTATE::DECAY)); }
    void            SetReleaseTime      (float time)               { _Envelope->SetTime  (ESTATE::RELEASE, time); }
    float           GetReleaseTime      (void)                     { return (_Envelope->GetTime  (ESTATE::RELEASE)); }
    void            SetSustainLevel     (float level_percent)      { _Envelope->SetLevel (ESTATE::SUSTAIN, level_percent); }
    float           GetSustainLevel     (void)                     { return (_Envelope->GetLevel (ESTATE::SUSTAIN)); }
    void            SetStart            (float level_percent);
    float           GetStart            (void)                     { return (_Envelope->GetLevel (ESTATE::START)); }
    void            SetEnd              (float level_percent)      { _Envelope->SetLevel (ESTATE::ATTACK, level_percent); }
    float           GetEnd              (void)                     { return (_Envelope->GetLevel (ESTATE::ATTACK)); }
    void            SetQ                (float level_percent);
    float           GetQ                (void)                     { return (_Q); }
    void            SetCtrl             (byte value)               { _ControlSrc = (ENV_CTRL_E)value; }
    ENV_CTRL_E      GetCtrl             (void)                     { return (_ControlSrc); }
    void            SetOutMap           (byte fmap);
    byte            GetOutMap           (void)                     { return (_OutMap << 1); }
    void            SetTuning           (bool qsel, uint16_t level);
    };


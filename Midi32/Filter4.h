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
class   FLT4_C
    {
private:
    ENVELOPE_GENERATOR_C&   EnvGen;
    ENVELOPE_C*             Funct[2];

    bool                    Valid;          // Completed init and good for use
    byte                    Number;
    short                   FreqIO;
    short                   QuIO;
    short                   OutSwitch[4];

    void    ClearState      (void);

public:
            FLT4_C          (short num, short first_device, byte& usecount, ENVELOPE_GENERATOR_C& envgen);
    void    Clear           (void);

    inline void     SetAttackTime       (byte fn, float time)             { this->Funct[fn]->SetTime  (ESTATE::ATTACK, time); }
    inline float    GetAttackTime       (byte fn)                         { return (this->Funct[fn]->GetTime  (ESTATE::ATTACK)); }
    inline void     SetDecayTime        (byte fn, float time)             { this->Funct[fn]->SetTime  (ESTATE::DECAY, time); }
    inline float    GetDecayTime        (byte fn)                         { return (this->Funct[fn]->GetTime  (ESTATE::DECAY)); }
    inline void     SetReleaseTime      (byte fn, float time)             { this->Funct[fn]->SetTime  (ESTATE::RELEASE, time); }
    inline float    GetReleaseTime      (byte fn)                         { return (this->Funct[fn]->GetTime  (ESTATE::RELEASE)); }
    inline void     SetSustainLevel     (byte fn, float level_percent)    { this->Funct[fn]->SetLevel (ESTATE::SUSTAIN, level_percent); }
    inline float    GetSustainLevel     (byte fn)                         { return (this->Funct[fn]->GetLevel (ESTATE::SUSTAIN)); }
    inline void     SetStart            (byte fn, float level_percent)    { this->Funct[fn]->SetLevel (ESTATE::START, level_percent); }
    inline float    GetStart            (byte fn)                         { return (this->Funct[fn]->GetLevel (ESTATE::START)); }
    inline void     SetEnd              (byte fn, float level_percent)    { this->Funct[fn]->SetLevel (ESTATE::ATTACK, level_percent); }
    inline float    GetEnd              (byte fn)                         { return (this->Funct[fn]->GetLevel (ESTATE::ATTACK)); }
    };



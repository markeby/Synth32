//#######################################################################
// Module:     Filter4.h
// Descrption: Controls for 3 pole variable state filter with 4 outputs
// Creator:    markeby
// Date:       7/26/2025
//#######################################################################
#pragma once
#include "Config.h"
#include "Envelope.h"

//###########################################
// Envelope selection bytes
//###########################################
enum class FILTER_CTRL_C: int
    {
    FIXED = 0,
    ENVELOPE = 1,
    MODULATE = 2,
    MODWHEEL = 3
    };

//#######################################################################
class   FLT4_C
    {
private:
    ENVELOPE_C*             Funct[FILTER_DEVICES];
    FILTER_CTRL_C           ControlSrc[FILTER_DEVICES];

    bool                    Valid;          // Completed init and good for use
    byte                    Number;
    short                   FreqIO;
    short                   QuIO;
    short                   OutSwitch[FILTER_OUTPUTS];
    byte                    OutMap;

    void    ClearState      (void);

public:
                    FLT4_C              (void);
    void            Begin               (short num, short first_device, byte& usecount, ENVELOPE_GENERATOR_C& envgen);
    void            Clear               (void);
    void            NoteSet             (byte key, byte velocity);
    void            NoteClear           (void);

    void            SetAttackTime       (byte fn, float time)               { Funct[fn]->SetTime  (ESTATE::ATTACK, time); }
    float           GetAttackTime       (byte fn)                           { return (Funct[fn]->GetTime  (ESTATE::ATTACK)); }
    void            SetDecayTime        (byte fn, float time)               { Funct[fn]->SetTime  (ESTATE::DECAY, time); }
    float           GetDecayTime        (byte fn)                           { return (Funct[fn]->GetTime  (ESTATE::DECAY)); }
    void            SetReleaseTime      (byte fn, float time)               { Funct[fn]->SetTime  (ESTATE::RELEASE, time); }
    float           GetReleaseTime      (byte fn)                           { return (Funct[fn]->GetTime  (ESTATE::RELEASE)); }
    void            SetSustainLevel     (byte fn, float level_percent)      { Funct[fn]->SetLevel (ESTATE::SUSTAIN, level_percent); }
    float           GetSustainLevel     (byte fn)                           { return (Funct[fn]->GetLevel (ESTATE::SUSTAIN)); }
    void            SetStart            (byte fn, float level_percent);
    float           GetStart            (byte fn)                           { return (Funct[fn]->GetLevel (ESTATE::START)); }
    void            SetEnd              (byte fn, float level_percent)      { Funct[fn]->SetLevel (ESTATE::ATTACK, level_percent); }
    float           GetEnd              (byte fn)                           { return (Funct[fn]->GetLevel (ESTATE::ATTACK)); }
    void            SetCtrl             (byte fn, byte value)               { ControlSrc[fn] = (FILTER_CTRL_C)value; }
    FILTER_CTRL_C   GetCtrl             (byte fn)                           { return (ControlSrc[fn]); }
    void            SetOutMap           (byte fmap);
    byte            GetOutMap           (void)                              { return (OutMap << 1); }
    void            SetTuning           (byte fn, uint32_t level)           { Funct[fn]->SetOverride (level); }
    };


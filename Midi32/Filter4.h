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
            FLT4_C          (void);
    void    Begin           (short num, short first_device, byte& usecount, ENVELOPE_GENERATOR_C& envgen);
    void    Clear           (void);
    void    NoteSet         (byte key, byte velocity);
    void    NoteClear       (void);

    inline void             SetAttackTime       (byte fn, float time)               { Funct[fn]->SetTime  (ESTATE::ATTACK, time); }
    inline float            GetAttackTime       (byte fn)                           { return (Funct[fn]->GetTime  (ESTATE::ATTACK)); }
    inline void             SetDecayTime        (byte fn, float time)               { Funct[fn]->SetTime  (ESTATE::DECAY, time); }
    inline float            GetDecayTime        (byte fn)                           { return (Funct[fn]->GetTime  (ESTATE::DECAY)); }
    inline void             SetReleaseTime      (byte fn, float time)               { Funct[fn]->SetTime  (ESTATE::RELEASE, time); }
    inline float            GetReleaseTime      (byte fn)                           { return (Funct[fn]->GetTime  (ESTATE::RELEASE)); }
    inline void             SetSustainLevel     (byte fn, float level_percent)      { Funct[fn]->SetLevel (ESTATE::SUSTAIN, level_percent); }
    inline float            GetSustainLevel     (byte fn)                           { return (Funct[fn]->GetLevel (ESTATE::SUSTAIN)); }
           void             SetStart            (byte fn, float level_percent);
    inline float            GetStart            (byte fn)                           { return (Funct[fn]->GetLevel (ESTATE::START)); }
    inline void             SetEnd              (byte fn, float level_percent)      { Funct[fn]->SetLevel (ESTATE::ATTACK, level_percent); }
    inline float            GetEnd              (byte fn)                           { return (Funct[fn]->GetLevel (ESTATE::ATTACK)); }
    inline void             SetCtrl             (byte fn, byte value)               { ControlSrc[fn] = (FILTER_CTRL_C)value; }
    inline FILTER_CTRL_C    GetCtrl             (byte fn)                           { return (ControlSrc[fn]); }
    void                    SetOutMap           (byte fmap);
    inline byte             GetOutMap           (void)                              { return (OutMap << 1); }
    };



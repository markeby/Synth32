//#######################################################################
// Module:     Osc.h
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       3/23/2023
//#######################################################################
#pragma once
#include "Config.h"
#include "Envelope.h"

namespace OSC_N
{
//#######################################################################
enum class SHAPE
    {
    SINE = 0,
    TRIANGLE,
    RAMP,
    PULSE,
    NOISE,
    ALL
    };

enum class D_A_OFF
    {
    EXPO = 0,
    WIDTH,
    TRIANGLE,
    RAMP,
    PULSE,
    NOISE,
    SINE,
    DIR
    };

#define TUNING_WAVES_SHAPE      (SHAPE::RAMP)

}// end namespace OSC_N

//#######################################################################
class   OSC_C
    {
private:
    ENVELOPE_C*             Mix[OSC_MIXER_COUNT];
    uint16_t*               _OctaveArray;

    bool                    Valid;          // Completed init and good for use
    byte                    Number;
    byte                    CurrentNote;
    short                   CurrentDA;
    short                   OscPortIO;
    short                   PwmPortIO;
    short                   RampDirPortIO;

    void  ClearState (void);

public:
            OSC_C               (void);
    void    Begin               (short num, short first_device, byte& usecount);
    void    SetTuningVolume     (byte select, uint16_t level);
    void    SetTuningNote       (byte note);
    void    TuningAdjust        (bool up);
    void    Mute                (bool state);
    void    NoteSet             (byte key, byte velocity);
    void    NoteClear           (void);
    void    Clear               (void);
    void    SetRampDirection    (bool data);
    void    PulseWidth          (float percent);

    //#######################################################################
    void    Expression          (float level);
    void    SetSoftLFO          (byte wave, bool sel)               { Mix[wave]->SetSoftLFO (sel); }
    void    SetDamperMode       (byte wave, DAMPER mode)            { Mix[wave]->SetDamperMode (mode); }
    void    Damper              (bool state);
    void    SetAttackTime       (byte wave, float time)             { Mix[wave]->SetTime  (ESTATE::ATTACK, time); }
    float   GetAttackTime       (byte wave)                         { return (Mix[wave]->GetTime  (ESTATE::ATTACK)); }
    void    SetDecayTime        (byte wave, float time)             { Mix[wave]->SetTime  (ESTATE::DECAY, time); }
    float   GetDecayTime        (byte wave)                         { return (Mix[wave]->GetTime  (ESTATE::DECAY)); }
    void    SetReleaseTime      (byte wave, float time)             { Mix[wave]->SetTime  (ESTATE::RELEASE, time); }
    float   GetReleaseTime      (byte wave)                         { return (Mix[wave]->GetTime  (ESTATE::RELEASE)); }
    void    SetSustainLevel     (byte wave, float level_percent)    { Mix[wave]->SetLevel (ESTATE::SUSTAIN, level_percent); }
    float   GetSustainLevel     (byte wave)                         { return (Mix[wave]->GetLevel (ESTATE::SUSTAIN)); }
    void    SetLevel            (byte wave, float level_percent)    { Mix[wave]->SetLevel (ESTATE::ATTACK, level_percent); }
    float   GetLevel            (byte wave)                         { return (Mix[wave]->GetLevel (ESTATE::ATTACK)); }
    byte    LastNote            (void)                              { return (CurrentNote); }
    short   LastDA              (void)                              { return (CurrentDA); }
    };



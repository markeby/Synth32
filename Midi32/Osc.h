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
    ENVELOPE_GENERATOR_C&   EnvGen;
    ENVELOPE_C*             Mix[OSC_MIXER_COUNT];
    uint16_t                OctaveArray[FULL_KEYS];

    bool                    Valid;          // Completed init and good for use
    byte                    Number;
    byte                    CurrentNote;
    short                   OscPortIO;
    short                   PwmPortIO;
    short                   RampDirPortIO;

    void  ClearState (void);

public:
                OSC_C              (short num, short first_device, byte& usecount, ENVELOPE_GENERATOR_C& envgen);
    void        SetTuningVolume    (byte select, uint16_t level);
    void        SetTuningNote      (byte note);
    void        NoteSet            (byte key, byte velocity);
    void        NoteClear          (void);
    void        Clear              (void);
    void        SetRampDirection   (bool data);
    void        PulseWidth         (float percent);
    void        TuningAdjust       (bool up);

    //#######################################################################
    inline void     SetSoftLFO      (byte wave, bool sel)               { Mix[wave]->SetSoftLFO (sel); }
    inline void     SetAttackTime   (byte wave, float time)             { Mix[wave]->SetTime  (ESTATE::ATTACK, time); }
    inline void     SetDecayTime    (byte wave, float time)             { Mix[wave]->SetTime  (ESTATE::DECAY, time); }
    inline void     SetReleaseTime  (byte wave, float time)             { Mix[wave]->SetTime  (ESTATE::RELEASE, time); }
    inline void     SetSustainLevel (byte wave, float level_percent)    { Mix[wave]->SetLevel (ESTATE::SUSTAIN, level_percent); }
    inline void     SetMaxLevel     (byte wave, float level_percent)    { Mix[wave]->SetLevel (ESTATE::ATTACK, level_percent); }
    inline float    GetMaxLevel     (byte wave)                         { return (this->Mix[wave]->GetLevel (ESTATE::ATTACK)); }
    inline float    GetSustainLevel (byte wave)                         { return (this->Mix[wave]->GetLevel (ESTATE::SUSTAIN)); }
    inline float    GetAttackTime   (byte wave)                         { return (this->Mix[wave]->GetTime  (ESTATE::ATTACK)); }
    inline float    GetDecayTime    (byte wave)                         { return (this->Mix[wave]->GetTime  (ESTATE::DECAY)); }
    inline float    GetReleaseTime  (byte wave)                         { return (this->Mix[wave]->GetTime  (ESTATE::RELEASE)); }
    inline ushort*  GetBankAddr     (void)                              { return (this->OctaveArray); }
    };



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
    SAWTOOTH,
    PULSE,
    SQUARE,
    ALL
    };

enum class D_A_OFF
    {
    DIR = 0,
    SINE,
    SQUARE,
    PULSE,
    SAWTOOTH,
    TRIANGLE,
    WIDTH,
    EXPO
    };

#define TUNING_WAVES_SHAPE      (SHAPE::SAWTOOTH)

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
    byte                    OscPortIO;
    byte                    PwmPortIO;
    byte                    RampDirPortIO;

    void  ClearState (void);

public:
                OSC_C              (byte num, byte first_device, byte& usecount, ENVELOPE_GENERATOR_C& envgen);
    void        SetTuningVolume    (byte select, uint16_t level);
    void        SetTuningNote      (byte note);
    void        NoteSet            (byte key, byte velocity);
    void        NoteClear          (void);
    void        Clear              (void);
    void        SawtoothDirection  (bool data);
    void        PulseWidth         (float percent);
    void        SetSoftLFO         (byte wave, bool sel);
    void        SetAttackTime      (byte wave, float time);
    void        SetDecayTime       (byte wave, float time);
    void        SetReleaseTime     (byte wave, float time);
    void        SetSustainLevel    (byte wave, float level_percent);
    void        SetMaxLevel        (byte wave, float level_percent);
    void        TuningAdjust       (bool up);


    //#######################################################################
    inline float     GetMaxLevel     (byte wave)       { return (this->Mix[wave]->GetLevel (ESTATE::ATTACK)); }
    inline float     GetSustainLevel (byte wave)       { return (this->Mix[wave]->GetLevel (ESTATE::SUSTAIN)); }
    inline float     GetAttackTime   (byte wave)       { return (this->Mix[wave]->GetTime (ESTATE::ATTACK)); }
    inline float     GetDecayTime    (byte wave)       { return (this->Mix[wave]->GetTime (ESTATE::DECAY)); }
    inline float     GetReleaseTime  (byte wave)       { return (this->Mix[wave]->GetTime (ESTATE::RELEASE)); }
    inline uint16_t* GetBankAddr     (void)            { return (this->OctaveArray); }
    };



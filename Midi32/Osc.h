//#######################################################################
// Module:     Osc.h
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       3/23/2023
//#######################################################################
#pragma once

#include "config.h"
#include "Envelope.h"

namespace OSC_N
{
//#######################################################################
enum class SHAPE {
    SINE = 0,
    TRIANGLE,
    SAWTOOTH,
    PULSE,
    SQUARE,
    };

enum class D_A_OFF {
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
class   SYNTH_OSC_C
    {
private:
    ENVELOPE_GENERATOR_C&   EnvGen;
    ENVELOPE_C*             Mix[OSC_MIXER_COUNT];
    uint16_t                OctaveArray[FULL_KEYS];

    bool                    Valid;          // Completed init and good for use
    byte                    Number;
    byte                    CurrentNote;
    byte                    OscChannel;
    byte                    PwmChannel;
    byte                    SawtoothDirChannel;

    void  ClearState (void);

public:
         SYNTH_OSC_C        (byte num, uint8_t first_device, byte& usecount, ENVELOPE_GENERATOR_C& envgen);
    void SetTuningVolume    (byte select, uint16_t level);
    void SetTuningNote      (uint8_t note);
    void NoteSet            (byte key, uint8_t velocity);
    void NoteClear          (void);
    void Clear              (void);
    void SetReverse         (bool data);
    void SetAttackTime      (byte wave, float time);
    void SetDecayTime       (byte wave, float time);
    void SetSustainTime     (byte wave, float time);
    void SetReleaseTime     (byte wave, float time);
    void SetSustainLevel    (byte wave, float level_percent);
    void SetMaxLevel        (byte wave, float level_percent);
    void TuningAdjust       (bool up);
    };



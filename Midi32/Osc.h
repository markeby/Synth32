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
    uint8_t                    Number;
    uint8_t                    CurrentNote;
    uint8_t                    OscChannel;
    uint8_t                    PwmChannel;
    uint8_t                    SawtoothDirChannel;

    void  ClearState (void);

public:
         SYNTH_OSC_C        (uint8_t num, uint8_t first_device, uint8_t& usecount, ENVELOPE_GENERATOR_C& envgen);
    void SetTuningVolume    (uint8_t select, uint16_t level);
    void SetTuningNote      (uint8_t note);
    void NoteSet            (uint8_t key, uint8_t velocity);
    void NoteClear          (void);
    void Clear              (void);
    void SawtoothDirection  (bool data);
    void SetAttackTime      (uint8_t wave, float time);
    void SetDecayTime       (uint8_t wave, float time);
    void SetSustainTime     (uint8_t wave, float time);
    void SetReleaseTime     (uint8_t wave, float time);
    void SetSustainLevel    (uint8_t wave, float level_percent);
    void SetMaxLevel        (uint8_t wave, float level_percent);
    void TuningAdjust       (bool up);
    };



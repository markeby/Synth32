//#######################################################################
// Module:     Noise.h
// Descrption: Load and save setting and message settings
// Creator:    markeby
// Date:       8/1/2022
//#######################################################################
#pragma once
#include "Envelope.h"

namespace NOISE_N
{
//#######################################################################
enum class SELECT {
    VCA = 0,
    VCF,
    };

}// end namespace NOISE_N

//#################################################
//    Synthesizer noise control
//#################################################
class SYNTH_NOISE_C
    {
private:
    ENVELOPE_GENERATOR_C&   EnvGen;
    byte                    Number;
    int                     Analog[FILTER_ANALOG_COUNT];
    int                     FilterDigital[2];
    int                     ColorControl;
    byte                    FilterSelected;
    ENVELOPE_C*             Envelope[FILTER_ANALOG_COUNT];
    byte                    CurrentNote;

    void    ClearState      (void);
public:
         SYNTH_NOISE_C      (byte num, int anaog, int digital, byte& usecount, ENVELOPE_GENERATOR_C& envgen);
    void SetTuningVolume    (byte select, float level);
    void Clear              (void);
    void FilterSelect       (byte select);
    void FilterCutoff       (float cutoff);
    void SetLevel           (uint16_t level);
    void SetAttackTime      (uint8_t sel, float time);
    void SetDecayTime       (uint8_t sel, float time);
    void SetReleaseTime     (uint8_t sel, float time);
    void SetSustainLevel    (uint8_t sel, float level_percent);
    void SetSustainTime     (uint8_t sel, float time);
    void SetBaseLevel       (NOISE_N::SELECT sel, float level_percent);
    void SetMaxLevel        (uint8_t sel, float level_percent);
    void NoteSet            (uint8_t note, uint8_t velocity);
    void NoteClear          (void);
    };


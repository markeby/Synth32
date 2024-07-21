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
    uint8_t                    Number;
    int                     Analog[FILTER_ANALOG_COUNT];
    int                     FilterDigital[2];
    int                     ColorControl;
    uint8_t                    FilterSelected;
    ENVELOPE_C*             Envelope[FILTER_ANALOG_COUNT];
    uint8_t                    CurrentNote;

    void    ClearState      (void);
public:
         SYNTH_NOISE_C      (uint8_t num, int anaog, int digital, uint8_t& usecount, ENVELOPE_GENERATOR_C& envgen);
    void SetTuningVolume    (uint8_t select, uint16_t level);
    void Clear              (void);
    void FilterSelect       (uint8_t select);
    void TuningFilterCut       (float cutoff);
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


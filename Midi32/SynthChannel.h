//#######################################################################
// Module:     SynthChannel.h
// Descrption: Load and save setting and message settings
// Creator:    markeby
// Date:       8/1/2022
//#######################################################################
#pragma once

#include "Osc.h"
#include "Noise.h"

//#################################################
//    Synthesizer channel audio class
//#################################################
class SYNTH_CHANNEL_C
    {
private:
    SYNTH_OSC_C*    OscP;                   // oscillator class
    SYNTH_NOISE_C*  NoiseP;
    uint8_t         Key;
    int32_t         ActiveTimer;
    int             Number;
    uint8_t         UseCount;

public:
             SYNTH_CHANNEL_C    (int num, int osc_d_a, int noise_d_a, int noise_dig, ENVELOPE_GENERATOR_C& envgen);
    void     Begin              (void);
    void     Loop               (void);
    void     NoteSet            (uint8_t key, uint8_t velocity);
    void     Clear              (void);
    bool     NoteClear          (uint8_t key);

    uint32_t       IsActive (void)      { return (this->ActiveTimer); }
    SYNTH_OSC_C*   pOsc     (void)      { return (this->OscP); }
    SYNTH_NOISE_C* pNoise   (void)      { return (this->NoiseP); }
    };


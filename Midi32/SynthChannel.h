//#######################################################################
// Module:     SynthChannel.h
// Descrption: Load and save setting and message settings
// Creator:    markeby
// Date:       8/1/2022
//#######################################################################
#pragma once

//#################################################
//    Synthesizer channel audio class
//#################################################
class SYNTH_CHANNEL_C
    {
private:
    SYNTH_OSC_C*    OscP;                   // oscillator class
    SYNTH_NOISE_C*  NoiseP;
    byte            Key;
    int32_t         ActiveTimer;
    int             Number;
    byte            UseCount;

public:
             SYNTH_CHANNEL_C    (int num, int osc_d_a, int noise_d_a, int noise_dig, ENVELOPE_GENERATOR_C& envgen);
    void     Begin              (void);
    void     Loop               (void);
    void     NoteSet            (byte key, byte velocity);
    void     Clear              (void);
    bool     NoteClear          (byte key);

    uint32_t       IsActive (void)      { return (ActiveTimer); }
    SYNTH_OSC_C*   pOsc     (void)      { return (OscP); }
    SYNTH_NOISE_C* pNoise   (void)      { return (NoiseP); }
    };


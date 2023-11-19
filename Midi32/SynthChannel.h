//#######################################################################
// Module:     Settings.h.hpp
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
    SYNTH_OSC_C     Osc;                   // oscillator class
    byte            Key;
    int32_t         ActiveTimer;
    int             FirstDevice;
    int             Number;

public:
             SYNTH_CHANNEL_C    (int num, int first_device);
    void     Begin              (void);
    void     Loop               (void);
    void     SetTuning          (void);
    uint32_t Active             (void);
    void     NoteSet            (byte key, byte velocity);
    void     Clear              (void);
    bool     NoteClear          (byte key);

    SYNTH_OSC_C* pOsc   (void)  { return (&Osc); }
    };


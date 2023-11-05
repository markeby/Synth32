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
    OSC_N::SYNTH_OSC_C  Osc;                   // oscillator uint64_t            ActiveTime;
    uint8_t             Key;
    int32_t             ActiveTimer;
    int                 FirstDevice;
    int                 Number;

public:
          SYNTH_CHANNEL_C   (int num, int first_device);
    void  Begin             (void);
    void  Loop              (void);

    void SetTuning          (void);
    uint32_t Active         (void);
    void NoteSet            (uint8_t key, uint8_t velocity);
    void Clear              (void);
    bool NoteClear          (uint8_t key);
    void SetMaxLevel        (int wave, float data);
    void SetAttack          (int wave, float data);
    void SetDecay           (int wave, float data);
    void SetSustainLevel    (int wave, float data);
    void SetSustainTime     (int wave, float data);
    void SetRelease         (int wave, float data);
    };


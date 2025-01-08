//#######################################################################
// Module:     LFOosc.h
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       9/18/2023
//#######################################################################
#pragma once

#include "config.h"


#define LFO_VCA_COUNT     3

//#######################################################################
enum class SHAPE {
    SINE = 0,
    SAWTOOTH,
    PULSE,
    };

enum class D_A_OFF {
    BEND = 0,
    WIDTH,
    FREQ,
    SAWTOOTH,
    PULSE,
    SINE
    };

#define MAX_FREQ_DEV    32

//#######################################################################
class   SYNTH_LFO_C
    {
private:
    bool    Valid;
    int     Active;
    int     Number;
    byte    OscChannelIO;
    byte    PwmChannelIO;
    byte    BendChannelIO;
    byte    SlopeChannelIO;
    byte    HardResetChannelIO;
    byte    InUse;
    bool    UpdateNeded;
    short   CurrentFreq;
    float   CurrentWidth;
    byte    CurrentLevel;
    bool    RampSlope;
    bool    ResetOn;

    typedef struct
        {
        uint8_t     Channel;            // I2C device index
        uint16_t    CurrentLevel;       // Current setting 12 bit D/A
        uint16_t    MaximumLevel;       // No higher than this
        bool        Select;
        String      Name;
        } VCA_T;

    VCA_T     Vca[OSC_MIXER_COUNT];

    void  ClearState (void);
    void  SetLevel   (uint8_t ch, uint8_t data);

public:
         SYNTH_LFO_C    (void);
    void Begin          (int num, uint8_t first_device, uint8_t lfo_digital);
    void Clear          (void);
    void SetFreq        (short value);
    void SetPulseWidth  (short value);
    void SetMaxLevel    (uint8_t ch, uint8_t data);
    void SetLevel       (uint8_t data);
    void ToggleWave     (short ch);
    void PitchBend      (short value);
    void ToggleRampDir  (void);
    void HardReset      (void);
    void Loop           (void);
    };



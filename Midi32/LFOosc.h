//#######################################################################
// Module:     LFOosc.h
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       9/18/2023
//#######################################################################
#pragma once

#include "config.h"


namespace LFO_N
{

#define LFO_VCA_COUNT     4

//#######################################################################
enum class SHAPE {
    SINE = 0,
    TRIANGLE,
    SAWTOOTH,
    PULSE,
    };

enum class D_A_OFF {
    BEND = 0,
    WIDTH,
    FREQ,
    DIR,
    SINE,
    PULSE,
    SAWTOOTH,
    TRIANGLE
    };

#define MAX_FREQ_DEV    36

//#######################################################################
class   SYNTH_LFO_C
    {
private:
    bool    Valid;
    int     Active;
    int     Number;
    byte    OscChannel;
    byte    PwmChannel;
    byte    BendChannel;
    bool    Update;
    int     CurrentLevel;
    int     FreqDiv;
    float   CurrentPercent;

    typedef struct
        {
        byte        Channel;            // I2C device index
        uint16_t    CurrentLevel;       // Current setting 12 bit D/A
        uint16_t    MaximumLevel;       // No higher than this
        bool        Select;
        String      Name;
        } VCA_T;

    VCA_T     Vca[OSC_MIXER_COUNT];

    void  ClearState (void);
    void  SetLevel  (byte ch, byte data);

public:
         SYNTH_LFO_C     (void);
    void Begin           (int num, byte first_device);
    void Clear           (void);
    void SetFreq         (byte data);
    void SetMaxLevel     (byte ch, byte data);
    void Level           (byte data);
    void Select          (byte ch, bool sel);
    void PitchBend       (float percent);
    void Range           (bool up);

    };
}



//#######################################################################
// Module:     LFOosc.h
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       9/18/2023
//#######################################################################
#pragma once

#include "config.h"


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

#define MAX_FREQ_DEV    32

//#######################################################################
class   SYNTH_LFO_C
    {
private:
    bool    Valid;
    int     Active;
    int     Number;
    uint8_t    OscChannel;
    uint8_t    PwmChannel;
    uint8_t    BendChannel;
    bool    UpdateNeded;
    float   CurrentPercent;
    uint8_t CurrentLevel;

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
    void  SetLevel  (uint8_t ch, uint8_t data);

public:
         SYNTH_LFO_C     (void);
    void Begin           (int num, uint8_t first_device);
    void Clear           (void);
    void SetFreq         (float percent);
    void SetMaxLevel     (uint8_t ch, uint8_t data);
    void Level           (uint8_t data);
    void Select          (uint8_t ch, bool sel);
    void PitchBend       (float percent);
    };



//#######################################################################
// Module:     Noise.h
// Descrption: Load and save setting and message settings
// Creator:    markeby
// Date:       8/1/2022
//#######################################################################
#pragma once

//#################################################
//    Synthesizer noise control
//#################################################
class NOISE_C
    {
private:
    int     FirstDigital;
    int     FirstAnallog;
    byte    FilterSelected;
    int     FilterDigital[3];
    int     NoiseWhite;

public:
            NOISE_C         (void);
    void    Begin           (int digital, int analog);
    void    FilterValue     (byte val);
    void    FilterSelect    (byte bit, bool state);
    void    FilterSelect    (byte seleci);
    void    NoiseSelect     (byte sel);
    void    SetMaxLevel     (float level_percent);
    byte    FilterIs        (void)
        { return (FilterSelected); }
    };


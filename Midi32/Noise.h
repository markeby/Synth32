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
    int             FirstDigital;
    byte            FilterSelected;

public:
             NOISE_C            (void);
    void     Begin              (int digital);
    void     FilterValue        (byte val);
    void     FilterSelect       (byte bit, bool state);
    void     FilterSelect       (byte seleci);
    byte     FilterIs           (void)              { return (FilterSelected); }
    };


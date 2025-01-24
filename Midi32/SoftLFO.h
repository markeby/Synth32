//#######################################################################
// Module:     SoftLFO.h
// Descrption: Sine wave processor
// Creator:    markeby
// Date:       7/05/2024
//#######################################################################
#pragma once

//#######################################################################
//#######################################################################
class SOFT_LFO_C
    {
private:
    float   Frequency;
    float   WaveLength;
    float   Current;
    float   Sine;
    float   Modulation;

public:
         SOFT_LFO_C     (void);
        ~SOFT_LFO_C     ()  {}

    void SetFrequency   (short value);
    void Loop           (float millisec);
    void Multiplier     (float value);

    float GetSin        (void)          { return (( this->Modulation > .05 ) ? this->Sine : 0.0); }
    };

extern SOFT_LFO_C SoftLFO;


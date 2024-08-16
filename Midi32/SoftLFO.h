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
    float   WaveLength;
    float   HalfWaveLength;
    float   Current;
    float   Triangle;
    float   Sine;
    float   Modulation;

public:
         SOFT_LFO_C     (void);
        ~SOFT_LFO_C     ()  {}

    void SetFrequency   (float freqpercent);
    void Loop           (float millisec);
    void Multiplier     (float value);

    float GetSin        (void)          { return (this->Sine); }
    float GetTri        (void)          { return (this->Triangle); }
    };

extern SOFT_LFO_C SoftLFO;


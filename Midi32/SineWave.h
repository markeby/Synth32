//#######################################################################
// Module:     SineWave.h
// Descrption: Sine wave processor
// Creator:    markeby
// Date:       7/05/2024
//#######################################################################
#pragma once

//#######################################################################
//#######################################################################
class SINEWAVE_C
    {
private:
    float   WaveLength;
    float   Current;
    float   Sine;
    float   Modulation;

public:
         SINEWAVE_C     (void);
        ~SINEWAVE_C     ()  {}

    void SetFrequency   (int freq);
    void Loop           (float deltaTime);
    void Multiplier     (float value);

    float GetSin        (void)          { return (this->Sine); }
    };

extern SINEWAVE_C SineWave;


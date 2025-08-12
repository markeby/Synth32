//#######################################################################
// Module:     SoftLFO.h
// Descrption: Sine wave processor
// Creator:    markeby
// Date:       7/05/2024
//#######################################################################
#pragma once
#include "Config.h"

//#######################################################################
//#######################################################################
class SOFT_LFO_C
    {
private:
    short   CurrentFreqCoarse;
    short   CurrentFreqFine;
    short   CurrentFreq;
    float   Frequency;
    float   WaveLength;
    float   Current;
    float   Sine;
    float   Modulation;
    byte    Midi;

    void OutputFrequency    (void);
    void ProcessFreq        (void);

public:
         SOFT_LFO_C     (void);
    void Loop           (float millisec);
    void Multiplier     (byte mchan, float value);

    void  SetMidi (byte mchan)
        { this->Midi = mchan; }

    byte  GetMidi (void)
        { return (this->Midi); }

    float GetSin (void)
        { return (( this->Modulation > .05 ) ? this->Sine : 0.0); }

    void SetFreqCoarse (short value)
        { this->CurrentFreqCoarse = value; this->ProcessFreq (); }

    void SetFreqFine (short value)
        { this->CurrentFreqFine = value; this->ProcessFreq (); }

    short GetFreq (void)
        { return (this->CurrentFreq); }

    void SetFreq (short value)
        { this->CurrentFreq = value; this->OutputFrequency (); }
    };

extern SOFT_LFO_C SoftLFO;


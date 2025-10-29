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
    short   _FreqCoarse;
    short   _FreqFine;
    short   _Freq;
    float   _Frequency;
    float   _WaveLength;
    float   _Current;
    float   _Sine;
    float   _Triangle;
    float   _Modulation;
    byte    _Midi;

    void OutputFrequency    (void);
    void ProcessFreq        (void);

public:
          SOFT_LFO_C    (void);
    void  Loop          (float millisec);
    void  Multiplier    (byte mchan, float value);
    void  ResetControl  (void)              {  _Modulation = 0; }
    void  SetMidi       (byte mchan)        { _Midi = mchan; }
    byte  GetMidi       (void)              { return (_Midi); }
    float GetTri        (void)              { return (( _Modulation > .005 ) ? _Triangle : 0.0); }
    float GetSin        (void)              { return (( _Modulation > .005 ) ? _Sine : 0.0); }
    void  SetFreqCoarse (short value)       { _FreqCoarse = value; ProcessFreq (); }
    void  SetFreqFine   (short value);
    short GetFreq       (void)              { return (_Freq); }
    void  SetFreq       (short value)       { _Freq = value; OutputFrequency (); }
    };

extern SOFT_LFO_C SoftLFO;


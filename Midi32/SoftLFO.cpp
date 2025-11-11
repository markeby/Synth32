//#######################################################################
#include "SoftLFO.h"
#include "Debug.h"

#ifdef DEBUG_SYNTH
static const char* Label = "LFO-S";
#define DBG(args...) {if(DebugSynth){ DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#endif

//#######################################################################
//#######################################################################
    SOFT_LFO_C::SOFT_LFO_C ()
    {
    _FreqCoarse = 0;
    _FreqFine   = 1;
    ProcessFreq ();
    _Current = 0.0;
    _Midi = 0;
    }

//#######################################################################
void SOFT_LFO_C::SetFreqFine (short value)
    {
    if ( value == 0 )
        value = 1;
    _FreqFine = value;
    ProcessFreq ();
    }
//#######################################################################
void SOFT_LFO_C::ProcessFreq ()
    {
    _Freq = (_FreqCoarse * MIDI_MULTIPLIER) + _FreqFine;
    DBG("_FreqCoarse = %d    _FreqFine = %d    _freq = %d", _FreqCoarse, _FreqFine, _Freq);
    if ( _Freq > DA_MAX )
        _Freq = DA_MAX;
    OutputFrequency ();
    }

//#######################################################################
void SOFT_LFO_C::OutputFrequency ()
    {
    _Frequency = _Freq * 0.014648;
    _WaveLength = 1000 / _Frequency;
    DBG ("Frequency = %f Hz  Sine Wavelength = %f ms", _Frequency, _WaveLength);
    }

//#######################################################################
// Generator for sin  and triangle waves
//  - Output is -1 to +1
//#######################################################################
void SOFT_LFO_C::Loop ()
    {
    // Calculate current position of wavelength and remove overflow
    _Current += DeltaTimeMilli;
    if ( _Current > _WaveLength )
        _Current -= _WaveLength;

    float zr = _Current / _WaveLength;  //Determine percentage of wavelength achieved and convert to radians
    float zt = zr * 2;                  //Double that value to use 1.0 as direction change downward for triangle

    //Calculate position in triangle wave
    if ( zt > 1.0 )                 //going down
        _Triangle = (1 - (zt - 1) - 0.5) * 2;
    else                            //going up
        _Triangle = (zt - 0.5) * 2;

    _Sine = sin (zr  * 6.28);       //Calculate position in sine wave

#if 0
    uint16_t zs = (uint16_t)((_Sine + 1.0) * 2047.0);
    uint16_t zz = (uint16_t)((_Triangle + 1.0) * 2047.0);
    I2cDevices.D2Analog (181, zz);     // for calibration testing
    I2cDevices.D2Analog (180, zs);     // for calibration testing
    I2cDevices.UpdateAnalog  ();
#endif
    }

//#######################################################################
void SOFT_LFO_C::Multiplier (byte mchan, float value)
    {
    if ( mchan == _Midi )
        _Modulation = value;
    }

//#######################################################################
SOFT_LFO_C   SoftLFO;


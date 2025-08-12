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
    this->CurrentFreqCoarse = 0;
    this->CurrentFreqFine   = 1;
    this->ProcessFreq ();
    this->Current = 0.0;
    this->Midi = 0;
    }

//#######################################################################
void SOFT_LFO_C::ProcessFreq ()
    {
    this->CurrentFreq = (this->CurrentFreqCoarse * MIDI_MULTIPLIER) + (this->CurrentFreqFine * (MIDI_MULTIPLIER * 0.01));
    if ( this->CurrentFreq > DA_MAX )
        this->CurrentFreq = DA_MAX;
    }

//#######################################################################
void SOFT_LFO_C::OutputFrequency ()
    {
    this->Frequency = (short)((float)this->CurrentFreq * 0.014648);
    this->WaveLength = 1000 / this->Frequency;
    DBG ("Frequency = %f Hz  Wavelength = %f ms", this->Frequency, this->WaveLength);
    }

//#######################################################################
void SOFT_LFO_C::Loop (float millisec)
    {
    // Calculate current position of wavelength and remove overflow
    this->Current += millisec;
    if ( this->Current > this->WaveLength )
        this->Current -= this->WaveLength;

    // Determine percentage of wavelength achieved and convert to radians
    float zf = this->Current / this->WaveLength;

    // Calculate position in sine wave and factor in modulation wheel position
    this->Sine = (sin (zf  * 6.28) + 1.0) * Modulation;
//    I2cDevices.D2Analog (184, (uint16_t)(this->Sine * 2047.0));     // for calibration
    }

//#######################################################################
void SOFT_LFO_C::Multiplier (byte mchan, float value)
    {
    if ( mchan == Midi )
        this->Modulation = value;
    }

//#######################################################################
SOFT_LFO_C   SoftLFO;


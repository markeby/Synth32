//#######################################################################
#include "SoftLFO.h"
#include "Debug.h"

#ifdef DEBUG_SYNTH
static const char* Label = "LFO-S";
#define DBG(args...) {if(DebugOsc){ DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#endif

//#######################################################################
//#######################################################################
    SOFT_LFO_C::SOFT_LFO_C ()
    {
    this->SetFrequency (1);
    this->Current = 0.0;
    this->Midi = 0;
    }

//#######################################################################
void SOFT_LFO_C::SetFrequency (short value)
    {
    this->Frequency = value * 0.014648;
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
    this->Sine = (sin (zf  * 6.28) + 1.0) * 0.5 * Modulation;
//    I2cDevices.D2Analog (87, (uint16_t)((1.0 + Sine) * 2047.0));
    }

//#######################################################################
void SOFT_LFO_C::Multiplier (byte mchan, float value)
    {
    if ( mchan == Midi )
        {
        Modulation = value;
        DBG ("Modulation = %f", Modulation);
        }
    }

//#######################################################################
SOFT_LFO_C   SoftLFO;


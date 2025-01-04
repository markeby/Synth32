//#######################################################################
#include "config.h"
#include "SoftLFO.h"
#include "Debug.h"

#ifdef DEBUG_ON
static const char* Label = "LFO-S";
#define DBG(args...) {if(DebugOsc){ DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#endif

//#######################################################################
//#######################################################################
    SOFT_LFO_C::SOFT_LFO_C ()
    {
    SetFrequency (10);
    Current = 0.0;
    }

//#######################################################################
void SOFT_LFO_C::SetFrequency (short value)
    {
    this->WaveLength = value;      // frequency / 2
    this->HalfWaveLength = this->WaveLength * 0.5;
    DBG ("Frequency = %f   Wavelength = %f", 1.0 / this->WaveLength, this->WaveLength);
    }

//#######################################################################
void SOFT_LFO_C::Loop (float millisec)
    {
    Current += millisec / WaveLength;  // process zero to one

    if ( Current > WaveLength )         // remove overflow
        Current -= WaveLength;

    Sine = sin (Current * 6.28) * Modulation;        // Current is zero to one so convert to radians then generate -1.0 to 1.0
//    I2cDevices.D2Analog (84, (uint16_t)((1.0 + Sine) * 2047.0));
    }

//#######################################################################
void SOFT_LFO_C::Multiplier (float value)
    {
    Modulation = value;
    DBG ("Modulation = %f", Modulation);
    }

SOFT_LFO_C SoftLFO;


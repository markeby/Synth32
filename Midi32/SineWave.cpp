//#######################################################################
#include "config.h"
#include "SineWave.h"
#include "Debug.h"
static const char* Label = "SINE";
#define DBG(args...) {if(DebugOsc){ DebugMsg(Label,DEBUG_NO_INDEX,args);}}

//#######################################################################
//#######################################################################
    SINEWAVE_C::SINEWAVE_C ()
    {
    SetFrequency (10);
    Current = 0.0;
    }

void SINEWAVE_C::SetFrequency (int freq)
    {
    this->WaveLength = 2000 / freq;      // frequency / 2
    DBG ("Frequency = %d   Wavelength = %f", freq, this->WaveLength);
    }

//#######################################################################
void SINEWAVE_C::Loop (float deltaTime)
    {
    Current += deltaTime / WaveLength;  // process zero to one

    if ( Current > WaveLength )         // remove overflow
        Current -= WaveLength;

    Sine = sin (Current * 6.28) * Modulation;        // Current is zero to one so convert to radians then generate -1.0 to 1.0
    I2cDevices.D2Analog (84, (uint16_t)((1.0 + Sine) * 2047.0));
    }

//#######################################################################
void SINEWAVE_C::Multiplier (float value)
    {
    Modulation = value;
    DBG ("Modulation = %f", Modulation);
    }

SINEWAVE_C SineWave;


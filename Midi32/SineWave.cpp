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
    SetFrequency (100);
    Current = 0.0;
    }

void SINEWAVE_C::SetFrequency (int freq)
    {
    this->WaveLength = 1.0 / freq;
    DBG ("Frequency = %d   Wavelength = %f", freq, this->WaveLength);
    }

//#######################################################################
void SINEWAVE_C::Loop (float deltaTime)
    {
    Current += WaveLength / deltaTime;  // process zero to one

    if ( Current > WaveLength )         // remove overflow
        Current -= WaveLength;

    Sine = sin (Current * 6.28);        // Current is zero to one so convert to radians
    }



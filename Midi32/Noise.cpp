//#######################################################################
// Module:     Noise.cpp
// Descrption: Noise controls
// Creator:    markeby
// Date:       2/8/2025
//#######################################################################
#include "Noise.h"
#include "Debug.h"

#ifdef DEBUG_SYNTH
static const char* Label = "LFO-S";
#define DBG(args...) {if(DebugOsc){ DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#endif

using namespace NOISE_N;

//#######################################################################
//#######################################################################
    NOISE_C::NOISE_C (int dig_first)
    {
    this->DigitalBase = dig_first;
    this->Reset ();
    }

//#######################################################################
void NOISE_C::Reset ()
    {
    for ( int z = 0;  z < NOISE_DUCT_TOTAL;  z++ )
        {
        I2cDevices.DigitalOut (DigitalBase + z, false);
        Duct[z / DUCT_COLOR_NUM].data[z % DUCT_COLOR_NUM] = false;
        }
    I2cDevices.UpdateDigital ();
    }

//#######################################################################
void NOISE_C::Select (int duct, int color, bool select)
    {
    I2cDevices.DigitalOut (DigitalBase + (duct * DUCT_COLOR_NUM) + color, select);
    Duct[duct].data[color] = select;
    I2cDevices.UpdateDigital ();
    }


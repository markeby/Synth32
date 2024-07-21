//#######################################################################
#include "Diagnostics.h"
#include "config.h"
#define WAVE_LENGTH 100.0
#define MIDDLE      (4095/2)
//#######################################################################
void AnalogDiagnostics ()
    {
    static float  current = 0.0;

    current += WAVE_LENGTH / DeltaTimeMicro;

    if ( current > WAVE_LENGTH )
        current -= WAVE_LENGTH;

    uint16_t da = (sin (current * 6.28) * MIDDLE) + MIDDLE;      // calculate in radians

    for ( int z = 0;  z < D_A_COUNT;  z++ )
        I2cDevices.D2Analog (z, da);
    I2cDevices.UpdateAnalog();
    }



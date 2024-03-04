//#######################################################################
// Module:     SyntFront.ino
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include <DispMessages.h>
#include "config.h"
#include "LFOosc.h"
#include "Noise.h"

//#######################################################################
//#######################################################################
void NOISE_C::Begin (int digital, int analog)
    {
    printf ("\t>>> Noise startup\n");
    FilterDigital[0] = digital + 2;
    FilterDigital[1] = digital + 4;
    FilterDigital[2] = digital + 6;
    NoiseWhite       = digital + 1;
    FirstDigital     = digital;
    }

//###################################################################
    NOISE_C::NOISE_C ()
    {
    FilterSelected = 0;
    }

//#######################################################################
void NOISE_C::FilterValue (byte val)
    {
    int z = (float)val * MIDI_MULTIPLIER;

    I2cDevices.D2Analog (68, z);
    I2cDevices.D2Analog (69, z);
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void NOISE_C::FilterSelect (byte bit, bool state)
    {
    bit = 1 << bit;
    if ( state )
        FilterSelected |= bit;
    else
        FilterSelected &= ~bit;
    FilterSelect (FilterSelected);
    }

//#######################################################################
void NOISE_C::NoiseSelect (byte sel)
    {
    I2cDevices.DigitalOut (NoiseWhite, sel ? 1 : 0);
    I2cDevices.UpdateDigital ();    // Update Digital output ports
    }

//#######################################################################
void NOISE_C::FilterSelect (byte select)
    {
    FilterSelected = select;

    for ( int z = 0;  z < 3;  z++ )
        {
        I2cDevices.DigitalOut (FilterDigital[z],     ((select >> 1) & 1) ^ 1);
        I2cDevices.DigitalOut (FilterDigital[z] + 1, (select & 1) ^ 1);
        }
    if ( DebugSynth )
        printf ("Noise Filster setting %d\n", select);
    I2cDevices.UpdateDigital ();    // Update Digital output ports
    }
//#######################################################################
void NOISE_C::SetMaxLevel (float level_percent)
    {
    int z = (int)(level_percent * (float)MAXDA);
    I2cDevices.D2Analog (70, z);
    I2cDevices.D2Analog (71, z);
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }


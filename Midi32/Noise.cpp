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
void NOISE_C::Begin (int digital)
    {
    printf ("\t>>> Noise startup\n");
    FirstDigital = digital;
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
    I2cDevices.D2Analog (70, z);
    I2cDevices.D2Analog (71, z);
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
void NOISE_C::FilterSelect (byte select)
    {
    FilterSelected = select;

    I2cDevices.DigitalOut  (FirstDigital + 1, (select & 1) ^ 1);
    I2cDevices.DigitalOut  (FirstDigital, ((select >> 1) & 1) ^ 1);
    if ( DebugSynth )
        printf ("Noise Filster setting %d\n", select);
    I2cDevices.UpdateDigital ();    // Update Digital output ports
    }


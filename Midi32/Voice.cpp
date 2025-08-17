//#######################################################################
// Module:     Port.cpp
// Descrption: Synthesizer Port processing
// Creator:    markeby
// Date:       3/16/2024
//#######################################################################
#include <Arduino.h>
#include "Osc.h"
#include "Voice.h"

//#####################################################################
VOICE_C::VOICE_C (short num, short osc_d_a, short mux_digital, short mod_mux_digital, short noise_digitinal, ENVELOPE_GENERATOR_C& envgen)
    {
    Number               = num;
    ActiveTimer          = 0;
    Key                  = -1;
    UseCount             = 0;
    Midi                 = 1;
    TuningOn             = false;

    if ( num & 1 )
        {
        DigitalOutOscillator = mux_digital + 8;
        DigitalOutFilter     = mux_digital;
        }
    else
        {
        DigitalOutOscillator = mux_digital;
        DigitalOutFilter     = mux_digital + 8;
        }
    NoiseDigitalIO       = noise_digitinal;
    NoiseReset ();

    ModMux[0] = mod_mux_digital + 8;
    ModMux[1] = mod_mux_digital;
    ModMux[2] = mod_mux_digital + 4;
    Osc.Begin (num, osc_d_a, UseCount, envgen);
    Flt.Begin (num, osc_d_a + (( num & 1 ) ? 10 : 16), UseCount, envgen);
    SetMux (true);                   // default with oscillators enable
    I2cDevices.UpdateDigital ();
    I2cDevices.UpdateAnalog  ();
    }

//#######################################################################
void VOICE_C::Loop ()
    {
    if ( ActiveTimer )
        {
        ActiveTimer += DeltaTimeMilli;
        if ( UseCount == 0 )
            ActiveTimer = 0;
        }
    }
//#######################################################################
void VOICE_C::NoteSet (byte mchan, byte key, byte velocity)
    {
    ActiveTimer = 1;
    Key = key;
    Osc.NoteSet (key, velocity);
    Flt.NoteSet (key, velocity);
    }

//#######################################################################
bool VOICE_C::NoteClear (byte mchan, byte key)
    {
    if ( key != Key )
        return (false);
    if ( mchan == Midi )
        {
        Osc.NoteClear ();
        Flt.NoteClear ();
        Key = -1;
        return (true);
        }
    return (false);
    }

//#######################################################################
void VOICE_C::SetMux (bool bypass)
    {
    I2cDevices.DigitalOut (DigitalOutFilter, bypass);
    I2cDevices.DigitalOut (DigitalOutOscillator, true);           // Just hard code on until I build an newer mixer/switch board
    I2cDevices.UpdateDigital ();
    }

//#######################################################################
void VOICE_C::SetModMux (byte select)
    {
    for ( int z = 0;  z < NUM_MOD_MUX_IN;  z++ )
        I2cDevices.DigitalOut (ModMux[z], (z == select));
    }

//#######################################################################
void VOICE_C::NoiseSelect (byte color)
    {
    for ( byte z = 0;  z < 4;  z++ )
        I2cDevices.DigitalOut (NoiseDigitalIO + z, color == z);
    NoiseSource = color;
    I2cDevices.UpdateDigital ();
    }

//#######################################################################
void VOICE_C::NoiseReset ()
    {
    for ( int z = 0;  z < SOURCE_CNT_NOISE;  z++ )
        I2cDevices.DigitalOut (NoiseDigitalIO + z, false);
    }

//#######################################################################
void VOICE_C::TuningAdjust (bool up)
    {
    if ( TuningOn )
        Osc.TuningAdjust (up);
    }



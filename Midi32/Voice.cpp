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
    this->Number               = num;
    this->ActiveTimer          = 0;
    this->Key                  = -1;
    this->UseCount             = 0;
    this->Midi                 = 1;
    if ( num & 1 )
        {
        this->DigitalOutOscillator = mux_digital + 8;
        this->DigitalOutFilter     = mux_digital;
        }
    else
        {
        this->DigitalOutOscillator = mux_digital;
        this->DigitalOutFilter     = mux_digital + 8;
        }
    this->NoiseDigitalIO       = noise_digitinal;
    this->NoiseReset ();

    this->ModMux[0] = mod_mux_digital + 8;
    this->ModMux[1] = mod_mux_digital;
    this->ModMux[2] = mod_mux_digital + 4;
    this->pOsc = new OSC_C (num, osc_d_a, UseCount, envgen);
    I2cDevices.UpdateDigital ();
    I2cDevices.UpdateAnalog  ();
    }

//#######################################################################
void VOICE_C::Loop ()
    {
    if ( this->ActiveTimer )
        {
        this->ActiveTimer += DeltaTimeMilli;
        if ( this->UseCount == 0 )
            this->ActiveTimer = 0;
        }
    }
//#######################################################################
void VOICE_C::NoteSet (byte mchan, byte key, byte velocity)
    {
    this->ActiveTimer = 1;
    this->Key = key;
    this->pOsc->NoteSet (key, velocity);
    }

//#######################################################################
bool VOICE_C::NoteClear (byte mchan, byte key)
    {
    if ( key != this->Key )
        return (false);
    if ( mchan == this->Midi )
        {
        this->pOsc->NoteClear();
        this->Key = -1;
        return (true);
        }
    return (false);
    }

//#######################################################################
void VOICE_C::SetMux (byte select, bool enable)
    {
    switch ( select )
        {
        case 1:
            I2cDevices.DigitalOut (this->DigitalOutFilter, enable);
            break;
        default:       // zero
            I2cDevices.DigitalOut (this->DigitalOutFilter, enable);
            break;
        }
    }

//#######################################################################
void VOICE_C::SetModMux (byte select)
    {
    for ( int z = 0;  z < NUM_MOD_MUX_IN;  z++ )
        I2cDevices.DigitalOut (this->ModMux[z], (z == select));
    }

//#######################################################################
void VOICE_C::NoiseSelect (byte color)
    {
    for ( byte z = 0;  z < 4;  z++ )
        I2cDevices.DigitalOut (this->NoiseDigitalIO + z, color == z);
    this->NoiseSource = color;
    I2cDevices.UpdateDigital ();
    }

//#######################################################################
void VOICE_C::NoiseReset ()
    {
    for ( int z = 0;  z < SOURCE_CNT_NOISE;  z++ )
        I2cDevices.DigitalOut (this->NoiseDigitalIO + z, false);
    }


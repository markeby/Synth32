//#######################################################################
// Module:     Voice.cpp
// Descrption: Voice class which contains oscillators and filters
// Creator:    markeby
// Date:       3/16/2024
//#######################################################################
#include <Arduino.h>
#include "Osc.h"
#include "Voice.h"
#include "Stats.h"

//#####################################################################
VOICE_C::VOICE_C (short num, short osc_d_a, short mixer, short mod_mux_digital, short noise_digitinal, ENV_GENERATOR_C& envgen)
    {
    _Number               = num;
    _ActiveTimer          = 0;
    _Key                  = -1;
    _UseCount             = 0;
    _Midi                 = 1;
    _TuningOn             = false;

    _DigitalOutLpFilter   = mixer;
    _DigitalOutOscillator = mixer + 8;
    _NoiseDigitalIO       = noise_digitinal;
    NoiseReset ();

    _ModMux[0] = mod_mux_digital + 8;
    _ModMux[1] = mod_mux_digital;
    _ModMux[2] = mod_mux_digital + 4;
    _Osc.Begin   (num, osc_d_a, _UseCount, envgen);
    _Flt4.Begin  (num, osc_d_a + (( num & 1 ) ? 10 : 16), _UseCount, envgen);
    _FltLP.Begin (num, osc_d_a + (( num & 1 ) ? 20 : 28), _UseCount, envgen);
    I2cDevices.UpdateDigital ();
    I2cDevices.UpdateAnalog  ();
    }

//#######################################################################
void VOICE_C::Loop ()
    {
    if ( _ActiveTimer )
        {
        _ActiveTimer += DeltaTimeMilli;
        if ( _UseCount == 0 )
            _ActiveTimer = 0;
        }
    }
//#######################################################################
void VOICE_C::NoteSet (byte mchan, byte key, byte velocity)
    {
    _ActiveTimer = 1;
    _Key = key;
    _Osc.NoteSet   (key, velocity);
    _Flt4.NoteSet  (key, velocity);
    _FltLP.NoteSet (key, velocity);
    SynthStats.Bump (_Number);
    }

//#######################################################################
bool VOICE_C::NoteClear (byte mchan, byte key)
    {
    if ( key != _Key )
        return (false);
    if ( mchan == _Midi )
        {
        _Osc.NoteClear   ();
        _Flt4.NoteClear  ();
        _FltLP.NoteClear ();
        _Key = -1;
        return (true);
        }
    return (false);
    }

//#######################################################################
void VOICE_C::SetModMux (byte select)
    {
    for ( int z = 0;  z < NUM_MOD_MUX_IN;  z++ )
        I2cDevices.DigitalOut (_ModMux[z], (z == select));
    }

//#######################################################################
void VOICE_C::NoiseSelect (byte color)
    {
    for ( byte z = 0;  z < 4;  z++ )
        I2cDevices.DigitalOut (_NoiseDigitalIO + z, color == z);
    _NoiseSource = color;
    I2cDevices.UpdateDigital ();
    }

//#######################################################################
void VOICE_C::NoiseReset ()
    {
    for ( int z = 0;  z < SOURCE_CNT_NOISE;  z++ )
        I2cDevices.DigitalOut (_NoiseDigitalIO + z, false);
    }

//#######################################################################
void VOICE_C::SetOutputMask (byte bitmap)
    {
    I2cDevices.DigitalOut (_DigitalOutOscillator, bitmap & 1);
    _Flt4.SetOutMap ((bitmap >> 1) & 0x0F);
    I2cDevices.DigitalOut (_DigitalOutLpFilter, bitmap & (1 << 5));
    I2cDevices.UpdateDigital ();
    }

//#######################################################################
void VOICE_C::TuningAdjust (bool up)
    {
    if ( _TuningOn )
        _Osc.TuningAdjust (up);
    }

STATS_C SynthStats;

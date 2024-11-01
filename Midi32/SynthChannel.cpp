//#######################################################################
// Module:     SynthChannel.cpp
// Descrption: Synthesizer Channel processing
// Creator:    markeby
// Date:       3/16/2024
//#######################################################################
#include <Arduino.h>
#include "config.h"
#include "Osc.h"
#include "SynthChannel.h"

//#####################################################################
SYNTH_CHANNEL_C::SYNTH_CHANNEL_C (int num, int osc_d_a, ENVELOPE_GENERATOR_C& envgen)
    {
    Number = num;
    ActiveTimer = 0;
    Key = -1;
    UseCount = 0;
    OscP   = new SYNTH_OSC_C (num, osc_d_a, UseCount, envgen);
    }

//#######################################################################
void SYNTH_CHANNEL_C::Loop ()
    {
    if ( ActiveTimer )
        {
        ActiveTimer += DeltaTimeMilli;
        if ( UseCount == 0 )
            ActiveTimer = 0;
        }
    }
//#######################################################################
void SYNTH_CHANNEL_C::NoteSet (uint8_t key, uint8_t velocity)
    {
    ActiveTimer = 1;
    Key = key;
    OscP->NoteSet (key, velocity);
    }

//#######################################################################
void SYNTH_CHANNEL_C::Clear (void)
    {
    OscP->Clear ();
    }

//#######################################################################
bool SYNTH_CHANNEL_C::NoteClear (uint8_t key)
    {
    if ( key != Key )
        return false;
    OscP->NoteClear ();
    Key = -1;
    return true;
    }



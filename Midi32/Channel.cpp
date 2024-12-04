//#######################################################################
// Module:     Channel.cpp
// Descrption: Synthesizer Channel processing
// Creator:    markeby
// Date:       3/16/2024
//#######################################################################
#include <Arduino.h>
#include "config.h"
#include "Osc.h"
#include "Channel.h"

//#####################################################################
CHANNEL_C::CHANNEL_C (int num, int osc_d_a, ENVELOPE_GENERATOR_C& envgen)
    {
    this->Number      = num;
    this->ActiveTimer = 0;
    this->Key         = -1;
    this->UseCount    = 0;
    this->OscP        = new OSC_C (num, osc_d_a, UseCount, envgen);
    }

//#######################################################################
void CHANNEL_C::Loop ()
    {
    if ( this->ActiveTimer )
        {
        this->ActiveTimer += DeltaTimeMilli;
        if ( this->UseCount == 0 )
            this->ActiveTimer = 0;
        }
    }
//#######################################################################
void CHANNEL_C::NoteSet (byte key, byte velocity)
    {
    this->ActiveTimer = 1;
    this->Key = key;
    this->OscP->NoteSet (key, velocity);
    }

//#######################################################################
bool CHANNEL_C::NoteClear (byte key)
    {
    if ( key != this->Key )
        return false;
    this->OscP->NoteClear ();
    this->Key = -1;
    return true;
    }


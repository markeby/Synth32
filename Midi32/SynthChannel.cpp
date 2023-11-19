//#######################################################################
// Module:     SyntFront.ino
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include "config.h"
#include "Osc.h"
#include "SynthChannel.h"

//#####################################################################
SYNTH_CHANNEL_C::SYNTH_CHANNEL_C (int num, int first_device)
    {
    Number = num;
    FirstDevice = first_device;
    ActiveTimer = 0;
    Key = -1;
    Osc.Begin (num, FirstDevice);
    }

//#######################################################################
void SYNTH_CHANNEL_C::Loop ()
    {
    if ( ActiveTimer )
        {
        ActiveTimer += DeltaTime;

        if ( Osc.Loop () )
            ActiveTimer = 0;
        }
    }
//#######################################################################
void SYNTH_CHANNEL_C::SetTuning (void)
    {
    Osc.SetTuning ();
    }

//#######################################################################
uint32_t SYNTH_CHANNEL_C::Active (void)
    {
    return (ActiveTimer);
    }

//#######################################################################
void SYNTH_CHANNEL_C::NoteSet (uint8_t key, uint8_t velocity)
    {
    ActiveTimer = 1;
    Key = key;
    Osc.NoteSet (key, velocity);
    }

//#######################################################################
void SYNTH_CHANNEL_C::Clear (void)
    {
    Osc.Clear ();
    }

//#######################################################################
bool SYNTH_CHANNEL_C::NoteClear (uint8_t key)
    {
    if ( key != Key )
        return false;
    Osc.NoteClear ();
    Key = -1;
    return true;
    }



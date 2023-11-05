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

//#####################################################################
void SYNTH_CHANNEL_C::SetMaxLevel (int wave, float data)
    {
    Osc.SetMaxLevel (wave, data);
    }

//#####################################################################
void SYNTH_CHANNEL_C::SetAttack (int wave, float data)
    {
    Osc.SetAttack (wave, data);
    }

//#####################################################################
void SYNTH_CHANNEL_C::SetDecay (int wave, float data)
    {
    Osc.SetDecay (wave, data);
    }

//#####################################################################
void SYNTH_CHANNEL_C::SetSustainLevel (int wave, float data)
    {
    Osc.SetSustainLevel (wave, data);
    }

//#####################################################################
void SYNTH_CHANNEL_C::SetSustainTime (int wave, float data)
    {
    Osc.SetSustainTime (wave, data);
    }

//#####################################################################
void SYNTH_CHANNEL_C::SetRelease (int wave, float data)
    {
    Osc.SetRelease (wave, data);
    }


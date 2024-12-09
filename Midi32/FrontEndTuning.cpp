//#######################################################################
// Module:     FrontEndTuning.cpp
// Descrption: Synthesizer front end tuning control
// Creator:    markeby
// Date:       12/9/2024
//#######################################################################
#include "../Common/SynthCommon.h"
#include "I2Cmessages.h"
#include "Osc.h"
#include "Settings.h"
#include "FrontEnd.h"
#include "Debug.h"

//#######################################################################
void SYNTH_FRONT_C::Tuning ()
    {
    for ( int zc = 0;  zc < CHAN_COUNT;  zc++ )
        {
        if ( this->DownTrigger )
            {
            this->pChan[zc]->pOsc()->SetTuningNote(DownKey);
            DisplayMessage.TuningNote (DownKey);
            }
        if ( this->TuningChange )
            {
            if ( this->TuningOn[zc] )
                {
                for ( int z = 0;  z < ENVELOPE_COUNT;  z++ )
                    {
                    if ( z < OSC_MIXER_COUNT )
                        {
                        this->pChan[zc]->pOsc()->SetTuningVolume (z, TuningLevel[z]);
                        DisplayMessage.TuningLevel (z, TuningLevel[z] * MIDI_INV_MULTIPLIER);
                        }
                    }
                }
            else
                {
                for ( int z = 0;  z < ENVELOPE_COUNT;  z++ )
                    {
                    if ( z < OSC_MIXER_COUNT )
                        this->pChan[zc]->pOsc()->SetTuningVolume(z, 0);
                    }
                }
            }
        }
    this->TuningChange = false;
    this->DownTrigger = false;
    I2cDevices.UpdateDigital();
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_FRONT_C::StartTuning ()
    {
    if ( SetTuning == false )
        {
        DisplayMessage.PageTuning ();
        for ( int z = 0;  z < ENVELOPE_COUNT;  z++)
            {
            TuningLevel[z] = (uint16_t)(pChan[0]->pOsc()->GetMaxLevel (z) * MAX_DA);
            DisplayMessage.TuningLevel (z, TuningLevel[z] * MIDI_INV_MULTIPLIER);
            }
        for ( int zc = 0;  zc < CHAN_COUNT;  zc++ )
            {
            TuningOn[zc] = false;
            TuningChange = true;
            }
        }
    SetTuning = true;
    }

//#######################################################################
void SYNTH_FRONT_C::TuningAdjust (bool up)
    {
    for ( int zc = 0;  zc < CHAN_COUNT;  zc++ )
        {
        if ( TuningOn[zc] )
            pChan[zc]->pOsc()->TuningAdjust (up);
        }
    }


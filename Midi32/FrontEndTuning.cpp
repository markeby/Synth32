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
        if ( this->Down.Trigger )
            {
            this->pChan[zc]->pOsc()->SetTuningNote(this->Down.Key);
            DisplayMessage.TuningNote (this->Down.Key);
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
    this->Down.Trigger = false;
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
    if ( SetTuning )
        {
        for ( int z = 0;  z < CHAN_COUNT;  z++ )
            {
            if ( TuningOn[z] )
                pChan[z]->pOsc()->TuningAdjust (up);
            }
        }
    }

//#######################################################################
void SYNTH_FRONT_C::TuningBump (bool state)
    {
    if ( SetTuning )
        {
        byte note = (state) ? 113 : 21;        // Highest F or lowest F
        DisplayMessage.TuningNote (note);
        for ( int zc = 0;  zc < CHAN_COUNT;  zc++ )
            pChan[zc]->pOsc()->SetTuningNote (note);
        }
    }


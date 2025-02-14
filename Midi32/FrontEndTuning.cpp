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
    for ( int zc = 0;  zc < VOICE_COUNT;  zc++ )
        {
        if ( this->Down.Trigger )
            {
            this->TuningBender = false;                                 // Not doing bender wheel
            this->pVoice[zc]->pOsc()->SetTuningNote(this->Down.Key);     // send key index to oscillator
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
                        this->pVoice[zc]->pOsc()->SetTuningVolume (z, TuningLevel[z]);
                        DisplayMessage.TuningLevel (z, TuningLevel[z] * MIDI_INV_MULTIPLIER);
                        }
                    }
                }
            else
                {
                for ( int z = 0;  z < ENVELOPE_COUNT;  z++ )
                    {
                    if ( z < OSC_MIXER_COUNT )
                        this->pVoice[zc]->pOsc()->SetTuningVolume(z, 0);
                    }
                }
            }
        }
    this->TuningChange = false;     // Indicate complete and ready for next not change
    this->Down.Trigger = false;     // Clear note change trigger
    I2cDevices.UpdateDigital ();    // Update all digital port changes
    I2cDevices.UpdateAnalog  ();    // Update D/A port changes
    }

//#######################################################################
void SYNTH_FRONT_C::StartTuning ()
    {
    if ( this->SetTuning == false )
        {
        DisplayMessage.PageTuning ();
        for ( int z = 0;  z < ENVELOPE_COUNT;  z++)
            {
            this->TuningLevel[z] = (uint16_t)(this->pVoice[0]->pOsc()->GetMaxLevel (z) * MAX_DA);
            DisplayMessage.TuningLevel (z, this->TuningLevel[z] * MIDI_INV_MULTIPLIER);
            }
        for ( int zc = 0;  zc < VOICE_COUNT;  zc++ )
            {
            this->TuningOn[zc] = false;
            this->TuningChange = true;
            }
        }
    this->TuningBender = true;      // Entering tuning mode starts with the bender wheel
    this->SetTuning    = true;
    }

//#######################################################################
void SYNTH_FRONT_C::TuningAdjust (bool up)
    {
    if ( this->SetTuning )
        {
        if ( this->TuningBender )
            {
            this->PitchBendOffset += ( up ) ? -1 : +1;
            this->PitchBend (PITCH_BEND_CENTER);

            }
        else
            {
            for (int z = 0;  z < VOICE_COUNT;  z++)
                {
                if ( this->TuningOn[z] )
                    this->pVoice[z]->pOsc()->TuningAdjust (up);
                }
            }
        }
    }

//#######################################################################
void SYNTH_FRONT_C::TuningBump (bool state)
    {
    if ( SetTuning )
        {
        this->TuningBender = false;             // Not doing bender wheel
        byte note = (state) ? 113 : 21;         // Highest F or lowest F
        DisplayMessage.TuningNote (note);
        for ( int zc = 0;  zc < VOICE_COUNT;  zc++ )
            this->pVoice[zc]->pOsc()->SetTuningNote (note);
        }
    }


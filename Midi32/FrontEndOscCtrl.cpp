//#######################################################################
// Module:     SyntFront.ino
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <UHS2-MIDI.h>
#include <MIDI.h>

#include "../Common/SynthCommon.h"
#include "../Common/DispMessages.h"
#include "I2Cmessages.h"
#include "Osc.h"
#include "Settings.h"
#include "FrontEnd.h"
#include "Debug.h"

#ifdef DEBUG_ON
static const char* Label  = "TOP";
static const char* LabelM = "M";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#define DBGM(args...) {if(DebugMidi){DebugMsg(LabelM,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#define DBGM(args...)
#endif

//#######################################################################
void SYNTH_FRONT_C::VoiceComponentSetSelected (short chan, bool state)
    {
    byte val = 0x0C;

    this->SynthConfig[this->CurrentMapSelected].SelectedEnvelope[chan] = state;
    for ( int z = 0;  z < OSC_MIXER_COUNT; z++ )
        {
        if ( this->SynthConfig[this->CurrentMapSelected].SelectedEnvelope[z] )
            val = 0x3C;
        }
    this->ShowVoiceXL (val);
    DisplayMessage.OscSelected (chan, this->SynthConfig[this->CurrentMapSelected].SelectedEnvelope[chan]);
    }

//#####################################################################
void SYNTH_FRONT_C::SetMaxLevel (short ch, short data)
    {
    if ( this->SetTuning )
        {
        this->TuningLevel[ch] = data * MIDI_MULTIPLIER;
        DisplayMessage.TuningLevel (ch, data);
        this->TuningChange = true;
        return;
        }

    float val = (float)data * PRS_SCALER;

    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetCMidihannel() )
            this->pVoice[z]->SetMaxLevel (ch, val);
        }

    this->SynthConfig[this->CurrentMapSelected].SetMaxLevel (ch, val);
    DisplayMessage.OscMaxLevel (ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetAttackTime (short data)
    {
    float dtime = data * TIME_MULT;

    for ( int ch = 0;  ch < OSC_MIXER_COUNT;  ch++ )
        {
        if (this->SynthConfig[this->CurrentMapSelected].SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < VOICE_COUNT;  z++ )
                {
                if ( this->CurrentMidiSelected == this->pVoice[z]->GetCMidihannel() )
                    this->pVoice[z]->pOsc()->SetAttackTime(ch, dtime);
                }
            this->SynthConfig[this->CurrentMapSelected].SetAttackTime (ch, dtime);
            DisplayMessage.OscAttackTime (ch, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetDecayTime (short data)
    {
    float dtime = data * TIME_MULT;

    for ( int ch = 0;  ch < OSC_MIXER_COUNT;  ch++ )
        {
        if ( this->SynthConfig[this->CurrentMapSelected].SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < VOICE_COUNT;  z++)
                {
                if ( this->CurrentMidiSelected == this->pVoice[z]->GetCMidihannel() )
                    this->pVoice[z]->pOsc()->SetDecayTime (ch, dtime);
                }
            this->SynthConfig[this->CurrentMapSelected].SetDecayTime (ch, dtime);
            DisplayMessage.OscDecayTime (ch, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetReleaseTime (short data)
    {
    float dtime = data * TIME_MULT;

    for ( int ch = 0;  ch < OSC_MIXER_COUNT;  ch++ )
        {
        if ( this->SynthConfig[this->CurrentMapSelected].SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < VOICE_COUNT;  z++)
                {
                if ( this->CurrentMidiSelected == this->pVoice[z]->GetCMidihannel() )
                    this->pVoice[z]->pOsc()->SetReleaseTime (ch, dtime);
                }
            this->SynthConfig[this->CurrentMapSelected].SetReleaseTime (ch, dtime);
            DisplayMessage.OscReleaseTime (ch, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetSustainLevel (short ch, short data)
    {
    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetCMidihannel() )
            this->pVoice[z]->pOsc()->SetSustainLevel (ch, val);
        }
    this->SynthConfig[this->CurrentMapSelected].SetSustainLevel (ch, val);
    DisplayMessage.OscSustainLevel (ch, data);
    }

//#######################################################################
void SYNTH_FRONT_C::ToggleRampDirection ()
    {
    bool zb = ! this->SynthConfig[this->CurrentMapSelected].GetRampDirection ();

    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetCMidihannel() )
            this->pVoice[z]->pOsc()->SawtoothDirection (zb);
        }

    this->SynthConfig[this->CurrentMapSelected].SetRampDirection (zb);
    DisplayMessage.OscSawtoothDirection (zb);
    }

//#######################################################################
void SYNTH_FRONT_C::SetPulseWidth (short data)
    {
    float percent = data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetCMidihannel() )
            this->pVoice[z]->pOsc()->PulseWidth (percent);
        }
    this->SynthConfig[this->CurrentMapSelected].SetPulseWidth (percent);
    DisplayMessage.OscPulseWidth (data);
    }

//#######################################################################
void SYNTH_FRONT_C::SetNoise (short ch, bool state)
    {
    int z;

    for ( z = 0;  z < DUCT_NUM;  z++ )
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetCMidihannel() )
            this->pNoise->Select (z, ch, state);
        }
    DisplayMessage.OscNoise (ch, state);
    }



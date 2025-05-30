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

#ifdef DEBUG_SYNTH
static const char* Label  = "TOP";
static const char* LabelM = "M";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#define DBGM(args...) {if(DebugMidi){DebugMsg(LabelM,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#define DBGM(args...)
#endif

//#######################################################################
void SYNTH_FRONT_C::VoiceSelectedCheck ()
    {
    if ( this->CurrentMidiSelected == 0 )
        {
        this->CurrentMidiSelected = this->SynthConfig.Voice[0].GetVoiceMidi ();
        this->CurrentMapSelected = 0;
        this->CurrentVoiceSelected = 0;
        DisplayMessage.SelectVoicePage (0);
        return;
        }

    if ( DisplayMessage.GetCurrentVoicePage () != this->CurrentMapSelected )
        DisplayMessage.SelectVoicePage (this->CurrentMapSelected);
    }

//#######################################################################
void SYNTH_FRONT_C::ResetVoiceComponentSetSelected ()
    {
    byte val = 0x0C;

    for (int z = 0;  z < OSC_MIXER_COUNT; z++)
        {
        this->SynthConfig.Voice[z].SelectedEnvelope[z] = false;
        this->ShowVoiceXL (val);
        DisplayMessage.OscSelected (z, false);
        }
    }

//#######################################################################
void SYNTH_FRONT_C::VoiceComponentSetSelected (short chan, bool state)
    {
    byte val = 0x0C;

    this->VoiceSelectedCheck ();
    this->SynthConfig.Voice[this->CurrentVoiceSelected].SelectedEnvelope[chan] = state;
    for ( int z = 0;  z < OSC_MIXER_COUNT; z++ )
        {
        if ( this->SynthConfig.Voice[this->CurrentVoiceSelected].SelectedEnvelope[z] )
            val = 0x3C;
        }
    this->ShowVoiceXL (val);
    DisplayMessage.OscSelected (chan, state);
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

    this->VoiceSelectedCheck ();

    float val = (float)data * PRS_SCALER;

    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
            {
            this->SynthConfig.Voice[z >> 1].SetMaxLevel (ch, val);
            this->pVoice[z]->SetMaxLevel (ch, val);
            }
        }

    DisplayMessage.OscMaxLevel (ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetAttackTime (short data)
    {
    this->VoiceSelectedCheck ();

    float dtime = data * TIME_MULT;

    for ( int ch = 0;  ch < OSC_MIXER_COUNT;  ch++ )
        {
        if (this->SynthConfig.Voice[this->CurrentVoiceSelected].SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < VOICE_COUNT;  z++ )
                {
                if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
                    {
                    this->SynthConfig.Voice[z >> 1].SetAttackTime (ch, dtime);
                    this->pVoice[z]->SetAttackTime (ch, dtime);
                    }
                }
            DisplayMessage.OscAttackTime (ch, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetDecayTime (short data)
    {
    float dtime = data * TIME_MULT;

     this->VoiceSelectedCheck ();

    for ( int ch = 0;  ch < OSC_MIXER_COUNT;  ch++ )
        {
        if ( this->SynthConfig.Voice[this->CurrentVoiceSelected].SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < VOICE_COUNT;  z++)
                {
                if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
                    {
                    this->SynthConfig.Voice[z >> 1].SetDecayTime (ch, dtime);
                    this->pVoice[z]->SetDecayTime (ch, dtime);
                    }
                }
            DisplayMessage.OscDecayTime (ch, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetReleaseTime (short data)
    {
    float dtime = data * TIME_MULT;

    this->VoiceSelectedCheck ();

    for ( int ch = 0;  ch < OSC_MIXER_COUNT;  ch++ )
        {
        if ( this->SynthConfig.Voice[this->CurrentVoiceSelected].SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < VOICE_COUNT;  z++)
                {
                if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
                    {
                    this->SynthConfig.Voice[z >> 1].SetReleaseTime (ch, dtime);
                    this->pVoice[z]->SetReleaseTime (ch, dtime);
                    }
                }
            DisplayMessage.OscReleaseTime (ch, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetSustainLevel (short ch, short data)
    {
    this->VoiceSelectedCheck ();

    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
            {
            this->SynthConfig.Voice[z >> 1].SetSustainLevel (ch, val);
            this->pVoice[z]->SetSustainLevel (ch, val);
            }
        }
    DisplayMessage.OscSustainLevel (ch, data);
    }

//#######################################################################
void SYNTH_FRONT_C::ToggleRampDirection ()
    {
    this->VoiceSelectedCheck ();

    bool zb = ! this->SynthConfig.Voice[this->CurrentMapSelected].GetRampDirection ();
    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
            {
            this->SynthConfig.Voice[z >> 1].SetRampDirection (zb);
            this->pVoice[z]->SetRampDirection (zb);
            }
        }
    DisplayMessage.OscRampDirection (zb);
    }

//#######################################################################
void SYNTH_FRONT_C::SetPulseWidth (short data)
    {
    this->VoiceSelectedCheck ();

    float percent = data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
            {
            this->SynthConfig.Voice[z >> 1].SetPulseWidth (percent);
            this->pVoice[z]->SetPulseWidth (percent);
            }
        }
    DisplayMessage.OscPulseWidth (data);
    }



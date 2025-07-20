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
void SYNTH_FRONT_C::VoiceLevelSelect (short ch, bool state)
    {
    this->VoiceSelectedCheck ();
    state = !this->SynthConfig.Voice[this->CurrentVoiceSelected].SelectedEnvelope[ch];
    this->SynthConfig.Voice[this->CurrentVoiceSelected].SelectedEnvelope[ch] = state;
    this->LaunchControl.ButtonColor (ch, (state) ? XL_LED::RED : XL_LED::GREEN);
    }

//#####################################################################
void SYNTH_FRONT_C::SetLevel (short ch, short data)
    {
    if ( this->SetTuning )
        {
        this->TuningLevel[ch] = data * MIDI_MULTIPLIER;
        DisplayMessage.TuningLevel (ch, data);
        this->TuningChange = true;
        return;
        }

    if ( this->SynthConfig.Voice[this->CurrentVoiceSelected].SelectedEnvelope[ch] )
        {
        this->SetSustainLevel (ch,  data);;
        return;
        }

    this->VoiceSelectedCheck ();

    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
            {
            this->SynthConfig.Voice[z >> 1].SetLevel (ch, val);
            this->pVoice[z]->SetLevel (ch, val);
            }
        }

    DisplayMessage.OscMaxLevel (ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetAttackTime (short ch, short data)
    {
    this->VoiceSelectedCheck ();

    float dtime = data * TIME_MULT;

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

//#####################################################################
void SYNTH_FRONT_C::SetDecayTime (short ch, short data)
    {
    float dtime = data * TIME_MULT;

     this->VoiceSelectedCheck ();

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

//#####################################################################
void SYNTH_FRONT_C::SetReleaseTime (short ch, short data)
    {
    float dtime = data * TIME_MULT;

    this->VoiceSelectedCheck ();

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
void SYNTH_FRONT_C::ToggleRampDirection (short ch)
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
    this->LaunchControl.ButtonColor (ch, ( zb ) ? XL_LED::RED : XL_LED::GREEN);
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



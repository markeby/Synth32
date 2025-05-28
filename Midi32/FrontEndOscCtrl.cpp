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
    DisplayMessage.OscSelected (chan, this->SynthConfig.Voice[this->CurrentMapSelected].SelectedEnvelope[chan]);
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
            this->pVoice[z]->SetMaxLevel (ch, val);
        }

    this->SynthConfig.Voice[this->CurrentMapSelected].SetMaxLevel (ch, val);
    DisplayMessage.OscMaxLevel (ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetAttackTime (short data)
    {
    this->VoiceSelectedCheck ();

    float dtime = data * TIME_MULT;

    for ( int ch = 0;  ch < OSC_MIXER_COUNT;  ch++ )
        {
        if (this->SynthConfig.Voice[this->CurrentMapSelected].SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < VOICE_COUNT;  z++ )
                {
                if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
                    this->pVoice[z]->SetAttackTime (ch, dtime);
                }
            this->SynthConfig.Voice[this->CurrentMapSelected].SetAttackTime (ch, dtime);
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
        if ( this->SynthConfig.Voice[this->CurrentMapSelected].SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < VOICE_COUNT;  z++)
                {
                if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
                    this->pVoice[z]->SetDecayTime (ch, dtime);
                }
            this->SynthConfig.Voice[this->CurrentMapSelected].SetDecayTime (ch, dtime);
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
        if ( this->SynthConfig.Voice[this->CurrentMapSelected].SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < VOICE_COUNT;  z++)
                {
                if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
                    this->pVoice[z]->SetReleaseTime (ch, dtime);
                }
            this->SynthConfig.Voice[this->CurrentMapSelected].SetReleaseTime (ch, dtime);
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
            this->pVoice[z]->SetSustainLevel (ch, val);
        }
    this->SynthConfig.Voice[this->CurrentMapSelected].SetSustainLevel (ch, val);
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
            this->pVoice[z]->SetRampDirection (zb);
        }

    this->SynthConfig.Voice[this->CurrentMapSelected].SetRampDirection (zb);
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
            this->pVoice[z]->SetPulseWidth (percent);
        }
    this->SynthConfig.Voice[this->CurrentMidiSelected, this->CurrentMapSelected].SetPulseWidth (percent);
    DisplayMessage.OscPulseWidth (data);
    }



//#######################################################################
// Module:     SyntFrontOscCtl.cpp
// Descrption: Synthesizer oscillator controls
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
void SYNTH_FRONT_C::VoiceLevelSelect (short ch, bool state)
    {
    this->PageSelectedCheck ();
    state = !this->SynthConfig.Voice[this->CurrentVoiceSelected].SelectedOscEnvelope[ch];
    this->SynthConfig.Voice[this->CurrentVoiceSelected].SelectedOscEnvelope[ch] = state;
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

    if ( this->SynthConfig.Voice[this->CurrentVoiceSelected].SelectedOscEnvelope[ch] )
        {
        this->SetSustainLevel (ch,  data);;
        return;
        }

    this->PageSelectedCheck ();

    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
            {
            this->SynthConfig.Voice[z >> 1].SetOscMaxLevel (ch, val);
            this->pVoice[z]->SetMaxLevel (ch, val);
            }
        }

    DisplayMessage.OscMaxLevel (ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetAttackTime (short ch, short data)
    {
    this->PageSelectedCheck ();

    float dtime = data * TIME_MULT;

    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
            {
            if ( this->CurrentVoiceSelected < 0 )
                {
                if ( this->CurrentFilterSelected < 0 )
                    return;


                }
            else
                {
                this->SynthConfig.Voice[z >> 1].SetOscAttackTime (ch, dtime);
                this->pVoice[z]->SetOscAttackTime (ch, dtime);
                }
            }
        }
    DisplayMessage.OscAttackTime (ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetDecayTime (short ch, short data)
    {
    float dtime = data * TIME_MULT;

    this->PageSelectedCheck ();

    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
            {
            if ( this->CurrentVoiceSelected < 0 )
                {
                if ( this->CurrentFilterSelected < 0 )
                    return;


                }
            else
                {
                this->SynthConfig.Voice[z >> 1].SetOscDecayTime (ch, dtime);
                this->pVoice[z]->SetOscDecayTime (ch, dtime);
                }
            }
        }
    DisplayMessage.OscDecayTime (ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetReleaseTime (short ch, short data)
    {
    float dtime = data * TIME_MULT;

    this->PageSelectedCheck ();

    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
            {
            if (  this->CurrentVoiceSelected < 0 )
                {
                if ( this->CurrentFilterSelected < 0 )
                    return;


                }
            else
                {
                this->SynthConfig.Voice[z >> 1].SetOscReleaseTime(ch, dtime);
                this->pVoice[z]->SetOscReleaseTime (ch, dtime);
                }
            }
        }
    DisplayMessage.OscReleaseTime (ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetSustainLevel (short ch, short data)
    {
    this->PageSelectedCheck ();

    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
            {
            if ( this->CurrentVoiceSelected < 0 )
                {
                if ( this->CurrentFilterSelected < 0 )
                    return;


                }
            else
                {
                this->SynthConfig.Voice[z >> 1].SetOscSustainLevel (ch, val);
                this->pVoice[z]->SetOscSustainLevel (ch, val);
                }
            }
        }
    DisplayMessage.OscSustainLevel (ch, data);
    }

//#######################################################################
void SYNTH_FRONT_C::ToggleRampDirection (short ch)
    {
    if ( this->CurrentVoiceSelected < 0 )
        return;
    this->PageSelectedCheck ();

    bool zb = !this->SynthConfig.Voice[this->CurrentVoiceSelected].GetRampDirection();
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
    if ( this->CurrentVoiceSelected < 0 )
        return;
    this->PageSelectedCheck ();

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


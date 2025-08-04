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

using namespace DISP_MESSAGE_N;

//#######################################################################
void SYNTH_FRONT_C::UpdateOscDisplay ()
    {
    SYNTH_VOICE_CONFIG_C& sc = this->SynthConfig.Voice[this->CurrentMapSelected];    // Configuration data for this voice pair

    DisplayMessage.SetPage (PAGE_C::PAGE_OSC, this->CurrentMidiSelected);

    for ( short z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        DisplayMessage.OscAttackTime   (z, sc.GetOscAttackTime (z) * (1.0/TIME_MULT));
        DisplayMessage.OscDecayTime    (z, sc.GetOscDecayTime (z) * (1.0/TIME_MULT));
        DisplayMessage.OscReleaseTime  (z, sc.GetOscReleaseTime (z) * (1.0/TIME_MULT));
        DisplayMessage.OscSustainLevel (z, sc.GetOscSustainLevel (z) * PRS_UNSCALER);
        DisplayMessage.OscMaxLevel     (z, sc.GetOscMaxLevel (z) * PRS_UNSCALER);
        }
    DisplayMessage.OscRampDirection (sc.GetRampDirection ());
    DisplayMessage.OscPulseWidth   (sc.GetPulseWidth () * PRS_UNSCALER);
    this->TemplateSelect (XL_MIDI_MAP_OSC);
    }

//#######################################################################
void SYNTH_FRONT_C::UpdateFltDisplay ()
    {
    SYNTH_VOICE_CONFIG_C& sc = this->SynthConfig.Voice[this->CurrentMapSelected];    // Configuration data for this voice pair

    DisplayMessage.SetPage (PAGE_C::PAGE_FLT, this->CurrentMidiSelected);
    this->TemplateSelect (XL_MIDI_MAP_FLT);

    for ( short z = 0;  z < 2;  z++ )
        {
        DisplayMessage.FltAttackTime  (z, sc.GetFltAttackTime (z) * (1.0/TIME_MULT));
        DisplayMessage.FltDecayTime   (z, sc.GetFltDecayTime (z) * (1.0/TIME_MULT));
        DisplayMessage.FltReleaseTime (z, sc.GetFltReleaseTime (z) * (1.0/TIME_MULT));
        DisplayMessage.FltSustain     (z, sc.GetFltSustainLevel (z) * PRS_UNSCALER);
        DisplayMessage.FltStart       (z, sc.GetFltStart (z) * PRS_UNSCALER);
        DisplayMessage.FltEnd         (z, sc.GetFltEnd (z) * PRS_UNSCALER);
        }
    }

//#######################################################################
void SYNTH_FRONT_C::VoiceLevelSelect (short ch, bool state)
    {
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
    short chf = ch - 8;
    float dtime = data * TIME_MULT;

    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
            {
            if ( this->CurrentVoiceSelected < 0 )
                {
                if ( this->CurrentFilterSelected < 0 )
                    return;
                this->SynthConfig.Voice[z >> 1].SetFltAttackTime (chf, dtime);
                this->pVoice[z]->SetFltAttackTime (chf, dtime);
                }
            else
                {
                this->SynthConfig.Voice[z >> 1].SetOscAttackTime (ch, dtime);
                this->pVoice[z]->SetOscAttackTime (ch, dtime);
                }
            }
        }
    if ( this->CurrentVoiceSelected < 0 )
        DisplayMessage.FltAttackTime (chf, data);
    else
        DisplayMessage.OscAttackTime (ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetDecayTime (short ch, short data)
    {
    short chf = ch - 8;
    float dtime = data * TIME_MULT;

    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
            {
            if ( this->CurrentVoiceSelected < 0 )
                {
                if ( this->CurrentFilterSelected < 0 )
                    return;
                this->SynthConfig.Voice[z >> 1].SetFltDecayTime (chf, dtime);
                this->pVoice[z]->SetFltDecayTime (chf, dtime);
                }
            else
                {
                this->SynthConfig.Voice[z >> 1].SetOscDecayTime (ch, dtime);
                this->pVoice[z]->SetOscDecayTime (ch, dtime);
                }
            }
        }
    if ( this->CurrentVoiceSelected < 0 )
        DisplayMessage.FltDecayTime (chf, data);
    else
        DisplayMessage.OscDecayTime (ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetReleaseTime (short ch, short data)
    {
    short chf = ch - 8;
    float dtime = data * TIME_MULT;

    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
            {
            if (  this->CurrentVoiceSelected < 0 )
                {
                if ( this->CurrentFilterSelected < 0 )
                    return;
                this->SynthConfig.Voice[z >> 1].SetFltReleaseTime (chf, dtime);
                this->pVoice[z]->SetFltReleaseTime (chf, dtime);
                }
            else
                {
                this->SynthConfig.Voice[z >> 1].SetOscReleaseTime(ch, dtime);
                this->pVoice[z]->SetOscReleaseTime (ch, dtime);
                }
            }
        }
    if ( this->CurrentVoiceSelected < 0 )
        DisplayMessage.FltReleaseTime (chf, data);
    else
        DisplayMessage.OscReleaseTime (ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetSustainLevel (short ch, short data)
    {
    short chf = ch - 8;
    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
            {
            if ( this->CurrentVoiceSelected < 0 )
                {
                if ( this->CurrentFilterSelected < 0 )
                    return;
                this->SynthConfig.Voice[z >> 1].SetFltSustainLevel (chf, val);
                this->pVoice[z]->SetFltSustain (chf, val);
                }
            else
                {
                this->SynthConfig.Voice[z >> 1].SetOscSustainLevel (ch, val);
                this->pVoice[z]->SetOscSustainLevel (ch, val);
                }
            }
        }
    if ( this->CurrentVoiceSelected < 0 )
        DisplayMessage.FltSustain (chf, data);
    else
        DisplayMessage.OscSustainLevel (ch, data);
    }

//#######################################################################
void SYNTH_FRONT_C::ToggleRampDirection (short ch)
    {
    if ( this->CurrentVoiceSelected < 0 )
        return;

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

//#######################################################################
void SYNTH_FRONT_C::FltStart (short ch, short data)
    {
    ch -= 8;
    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
            {
            if ( this->CurrentFilterSelected < 0 )
                return;
            this->SynthConfig.Voice[z >> 1].SetFltStart (ch, val);
            this->pVoice[z]->SetFltStart (ch, val);
            }
        }
    DisplayMessage.FltStart (ch, data);
    }

//#######################################################################
void SYNTH_FRONT_C::FltEnd (short ch, short data)
    {
    ch -= 8;
    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( this->CurrentMidiSelected == this->pVoice[z]->GetMidi () )
            {
            if ( this->CurrentFilterSelected < 0 )
                return;
            this->SynthConfig.Voice[z >> 1].SetFltEnd (ch, val);
            this->pVoice[z]->SetFltEnd (ch, val);
            }
        }
    DisplayMessage.FltEnd (ch, data);
    }


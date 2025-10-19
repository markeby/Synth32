//#######################################################################
// Module:     FrontEndVoice.cpp
// Descrption: Synthesizer front end control for oscillators
// Creator:    markeby
// Date:       10/17/2025
//#######################################################################
#include <Arduino.h>
#include <UHS2-MIDI.h>
#include <MIDI.h>

#include "Config.h"
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
void updateOscButtons ()
    {
    SYNTH_VOICE_CONFIG_C& sc = SynthConfig.Voice[CurrentConfigSelected];    // Configuration data for this voice pair

    bool zb = sc.GetRampDirection ();
    DisplayMessage.OscRampDirection (zb);
    sc.SetButtonStateOsc (7, ( zb ) ? (byte)XL_LED::RED : (byte)XL_LED::GREEN);
    sc.SetButtonStateOsc (15, ( sc.IsMute () ) ? (byte)XL_LED::RED : (byte)XL_LED::GREEN);
    LaunchControl.TemplateRefresh ();
    }

//#######################################################################
void UpdateOscDisplay ()
    {
    SYNTH_VOICE_CONFIG_C& sc = SynthConfig.Voice[CurrentConfigSelected];    // Configuration data for this voice pair

    DisplayMessage.SetPage (PAGE_C::PAGE_OSC, CurrentMidiSelected);

    for ( short z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        DisplayMessage.OscAttackTime    (z, sc.GetOscAttackTime (z) * (1.0/TIME_MULT));
        DisplayMessage.OscDecayTime     (z, sc.GetOscDecayTime (z) * (1.0/TIME_MULT));
        DisplayMessage.OscReleaseTime   (z, sc.GetOscReleaseTime (z) * (1.0/TIME_MULT));
        DisplayMessage.OscSustainLevel  (z, sc.GetOscSustainLevel (z) * PRS_UNSCALER);
        DisplayMessage.OscMaxLevel      (z, sc.GetOscMaxLevel (z) * PRS_UNSCALER);
        DisplayMessage.OscDamperMode    (z, sc.GetDamperMode (z));
        SynthConfig.Voice[CurrentConfigSelected].SetButtonStateOsc (z + 8, ( sc.GetDamperMode (z) ) ? (byte)XL_LED::RED : (byte)XL_LED::GREEN);
        DisplayMessage.OscSelectedLevel (z, sc.SelectedOscEnvelope[z]);
        }
    DisplayMessage.OscPulseWidth   (sc.GetPulseWidth () * PRS_UNSCALER);
    updateOscButtons ();
    TemplateSelect (XL_MIDI_MAP_OSC);
    }

//#######################################################################
void updateFltButtons ()
    {
    byte mask = SynthConfig.Voice[CurrentConfigSelected].GetOutputMask ();
    for ( short z = 0;  z < 5;  z++ )
        SynthConfig.Voice[CurrentConfigSelected].SetButtonStateFlt (z + 8, ((mask >> z) & 1 ) ? (byte)XL_LED::RED : (byte)XL_LED::GREEN);
    LaunchControl.TemplateRefresh ();
    }

//#######################################################################
void UpdateFltDisplay ()
    {
    SYNTH_VOICE_CONFIG_C& sc = SynthConfig.Voice[CurrentConfigSelected];    // Configuration data for this voice pair

    DisplayMessage.SetPage (PAGE_C::PAGE_FLT, CurrentMidiSelected);
    for ( short z = 0;  z < FILTER_DEVICES;  z++ )
        {
        DisplayMessage.FltAttackTime  (z, sc.GetFltAttackTime (z) * (1.0/TIME_MULT));
        DisplayMessage.FltDecayTime   (z, sc.GetFltDecayTime (z) * (1.0/TIME_MULT));
        DisplayMessage.FltReleaseTime (z, sc.GetFltReleaseTime (z) * (1.0/TIME_MULT));
        DisplayMessage.FltSustain     (z, sc.GetFltSustainLevel (z) * PRS_UNSCALER);
        DisplayMessage.FltStart       (z, sc.GetFltStart (z) * PRS_UNSCALER);
        DisplayMessage.FltEnd         (z, sc.GetFltEnd (z) * PRS_UNSCALER);
        DisplayMessage.FltCtrl        (z, sc.GetFltCtrl (z));
        DisplayMessage.FltOut         (sc.GetOutputMask ());
        }
    TemplateSelect (XL_MIDI_MAP_FLT);
    updateFltButtons ();
    }

//#######################################################################
void VoiceLevelSelect (short ch, bool state)
    {
    state = !SynthConfig.Voice[CurrentConfigSelected].SelectedOscEnvelope[ch];
    SynthConfig.Voice[CurrentConfigSelected].SelectedOscEnvelope[ch] = state;
    DisplayMessage.OscSelectedLevel (ch, state);
    SynthConfig.Voice[CurrentConfigSelected].SetButtonStateOsc (ch, ( state ) ? (byte)XL_LED::RED : (byte)XL_LED::GREEN);
    LaunchControl.TemplateRefresh ();
    }

//#######################################################################
void VoiceDamperToggle (short ch)
    {
    bool zb = !SynthConfig.Voice[CurrentConfigSelected].GetDamperMode (ch);
    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( CurrentMidiSelected == VoiceArray[z]->GetMidi () )
            {
            SynthConfig.Voice[z >> 1].SetDamperMode (ch, zb);
            VoiceArray[z]->SetDamperMode (ch, zb);
            DisplayMessage.OscDamperMode (ch, zb);
            }
        }
    SynthConfig.Voice[CurrentConfigSelected].SetButtonStateOsc (ch + 8, ( zb ) ? (byte)XL_LED::AMBER : (byte)XL_LED::GREEN);
    LaunchControl.TemplateRefresh ();
    }

//#######################################################################
void MuteVoicesReset ()
    {
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        SynthConfig.Voice[z >> 1].SetMute (false);
        VoiceArray[z]->Mute (false);
        }
    }

//#######################################################################
void MuteVoiceToggle ()
    {
    bool state = !SynthConfig.Voice[CurrentConfigSelected].IsMute ();
    SynthConfig.Voice[CurrentConfigSelected].SetMute (state);

    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( CurrentMidiSelected == VoiceArray[z]->GetMidi () )
            VoiceArray[z]->Mute (state);
        }
    DisplayMessage.OscMute (state);
    SynthConfig.Voice[CurrentConfigSelected].SetButtonStateOsc (15, ( state ) ? (byte)XL_LED::RED : (byte)XL_LED::GREEN);
    updateOscButtons ();
    }

//#####################################################################
void SetLevel (short ch, short data)
    {
    if ( SynthConfig.Voice[CurrentConfigSelected].SelectedOscEnvelope[ch] )
        {
        SetSustainLevel (ch,  data);;
        return;
        }

    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( CurrentMidiSelected == VoiceArray[z]->GetMidi () )
            {
            SynthConfig.Voice[z >> 1].SetOscMaxLevel (ch, val);
            VoiceArray[z]->SetMaxLevel (ch, val);
            }
        }

    DisplayMessage.OscMaxLevel (ch, data);
    }

//#####################################################################
void SetAttackTime (short ch, short data)
    {
    short chf = ch - 8;
    float dtime = data * TIME_MULT;

    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( CurrentMidiSelected == VoiceArray[z]->GetMidi () )
            {
            if ( CurrentVoiceSelected < 0 )
                {
                if ( CurrentFilterSelected < 0 )
                    return;
                SynthConfig.Voice[z >> 1].SetFltAttackTime (chf, dtime);
                VoiceArray[z]->SetFltAttackTime (chf, dtime);
                }
            else
                {
                SynthConfig.Voice[z >> 1].SetOscAttackTime (ch, dtime);
                VoiceArray[z]->SetOscAttackTime (ch, dtime);
                }
            }
        }
    if ( CurrentVoiceSelected < 0 )
        DisplayMessage.FltAttackTime (chf, data);
    else
        DisplayMessage.OscAttackTime (ch, data);
    }

//#####################################################################
void SetDecayTime (short ch, short data)
    {
    short chf = ch - 8;
    float dtime = data * TIME_MULT;

    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( CurrentMidiSelected == VoiceArray[z]->GetMidi () )
            {
            if ( CurrentVoiceSelected < 0 )
                {
                if ( CurrentFilterSelected < 0 )
                    return;
                SynthConfig.Voice[z >> 1].SetFltDecayTime (chf, dtime);
                VoiceArray[z]->SetFltDecayTime (chf, dtime);
                }
            else
                {
                SynthConfig.Voice[z >> 1].SetOscDecayTime (ch, dtime);
                VoiceArray[z]->SetOscDecayTime (ch, dtime);
                }
            }
        }
    if ( CurrentVoiceSelected < 0 )
        DisplayMessage.FltDecayTime (chf, data);
    else
        DisplayMessage.OscDecayTime (ch, data);
    }

//#####################################################################
void SetReleaseTime (short ch, short data)
    {
    short chf = ch - 8;
    float dtime = data * TIME_MULT;

    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( CurrentMidiSelected == VoiceArray[z]->GetMidi () )
            {
            if (  CurrentVoiceSelected < 0 )
                {
                if ( CurrentFilterSelected < 0 )
                    return;
                SynthConfig.Voice[z >> 1].SetFltReleaseTime (chf, dtime);
                VoiceArray[z]->SetFltReleaseTime (chf, dtime);
                }
            else
                {
                SynthConfig.Voice[z >> 1].SetOscReleaseTime(ch, dtime);
                VoiceArray[z]->SetOscReleaseTime (ch, dtime);
                }
            }
        }
    if ( CurrentVoiceSelected < 0 )
        DisplayMessage.FltReleaseTime (chf, data);
    else
        DisplayMessage.OscReleaseTime (ch, data);
    }

//#####################################################################
void SetSustainLevel (short ch, short data)
    {
    short chf = ch - 8;
    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( CurrentMidiSelected == VoiceArray[z]->GetMidi () )
            {
            if ( CurrentVoiceSelected < 0 )
                {
                if ( CurrentFilterSelected < 0 )
                    return;
                SynthConfig.Voice[z >> 1].SetFltSustainLevel (chf, val);
                VoiceArray[z]->SetFltSustain (chf, val);
                }
            else
                {
                SynthConfig.Voice[z >> 1].SetOscSustainLevel (ch, val);
                VoiceArray[z]->SetOscSustainLevel (ch, val);
                }
            }
        }
    if ( CurrentVoiceSelected < 0 )
        DisplayMessage.FltSustain (chf, data);
    else
        DisplayMessage.OscSustainLevel (ch, data);
    }

//#######################################################################
void ToggleRampDirection (short ch)
    {
    bool zb = !SynthConfig.Voice[CurrentConfigSelected].GetRampDirection ();
    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( CurrentMidiSelected == VoiceArray[z]->GetMidi () )
            {
            SynthConfig.Voice[z >> 1].SetRampDirection (zb);
            VoiceArray[z]->SetRampDirection (zb);
            }
        }
    updateOscButtons ();
    }

//#######################################################################
void SetPulseWidth (short data)
    {
    float percent = data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( CurrentMidiSelected == VoiceArray[z]->GetMidi () )
            {
            SynthConfig.Voice[z >> 1].SetPulseWidth (percent);
            VoiceArray[z]->SetPulseWidth (percent);
            }
        }
    DisplayMessage.OscPulseWidth (data);
    }

//#######################################################################
void FltStart (short ch, short data)
    {
    ch -= 8;
    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( CurrentMidiSelected == VoiceArray[z]->GetMidi () )
            {
            SynthConfig.Voice[z >> 1].SetFltStart (ch, val);
            VoiceArray[z]->SetFltStart (ch, val);
            }
        }
    DisplayMessage.FltStart (ch, data);
    }

//#######################################################################
void FltEnd (short ch, short data)
    {
    ch -= 8;
    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( CurrentMidiSelected == VoiceArray[z]->GetMidi () )
            {
            SynthConfig.Voice[z >> 1].SetFltEnd (ch, val);
            VoiceArray[z]->SetFltEnd (ch, val);
            }
        }
    DisplayMessage.FltEnd (ch, data);
    }

//#######################################################################
void SelectFilter (short index)
    {
    byte mask = 1 << index;                                                         // setup masking for the selected bit
    mask ^= SynthConfig.Voice[CurrentConfigSelected].GetOutputMask ();        // Setup final output inverting selected bit

    for ( short z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( CurrentMidiSelected == VoiceArray[z]->GetMidi () )
            {
            SynthConfig.Voice[z >> 1].SetOutputMask (mask);
            VoiceArray[z]->SetOutputMask (mask);
            }
        }
    DisplayMessage.FltOut (mask);
    updateFltButtons ();
    }

//#######################################################################
void FreqCtrlModeAdv (short index)
    {
    byte zb = (SynthConfig.Voice[CurrentConfigSelected].GetFltCtrl (index) + 1) % FILTER_CONTROLS;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( CurrentMidiSelected == VoiceArray[z]->GetMidi () )
            {
            SynthConfig.Voice[z >> 1].SetFltCtrl (index, zb);
            VoiceArray[z]->SetFltCtrl (index, zb);
            }
        }
    DisplayMessage.FltCtrl (index, zb);
    updateFltButtons ();
    }



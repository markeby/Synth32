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
static void updateOscButtons ()
    {
    SYNTH_VOICE_CONFIG_C& sc = SynthConfig.Voice[SelectedConfig];    // Configuration data for this voice pair

    bool zb = sc.GetRampDirection ();
    DisplayMessage.OscRampDirection (zb);
    LaunchControl.SetButtonStateAG (7, zb);
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        LaunchControl.SetButtonStateAG (z, sc.SelectedOscEnvelope[z]);
        LaunchControl.SetColorTri (z + 8, sc.GetDamperMode (z));
        }
    LaunchControl.SetButtonStateRG (15, sc.IsMute ());
    LaunchControl.TemplateRefresh ();
    }

//#######################################################################
static void updateFltButtons ()
    {
    SYNTH_VOICE_CONFIG_C& sc = SynthConfig.Voice[SelectedConfig];    // Configuration data for this voice pair

    byte mask = SynthConfig.Voice[SelectedConfig].GetOutputMask ();
    for ( short z = 0;  z < 5;  z++ )
        LaunchControl.SetButtonStateRG (z + 8, (mask >> z) & 1);

    LaunchControl.TemplateRefresh ();
    }

//#######################################################################
void StartOscDisplay ()
    {
    SYNTH_VOICE_CONFIG_C& sc = SynthConfig.Voice[SelectedConfig];    // Configuration data for this voice pair

    DisplayMessage.SetPage (PAGE_C::PAGE_OSC, SelectedMidi);

    LaunchControl.SelectTemplate (XL_MIDI_MAP_OSC);
    updateOscButtons ();

    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        DisplayMessage.OscAttackTime    (z, sc.GetOscAttackTime (z) * (1.0/TIME_MULT));
        DisplayMessage.OscDecayTime     (z, sc.GetOscDecayTime (z) * (1.0/TIME_MULT));
        DisplayMessage.OscReleaseTime   (z, sc.GetOscReleaseTime (z) * (1.0/TIME_MULT));
        DisplayMessage.OscSustainLevel  (z, sc.GetOscSustainLevel (z) * PRS_UNSCALER);
        DisplayMessage.OscMaxLevel      (z, sc.GetOscMaxLevel (z) * PRS_UNSCALER);
        DisplayMessage.OscDamperMode    (z, sc.GetDamperMode (z));
        DisplayMessage.OscSelectedLevel (z, sc.SelectedOscEnvelope[z]);
        }
    DisplayMessage.OscPulseWidth   (sc.GetPulseWidth () * PRS_UNSCALER);
    DisplayMessage.MasterLevel   (sc.GetMasterLevel () * PRS_UNSCALER);
    }

//#######################################################################
void StartFltDisplay ()
    {
    SYNTH_VOICE_CONFIG_C& sc = SynthConfig.Voice[SelectedConfig];    // Configuration data for this voice pair

    DisplayMessage.SetPage (PAGE_C::PAGE_FLT, SelectedMidi);
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
    LaunchControl.SelectTemplate (XL_MIDI_MAP_FLT);
    updateFltButtons ();
    }

//#######################################################################
void VoiceLevelSelect (short ch, bool state)
    {
    state = !SynthConfig.Voice[SelectedConfig].SelectedOscEnvelope[ch];
    SynthConfig.Voice[SelectedConfig].SelectedOscEnvelope[ch] = state;
    DisplayMessage.OscSelectedLevel (ch, state);
    LaunchControl.SetButtonStateAG (ch, state);
    LaunchControl.TemplateRefresh ();
    }

//#######################################################################
void VoiceDamperToggle (short index)
    {
    byte zb = (SynthConfig.Voice[SelectedConfig].GetDamperMode (index) + 1) % (byte)DAMPER::MAX;
    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( SelectedMidi == VoiceArray[z]->GetMidi () )
            {
            SynthConfig.Voice[z >> 1].SetDamperMode (index, zb);
            VoiceArray[z]->SetDamperMode (index, (DAMPER)zb);
            DisplayMessage.OscDamperMode (index, zb);
            }
        }
    LaunchControl.SetColorTri (index + 8, zb);
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
    bool state = !SynthConfig.Voice[SelectedConfig].IsMute ();
    SynthConfig.Voice[SelectedConfig].SetMute (state);

    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( SelectedMidi == VoiceArray[z]->GetMidi () )
            VoiceArray[z]->Mute (state);
        }
    DisplayMessage.OscMute (state);
    updateOscButtons ();
    }

//#####################################################################
void SetMasterLevel (short ch, short data)
    {
    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( SelectedMidi == VoiceArray[z]->GetMidi () )
            {
            SynthConfig.Voice[z >> 1].SetMasterLevel (val);
            VoiceArray[z]->Expression (val);
            }
        }

    DisplayMessage.MasterLevel (data);
    }

//#####################################################################
void SetLevel (short ch, short data)
    {
    if ( SynthConfig.Voice[SelectedConfig].SelectedOscEnvelope[ch] )
        {
        SetSustainLevel (ch,  data);;
        return;
        }

    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( SelectedMidi == VoiceArray[z]->GetMidi () )
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
        if ( SelectedMidi == VoiceArray[z]->GetMidi () )
            {
            if ( SelectedVoice < 0 )
                {
                if ( SelectedFilter < 0 )
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
    if ( SelectedVoice < 0 )
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
        if ( SelectedMidi == VoiceArray[z]->GetMidi () )
            {
            if ( SelectedVoice < 0 )
                {
                if ( SelectedFilter < 0 )
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
    if ( SelectedVoice < 0 )
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
        if ( SelectedMidi == VoiceArray[z]->GetMidi () )
            {
            if (  SelectedVoice < 0 )
                {
                if ( SelectedFilter < 0 )
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
    if ( SelectedVoice < 0 )
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
        if ( SelectedMidi == VoiceArray[z]->GetMidi () )
            {
            if ( SelectedVoice < 0 )
                {
                if ( SelectedFilter < 0 )
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
    if ( SelectedVoice < 0 )
        DisplayMessage.FltSustain (chf, data);
    else
        DisplayMessage.OscSustainLevel (ch, data);
    }

//#######################################################################
void ToggleRampDirection (short ch, short data)
    {
    bool zb = !SynthConfig.Voice[SelectedConfig].GetRampDirection ();
    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( SelectedMidi == VoiceArray[z]->GetMidi () )
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
        if ( SelectedMidi == VoiceArray[z]->GetMidi () )
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
        if ( SelectedMidi == VoiceArray[z]->GetMidi () )
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
        if ( SelectedMidi == VoiceArray[z]->GetMidi () )
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
    mask ^= SynthConfig.Voice[SelectedConfig].GetOutputMask ();        // Setup final output inverting selected bit

    for ( short z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( SelectedMidi == VoiceArray[z]->GetMidi () )
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
    byte zb = (SynthConfig.Voice[SelectedConfig].GetFltCtrl (index) + 1) % FILTER_CONTROLS;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        if ( SelectedMidi == VoiceArray[z]->GetMidi () )
            {
            SynthConfig.Voice[z >> 1].SetFltCtrl (index, zb);
            VoiceArray[z]->SetFltCtrl (index, zb);
            }
        }
    DisplayMessage.FltCtrl (index, zb);
    updateFltButtons ();
    }

//#######################################################################
void Damper (byte mchan, bool state)
    {
    for ( int z = 0;  z < VOICE_COUNT;  z++)
        VoiceArray[z]->Damper (mchan, state);
    }



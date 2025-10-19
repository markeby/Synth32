//#######################################################################
// Module:     Control_LFO.cpp
// Descrption: Synthesizer front end control for LFO
// Creator:    markeby
// Date:       10/17/2025
//#######################################################################
#include <Arduino.h>
#include <UHS2-MIDI.h>

#include "Config.h"
#include "I2Cmessages.h"
#include "Osc.h"
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
void UpdateLfoDisplay ()
    {
    SYNTH_VOICE_CONFIG_C& sc = SynthConfig.Voice[CurrentMapSelected];    // Configuration data for this voice pair

    DisplayMessage.Page (PAGE_C::PAGE_MOD);

    for ( int z = 0;  z < 2;  z++ )
        {
        DisplayMessage.LfoHardFreq       (z, SynthConfig.GetFrequency (z));
        DisplayMessage.LfoHardPulseWidth (z, SynthConfig.GetPulseWidth (z));
        DisplayMessage.LfoHardRampSlope  (z, SynthConfig.GetRampDir (z));
        DisplayMessage.SetModLevelAlt    (z, SynthConfig.GetModLevelAlt (z));
        }
    DisplayMessage.LfoSoftFreq (SynthConfig.GetSoftFreq ());
    UpdateModButtons ();
    TemplateSelect (XL_MIDI_MAP_LFO);
    }

//#####################################################################
void UpdateModButtons ()
    {
    bool state;
    byte* pb = SynthConfig.GetButtonStateLfoPtr ();

    for (int idx = 0;  idx < 2;  idx++ )
        {
        for ( int z = 0;  z < SOURCE_CNT_LFO;  z++ )
            {
            state = SynthConfig.GetSelect(idx, z);
            int zf =  ( idx ) ? z + 4 : z;
            pb[zf] = ( state ) ? (byte)XL_LED::RED : (byte)XL_LED::GREEN;
            }
        state = SynthConfig.GetRampDir (idx);
        short zf = ( idx ) ? 7 : 3;
        pb[zf] = ( state ) ? (byte)XL_LED::RED : (byte)XL_LED::GREEN;

        bool state = SynthConfig.GetModLevelAlt (idx);
        pb[idx + 14] = (state) ? (byte)XL_LED::RED : (byte)XL_LED::GREEN;
        }

    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++)
        {
        bool state = SynthConfig.GetModSoftMixer (z);
        pb[z + 8] = ( state ) ? (byte)XL_LED::RED : (byte)XL_LED::GREEN;
        }

    LaunchControl.TemplateRefresh ();
    }

//#######################################################################
void FreqLFO (short ch, short data)
    {
    switch ( ch )
        {
        case 0:     // LFO #0 coarse
            Lfo[0].SetFreqCoarse      (data);
            SynthConfig.SetFrequency (0, Lfo[0].GetFreq ());
            DisplayMessage.LfoHardFreq     (0, Lfo[0].GetFreq ());
            break;
        case 1:     // LFO #0 fine
            Lfo[0].SetFreqFine      (data);
            SynthConfig.SetFrequency (0, Lfo[0].GetFreq ());
            DisplayMessage.LfoHardFreq     (0, Lfo[0].GetFreq ());
            break;
        case 2:
            data = (short)((float)data * MIDI_MULTIPLIER);
            Lfo[0].SetPulseWidth       (data);
            SynthConfig.SetPulseWidth  (0, data);
            DisplayMessage.LfoHardPulseWidth (0, data);
            break;
        case 3:     // LFO #1 coarse
            Lfo[1].SetFreqCoarse      (data);
            SynthConfig.SetFrequency (1, Lfo[1].GetFreq ());
            DisplayMessage.LfoHardFreq     (1, Lfo[1].GetFreq ());
            break;
        case 4:     // LFO #1 fine
            Lfo[1].SetFreqFine      (data);
            SynthConfig.SetFrequency (1, Lfo[1].GetFreq ());
            DisplayMessage.LfoHardFreq     (1, Lfo[1].GetFreq ());
            break;
        case 5:
            data = (short)((float)data * MIDI_MULTIPLIER);
            Lfo[1].SetPulseWidth       (data);
            SynthConfig.SetPulseWidth  (1, data);
            DisplayMessage.LfoHardPulseWidth (1, data);
            break;
        case 6:     // SoftLFO coarse
            SoftLFO.SetFreqCoarse         (data);
            SynthConfig.SetSoftFreq (SoftLFO.GetFreq ());
            DisplayMessage.LfoSoftFreq    (SoftLFO.GetFreq ());
            break;
        case 7:     // SoftLFO fine
            SoftLFO.SetFreqFine           (data);
            SynthConfig.SetSoftFreq (SoftLFO.GetFreq ());
            DisplayMessage.LfoSoftFreq    (SoftLFO.GetFreq ());
            break;
        }
    }

//#######################################################################
void PitchBender (byte mchan, short value)
    {
    value = (value + 8192) >> 2;
    Lfo[0].PitchBend (mchan, value);
    Lfo[1].PitchBend (mchan, value);
    DBG ("Pitch Bend value = %d", value);
    }

//#####################################################################
void SelectModVCA (byte ch, bool state)
    {
    SynthConfig.SetModSoftMixer (ch, state);

    DisplayMessage.LfoSoftSelect (ch, state);

    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( VoiceArray[z]->GetMidi () == SoftLFO.GetMidi () )
            VoiceArray[z]->SetSoftLFO (ch, state);
        else
            VoiceArray[z]->SetSoftLFO (ch, false);
        }
    UpdateModButtons ();
    }

//#####################################################################
void SelectModVCA (byte ch)
    {
    SelectModVCA (ch, !SynthConfig.GetModSoftMixer (ch));
    }

//#####################################################################
void SelectModVCO (short index, short ch)
    {
    SelectModVCO (index, ch, !Lfo[index].GetWave(ch));
    }

//#####################################################################
void SelectModVCO (short index, short ch, bool state)
    {
    SynthConfig.SetSelect (index, ch, state);
    Lfo[index].SetWave (ch, state);
    DisplayMessage.LfoHardSelect (index, ch, state);
    UpdateModButtons ();
    }

//#####################################################################
void ToggleModRampDir (short index)
    {
    SetModRampDir (index, !Lfo[index].GetRampDir ());
    }

//#####################################################################
void SetModRampDir (short index, bool state)
    {
    SynthConfig.SetRampDir (index, state);
    Lfo[index].SetRampDir (state);
    DisplayMessage.LfoHardRampSlope (index, state);
    UpdateModButtons ();
    }

//#####################################################################
void ToggleModLevelAlt  (short index)
    {
    SetModLevelAlt (index, !Lfo[index].GetModLevelAlt ());
    }

//#####################################################################
void SetModLevelAlt  (short index, bool state)
    {
    SynthConfig.SetModLevelAlt (index, state);
    Lfo[index].SetModLevelAlt (state);
    DisplayMessage.SetModLevelAlt (index, state);
    UpdateModButtons ();
    }



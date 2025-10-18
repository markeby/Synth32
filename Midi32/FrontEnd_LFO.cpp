//#######################################################################
// Module:     FrontEndLFO.cpp
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
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
void SYNTH_FRONT_C::UpdateLfoDisplay ()
    {
    SYNTH_VOICE_CONFIG_C& sc = this->SynthConfig.Voice[this->CurrentMapSelected];    // Configuration data for this voice pair

    DisplayMessage.Page (PAGE_C::PAGE_MOD);

    for ( int z = 0;  z < 2;  z++ )
        {
        DisplayMessage.LfoHardFreq       (z, this->SynthConfig.GetFrequency (z));
        DisplayMessage.LfoHardPulseWidth (z, this->SynthConfig.GetPulseWidth (z));
        DisplayMessage.LfoHardRampSlope  (z, this->SynthConfig.GetRampDir (z));
        DisplayMessage.SetModLevelAlt    (z, this->SynthConfig.GetModLevelAlt (z));
        }
    DisplayMessage.LfoSoftFreq (this->SynthConfig.GetSoftFreq ());
    this->UpdateModButtons ();
    this->TemplateSelect (XL_MIDI_MAP_LFO);
    }

//#####################################################################
void SYNTH_FRONT_C::UpdateModButtons ()
    {
    bool state;
    byte* pb = this->SynthConfig.GetButtonStateLfoPtr ();

    for (int idx = 0;  idx < 2;  idx++ )
        {
        for ( int z = 0;  z < SOURCE_CNT_LFO;  z++ )
            {
            state = this->SynthConfig.GetSelect(idx, z);
            int zf =  ( idx ) ? z + 4 : z;
            pb[zf] = ( state ) ? (byte)XL_LED::RED : (byte)XL_LED::GREEN;
            }
        state = this->SynthConfig.GetRampDir (idx);
        short zf = ( idx ) ? 7 : 3;
        pb[zf] = ( state ) ? (byte)XL_LED::RED : (byte)XL_LED::GREEN;

        bool state = this->SynthConfig.GetModLevelAlt (idx);
        pb[idx + 14] = (state) ? (byte)XL_LED::RED : (byte)XL_LED::GREEN;
        }

    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++)
        {
        bool state = this->SynthConfig.GetModSoftMixer (z);
        pb[z + 8] = ( state ) ? (byte)XL_LED::RED : (byte)XL_LED::GREEN;
        }

    this->LaunchControl.TemplateRefresh ();
    }

//#######################################################################
void SYNTH_FRONT_C::FreqLFO (short ch, short data)
    {
    switch ( ch )
        {
        case 0:     // LFO #0 coarse
            this->Lfo[0].SetFreqCoarse      (data);
            this->SynthConfig.SetFrequency (0, this->Lfo[0].GetFreq ());
            DisplayMessage.LfoHardFreq     (0, this->Lfo[0].GetFreq ());
            break;
        case 1:     // LFO #0 fine
            this->Lfo[0].SetFreqFine      (data);
            this->SynthConfig.SetFrequency (0, this->Lfo[0].GetFreq ());
            DisplayMessage.LfoHardFreq     (0, this->Lfo[0].GetFreq ());
            break;
        case 2:
            data = (short)((float)data * MIDI_MULTIPLIER);
            this->Lfo[0].SetPulseWidth       (data);
            this->SynthConfig.SetPulseWidth  (0, data);
            DisplayMessage.LfoHardPulseWidth (0, data);
            break;
        case 3:     // LFO #1 coarse
            this->Lfo[1].SetFreqCoarse      (data);
            this->SynthConfig.SetFrequency (1, this->Lfo[1].GetFreq ());
            DisplayMessage.LfoHardFreq     (1, this->Lfo[1].GetFreq ());
            break;
        case 4:     // LFO #1 fine
            this->Lfo[1].SetFreqFine      (data);
            this->SynthConfig.SetFrequency (1, this->Lfo[1].GetFreq ());
            DisplayMessage.LfoHardFreq     (1, this->Lfo[1].GetFreq ());
            break;
        case 5:
            data = (short)((float)data * MIDI_MULTIPLIER);
            this->Lfo[1].SetPulseWidth       (data);
            this->SynthConfig.SetPulseWidth  (1, data);
            DisplayMessage.LfoHardPulseWidth (1, data);
            break;
        case 6:     // SoftLFO coarse
            SoftLFO.SetFreqCoarse         (data);
            this->SynthConfig.SetSoftFreq (SoftLFO.GetFreq ());
            DisplayMessage.LfoSoftFreq    (SoftLFO.GetFreq ());
            break;
        case 7:     // SoftLFO fine
            SoftLFO.SetFreqFine           (data);
            this->SynthConfig.SetSoftFreq (SoftLFO.GetFreq ());
            DisplayMessage.LfoSoftFreq    (SoftLFO.GetFreq ());
            break;
        }
    }

//#######################################################################
void SYNTH_FRONT_C::PitchBend (byte mchan, short value)
    {
    this->Lfo[0].PitchBend (mchan, value);
    this->Lfo[1].PitchBend (mchan, value);
    DBG ("Pitch Bend value = %d", value);
    }

//#####################################################################
void SYNTH_FRONT_C::SelectModVCA (byte ch, bool state)
    {
    this->SynthConfig.SetModSoftMixer (ch, state);

    DisplayMessage.LfoSoftSelect (ch, state);

    for ( int z = 0;  z < VOICE_COUNT;  z++)
        {
        if ( this->pVoice[z]->GetMidi () == SoftLFO.GetMidi () )
            this->pVoice[z]->SetSoftLFO (ch, state);
        else
            this->pVoice[z]->SetSoftLFO (ch, false);
        }
    this->UpdateModButtons ();
    }

//#####################################################################
void SYNTH_FRONT_C::SelectModVCA (byte ch)
    {
    this->SelectModVCA (ch, !this->SynthConfig.GetModSoftMixer (ch));
    }

//#####################################################################
void SYNTH_FRONT_C::SelectModVCO (short index, short ch)
    {
    this->SelectModVCO (index, ch, !this->Lfo[index].GetWave(ch));
    }

//#####################################################################
void SYNTH_FRONT_C::SelectModVCO (short index, short ch, bool state)
    {
    this->SynthConfig.SetSelect (index, ch, state);
    this->Lfo[index].SetWave (ch, state);
    DisplayMessage.LfoHardSelect (index, ch, state);
    this->UpdateModButtons ();
    }

//#####################################################################
void SYNTH_FRONT_C::ToggleModRampDir (short index)
    {
    this->SetModRampDir (index, !this->Lfo[index].GetRampDir ());
    }

//#####################################################################
void SYNTH_FRONT_C::SetModRampDir (short index, bool state)
    {
    this->SynthConfig.SetRampDir (index, state);
    this->Lfo[index].SetRampDir (state);
    DisplayMessage.LfoHardRampSlope (index, state);
    this->UpdateModButtons ();
    }

//#####################################################################
void SYNTH_FRONT_C::ToggleModLevelAlt  (short index)
    {
    this->SetModLevelAlt (index, !this->Lfo[index].GetModLevelAlt ());
    }

//#####################################################################
void SYNTH_FRONT_C::SetModLevelAlt  (short index, bool state)
    {
    this->SynthConfig.SetModLevelAlt (index, state);
    this->Lfo[index].SetModLevelAlt (state);
    DisplayMessage.SetModLevelAlt (index, state);
    this->UpdateModButtons ();
    }



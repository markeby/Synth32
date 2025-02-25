//#######################################################################
// Module:     FrontEnd.cpp
// Descrption: Synthesizer front end controller
// Creator:    markeby
// Date:       12/9/2024
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

//#####################################################################
void SYNTH_FRONT_C::MidiMapMode ()
    {
    if ( !this->MapSelectMode )
        {
        DisplayMessage.Page (DISP_MESSAGE_N::PAGE_C::PAGE_MIDI_MAP);
        this->MapSelectMode = true;
        DisplayMessage.SendMapVoiceMidi (this->CurrentMapSelected, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, this->SynthConfig[this->CurrentMapSelected].GetVoiceMidi ());
        }
    else
        {
        DisplayMessage.Unlock ();
        this->ResetUSB ();
        this->CurrentMapSelected = 0;
        this->CurrentMidiSelected = this->SynthConfig[this->CurrentMapSelected].GetVoiceMidi ();
        this->MapSelectMode = false;
        DisplayMessage.SelectVoicePage (0);
        }
    }

//#####################################################################
void SYNTH_FRONT_C::MapModeBump (short down)
    {
    short z;

    if ( this->CurrentMapSelected < MAP_COUNT )
        {
        z = this->SynthConfig[this->CurrentMapSelected].GetVoiceMidi () + down;
        if ( z < 1 )            z = MAX_MIDI;
        if ( z > MAX_MIDI )     z = 1;
        this->SynthConfig[this->CurrentMapSelected].SetVoiceMidi (z);
        }
    else
        {
        z = this->SynthConfig[this->CurrentMapSelected].GetNoiseVoice () + down;
        if ( z <  0 )                    z = SOURCE_CNT_NOISE - 1;
        if ( z >= SOURCE_CNT_NOISE )     z = 0;
        this->SynthConfig[this->CurrentMapSelected].SetNoiseVoice (z);
        }

    DisplayMessage.SendMapVoiceMidi (this->CurrentMapSelected, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, z);
    }

//#####################################################################
void SYNTH_FRONT_C::ChangeMapSelect (short right)
    {
    if ( this->MapSelectMode )
        {
        if ( right == 0 )   // go left
            {
            if ( --this->CurrentMapSelected < 0 )
                this->CurrentMapSelected = (MAP_COUNT * GROUP_COUNT) - 1;
            }
        else                // not left so must be right
            {
            if ( ++this->CurrentMapSelected >= (MAP_COUNT * GROUP_COUNT) )
                this->CurrentMapSelected = 0;
            }
        if ( this->CurrentMapSelected < MAP_COUNT )
            DisplayMessage.SendMapVoiceMidi(this->CurrentMapSelected, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, this->SynthConfig[this->CurrentMapSelected].GetVoiceMidi ());
        else
            DisplayMessage.SendMapVoiceMidi(this->CurrentMapSelected, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, this->SynthConfig[this->CurrentMapSelected].GetNoiseVoice ());
        }
    }

//#####################################################################
void SYNTH_FRONT_C::ResolveMapAllocation ()
    {
    for ( short z = 0;  z <= MAP_COUNT;  z++ )
        {
        byte  m = this->SynthConfig[z].GetVoiceMidi ();
        for ( short zz = z + 1;  zz <= MAP_COUNT;  zz++ )
            {
            if ( m == this->SynthConfig[zz].GetVoiceMidi () )
                this->SynthConfig[zz] = this->SynthConfig[z];
            }
        }
    for ( short z = 0;  z < MAP_COUNT;  z++ )
        {
        short v = z * 2;                                            // calculate voice pair to set
        SYNTH_CONFIG_C& sc = this->SynthConfig[z];
        byte  m = sc.GetVoiceMidi ();
        VOICE_C& v0 = *(pVoice[v]);
        VOICE_C& v1 = *(pVoice[v + 1]);
        DisplayMessage.SetCurrentVoicePage (z);

        v0.SetMidiChannel (m);                          // set voice pair to selected midi channel
        v1.SetMidiChannel (m);

        DisplayMessage.SetVoicePage   (z, m);                       // Tell the display how to map
        this->pNoise->Select (z,  DUCT_WHITE, false);               // turn off all noise ducts
        this->pNoise->Select (z,  DUCT_PINK,  false);
        this->pNoise->Select (z,  DUCT_RED,   false);
        this->pNoise->Select (z,  DUCT_BLUE,  false);
        this->pNoise->Select (z,  this->SynthConfig[z].GetNoiseVoice(), true);      // set selected noise
        for ( short v = 0;  v < OSC_MIXER_COUNT;  v++ )
            {
            v0.pOsc ()->SetAttackTime      (v, sc.GetAttackTime   (v));
            v1.pOsc ()->SetAttackTime      (v, sc.GetAttackTime   (v));
            DisplayMessage.OscAttackTime   (v, sc.GetAttackTime   (v) * (1.0/TIME_MULT));
            v0.pOsc ()->SetDecayTime       (v, sc.GetDecayTime    (v));
            v1.pOsc ()->SetDecayTime       (v, sc.GetDecayTime    (v));
            DisplayMessage.OscDecayTime    (v, sc.GetDecayTime    (v) * (1.0/TIME_MULT));
            v0.pOsc ()->SetReleaseTime     (v, sc.GetReleaseTime  (v));
            v1.pOsc ()->SetReleaseTime     (v, sc.GetReleaseTime  (v));
            DisplayMessage.OscReleaseTime  (v, sc.GetReleaseTime  (v) * (1.0/TIME_MULT));
            v0.pOsc ()->SetSustainLevel    (v, sc.GetSustainLevel (v));
            v1.pOsc ()->SetSustainLevel    (v, sc.GetSustainLevel (v));
            DisplayMessage.OscSustainLevel (v, sc.GetSustainLevel (v) * PRS_UNSCALER);
            v0.pOsc ()->SetMaxLevel        (v, sc.GetMaxLevel     (v));
            v1.pOsc ()->SetMaxLevel        (v, sc.GetMaxLevel     (v));
            DisplayMessage.OscMaxLevel     (v, sc.GetMaxLevel     (v) * PRS_UNSCALER);
            sc.SelectedEnvelope[v] = false;
            this->VoiceComponentSetSelected (v, false);
            }
        v0.pOsc ()->SawtoothDirection       (sc.GetRampDirection ());
        v1.pOsc ()->SawtoothDirection       (sc.GetRampDirection ());
        DisplayMessage.OscSawtoothDirection (sc.GetRampDirection ());
        v0.pOsc ()->PulseWidth              (sc.GetPulseWidth    ());
        v1.pOsc ()->PulseWidth              (sc.GetPulseWidth    ());
        DisplayMessage.OscPulseWidth        (sc.GetPulseWidth    () * PRS_UNSCALER);
        }
    this->ResetXL  ();
    }

//#####################################################################
void SYNTH_FRONT_C::SaveDefaultConfig ()
    {
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        this->SynthConfig[z].Save (z);
    }


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

#ifdef DEBUG_SYNTH
static const char* Label  = "TOP";
static const char* LabelM = "M";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#define DBGM(args...) {if(DebugMidi){DebugMsg(LabelM,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#define DBGM(args...)
#endif

//#####################################################################
//#####################################################################
void SYNTH_FRONT_C::MidiMapMode ()
    {
    if ( !this->MapSelectMode )
        {
        this->TemplateSelect (XL_MIDI_MAP_MAPPING);
        DisplayMessage.Page (DISP_MESSAGE_N::PAGE_C::PAGE_MIDI_MAP);
        this->LoadSaveMode = false;
        this->MapSelectMode = true;
        for ( int z = (MAP_COUNT * GROUP_COUNT) - 1;  z >= 0;  z-- )
            this->UpdateMapModeDisplay (z);
        }
    else
        this->ResolveMapAllocation ();
    }

//#####################################################################
void SYNTH_FRONT_C::UpdateMapModeDisplay (int sel)
    {
    if ( sel < MAP_COUNT )
        DisplayMessage.SendMapVoiceMidi (sel, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, this->SynthConfig.Voice[sel].GetVoiceMidi ());

    else if ( sel < (MAP_COUNT * 2) )
        DisplayMessage.SendMapVoiceMidi (sel, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, this->SynthConfig.Voice[sel - MAP_COUNT].GetVoiceNoise ());

    else if ( sel < ((MAP_COUNT * 3) - 1) )
        DisplayMessage.SendMapVoiceMidi (sel, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, this->SynthConfig.GetModMidi (sel - (MAP_COUNT * 2)));
    }
//#####################################################################
void SYNTH_FRONT_C::MapModeBump (short down)
    {
    short z;
    ushort zi = this->CurrentMapSelected;

    while ( true )
        {
        if ( zi < MAP_COUNT )
            {
            z = this->SynthConfig.Voice[zi].GetVoiceMidi ();
            z += down;
            if ( z < 1 )            z = MAX_MIDI;
            if ( z > MAX_MIDI )     z = 1;
            this->SynthConfig.Voice[zi].SetVoiceMidi (z);
            break;
            }
        else
            zi -= MAP_COUNT;

        if ( zi < MAP_COUNT )
            {
            z = this->SynthConfig.Voice[zi].GetVoiceNoise () + down;
            if ( z <  0 )                   z = SOURCE_CNT_NOISE - 1;
            if ( z >= SOURCE_CNT_NOISE )    z = 0;
            this->SynthConfig.Voice[zi].SetVoiceNoise (z);
            break;
            }
        else
            zi -= MAP_COUNT;

        if ( zi < MAP_COUNT )
            {
            z = this->SynthConfig.GetModMidi (zi) + down;
            if ( z <  0 )           z = MAX_MIDI;
            if ( z > MAX_MIDI )     z = 0;
            printf("@@ zi = %d  z = %d\n", zi, z);
            this->SynthConfig.SetModMidi (zi, z);
            break;
            }
        else
            zi -= MAP_COUNT;
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
                this->CurrentMapSelected = (MAP_COUNT * GROUP_COUNT) - 2;
            }
        else                // not left so must be right
            {
            if ( ++this->CurrentMapSelected >= ((MAP_COUNT * GROUP_COUNT) - 1) )
                this->CurrentMapSelected = 0;
            }
        UpdateMapModeDisplay (this->CurrentMapSelected);
        }
    }

//#####################################################################
void SYNTH_FRONT_C::ResolveMapAllocation ()
    {
    this->LoadSaveMode   = false;
    this->MapSelectMode  = false;
    this->ResolutionMode = true;
    DisplayMessage.Unlock ();

    for ( short z = 0;  z <= MAP_COUNT;  z++ )              // make sure any subsequent config of the same midi address match completely
        {
        byte  m = this->SynthConfig.Voice[z].GetVoiceMidi ();
        for ( short zz = z + 1;  zz <= MAP_COUNT;  zz++ )
            {
            if ( m == this->SynthConfig.Voice[zz].GetVoiceMidi () )
                this->SynthConfig.Voice[zz] = this->SynthConfig.Voice[z];
            }
        }

    char lastMidi = 0;
    for ( short z = 0;  z < MAP_COUNT;  z++ )
        {
        short                 v    = z * 2;                         // calculate voice pair
        VOICE_C&              v0   = *(pVoice[v]);                  // Voice pair for this configuration processing
        VOICE_C&              v1   = *(pVoice[v + 1]);
        SYNTH_VOICE_CONFIG_C& sc   = this->SynthConfig.Voice[z];    // Configuration data for this voice pair
        byte                  m    = sc.GetVoiceMidi ();            // Get the midi value for voice pair
        bool                  newm = false;

        if ( m != lastMidi )
            newm = true;
        lastMidi = m;

        // set voice MIDI channel
        v0.SetMidi (m);
        v1.SetMidi (m);

        // configure hardware LFOs
        v0.SetModMux (0);                                       // set to zero in case none are touched below
        v1.SetModMux (0);
        for ( short v = 0;  v < 2;  v++ )
            {
            if ( m == this->SynthConfig.GetModMidi (v) )        // Detect MIDI matching
                v0.SetModMux (v + 1);
            }

        // Set selected noise
        v0.NoiseSelect (sc.GetVoiceNoise ());
        v1.NoiseSelect (sc.GetVoiceNoise ());

        for ( short v = 0;  v < OSC_MIXER_COUNT;  v++ )
            {
            v0.SetOscAttackTime (v, sc.GetOscAttackTime (v));
            v1.SetOscAttackTime (v, sc.GetOscAttackTime (v));
            v0.SetOscDecayTime (v, sc.GetOscDecayTime (v));
            v1.SetOscDecayTime (v, sc.GetOscDecayTime (v));
            v0.SetOscReleaseTime (v, sc.GetOscReleaseTime (v));
            v1.SetOscReleaseTime (v, sc.GetOscReleaseTime (v));
            v0.SetOscSustainLevel (v, sc.GetOscSustainLevel (v));
            v1.SetOscSustainLevel (v, sc.GetOscSustainLevel (v));
            v0.SetMaxLevel (v, sc.GetOscMaxLevel (v));
            v1.SetMaxLevel (v, sc.GetOscMaxLevel (v));
            sc.SelectedOscEnvelope[v] = false;

            if ( v < 2 )                            // filter envelopes setup here
                {
                v0.SetFltAttackTime (v, sc.GetFltAttackTime (v));
                v1.SetFltAttackTime (v, sc.GetFltAttackTime (v));
                v0.SetFltDecayTime (v, sc.GetFltDecayTime (v));
                v1.SetFltDecayTime (v, sc.GetFltDecayTime (v));
                v0.SetFltReleaseTime (v, sc.GetFltReleaseTime (v));
                v1.SetFltReleaseTime (v, sc.GetFltReleaseTime (v));
                v0.SetFltSustain (v, sc.GetFltSustainLevel (v));
                v1.SetFltSustain (v, sc.GetFltSustainLevel (v));
                v0.SetFltStart (v, sc.GetFltStart (v));
                v1.SetFltStart (v, sc.GetFltStart (v));
                v0.SetFltEnd (v, sc.GetFltEnd (v));
                v1.SetFltEnd (v, sc.GetFltEnd (v));
                sc.SelectedFltEnvelope[v] = false;
                }
            }
        v0.SetRampDirection (sc.GetRampDirection ());
        v1.SetRampDirection (sc.GetRampDirection ());
        v0.SetPulseWidth    (sc.GetPulseWidth    ());
        v1.SetPulseWidth    (sc.GetPulseWidth    ());
        v0.SetFltOut        (sc.GetFltOut        ());
        v1.SetFltOut        (sc.GetFltOut        ());
        }

    this->Lfo[0].SetMidi (this->SynthConfig.GetModMidi (0));
    this->Lfo[1].SetMidi (this->SynthConfig.GetModMidi (1));
    SoftLFO.SetMidi      (this->SynthConfig.GetModMidi (2));
    for ( short z = 0;  z < OSC_MIXER_COUNT;  z++ )
        this->SelectModVCA (z, this->SynthConfig.GetModSoftMixer (z));

    SoftLFO.SetFreq             (this->SynthConfig.GetSoftFreq    ());
    this->Lfo[0].SetFreq        (this->SynthConfig.GetFrequency   (0));
    this->Lfo[1].SetFreq        (this->SynthConfig.GetFrequency   (1));
    this->Lfo[0].SetPulseWidth  (this->SynthConfig.GetPulseWidth  (0));
    this->Lfo[1].SetPulseWidth  (this->SynthConfig.GetPulseWidth  (1));
    this->Lfo[0].SetRampDir     (this->SynthConfig.GetRampDir     (0));
    this->Lfo[1].SetRampDir     (this->SynthConfig.GetRampDir     (1));
    this->Lfo[0].SetModLevelAlt (this->SynthConfig.GetModLevelAlt (0));
    this->Lfo[1].SetModLevelAlt (this->SynthConfig.GetModLevelAlt (1));

    for ( short z = 0;  z < SOURCE_CNT_LFO;  z++ )
        {
        this->SelectModVCO (0, z, this->SynthConfig.GetSelect (0, z));
        this->SelectModVCO (1, z, this->SynthConfig.GetSelect (1, z));
        }

    for ( short z = 0;  z < OSC_MIXER_COUNT;  z++ )
        this->SelectModVCA (z, this->SynthConfig.GetModSoftMixer (z));

    I2cDevices.UpdateDigital ();
    I2cDevices.UpdateAnalog  ();
    this->PageAdvance ();
    }

//#####################################################################
void SYNTH_FRONT_C::SaveDefaultConfig ()
    {
    this->SynthConfig.Save (0);
    SynthFront.ResolveMapAllocation ();
    }

//#####################################################################
void SYNTH_FRONT_C::LoadDefaultConfig ()
    {
    this->SynthConfig.Load (0);
    this->ResolveMapAllocation ();
    }

//#####################################################################
void SYNTH_FRONT_C::LoadSelectedConfig ()
    {
    DisplayMessage.LoadMessage ();
    this->SynthConfig.Load (this->LoadSaveSelection);
    this->ResolveMapAllocation ();
    }

//#####################################################################
void SYNTH_FRONT_C::SaveSelectedConfig ()
    {
    DisplayMessage.SaveMessage ();
    this->SynthConfig.Save (this->LoadSaveSelection);
    this->ResolveMapAllocation ();
    }

//#####################################################################
void SYNTH_FRONT_C::LoadSaveBump (short down)
    {
    short z = this->LoadSaveSelection;
    z += down;
    if ( z < 1 )                z = MAX_LOAD_SAVE;
    if ( z > MAX_LOAD_SAVE )    z = 1;
    this->LoadSaveSelection = z;
    DisplayMessage.SendLoadSave (z);
    }

//#####################################################################
void SYNTH_FRONT_C::OpenLoadSavePage ()
    {
    DisplayMessage.Page (DISP_MESSAGE_N::PAGE_C::PAGE_LOAD_SAVE);
    this->LoadSaveMode = true;
    DisplayMessage.SendLoadSave (this->LoadSaveSelection);
    this->TemplateSelect (XL_MIDI_MAP_LOADSAVE);
    }


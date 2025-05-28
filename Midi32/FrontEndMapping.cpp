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
void SYNTH_FRONT_C::UpdateMapModeDisplay (int sel)
    {
    if ( sel < MAP_COUNT )
        DisplayMessage.SendMapVoiceMidi (sel, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, this->SynthConfig.Voice[sel].GetVoiceMidi () - 1);

    else if ( sel < (MAP_COUNT * 2) )
        DisplayMessage.SendMapVoiceMidi (sel, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, this->SynthConfig.Voice[sel - MAP_COUNT].GetVoiceNoise ());

    else if ( sel < (MAP_COUNT * 3) )
        DisplayMessage.SendMapVoiceMidi (sel, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, this->SynthConfig.Voice[sel - (MAP_COUNT * 2)].GetOutputEnable ());

    else if ( sel < ((MAP_COUNT * 4) - 1) )
        DisplayMessage.SendMapVoiceMidi (sel, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, this->SynthConfig.GetModMidi (sel - (MAP_COUNT * 3)));
    }
//#####################################################################
void SYNTH_FRONT_C::MidiMapMode ()
    {
    if ( !this->MapSelectMode )
        {
        DisplayMessage.Page (DISP_MESSAGE_N::PAGE_C::PAGE_MIDI_MAP);
        this->MapSelectMode = true;
        for ( int z = (MAP_COUNT * GROUP_COUNT) - 1;  z >= 0;  z-- )
            this->UpdateMapModeDisplay (z);
        }
    else
        {
        this->ResetUSB ();
        this->CurrentMapSelected = 0;
        this->CurrentMidiSelected = this->SynthConfig.Voice[this->CurrentMapSelected].GetVoiceMidi ();
        }
    }

//#####################################################################
void SYNTH_FRONT_C::MapModeBump (short down)
    {
    int z;
    ushort zi = this->CurrentMapSelected;

    while ( true )
        {
        if ( zi < MAP_COUNT )
            {
            z = this->SynthConfig.Voice[zi].GetVoiceMidi () + down;
            if ( z < 0 )            z = MAX_MIDI - 1;
            if ( z >= MAX_MIDI )    z = 0;
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
            z = this->SynthConfig.Voice[zi].GetOutputEnable () + down;
            if ( z <  0 )       z = 2;
            if ( z > 2 )        z = 0;
            this->SynthConfig.Voice[zi].SetOutputEnable (z);
            break;
            }
        else
            zi -= MAP_COUNT;

        if ( zi < MAP_COUNT )
            {
            z = this->SynthConfig.GetModMidi (zi) + down;
            if ( z <  0 )           z = MAX_MIDI;
            if ( z > MAX_MIDI )     z = 0;
            this->SynthConfig.SetModMidi (zi, z);
            }
        break;
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
    this->ResetXL  ();

    DisplayMessage.Unlock ();
    this->MapSelectMode = false;
    this->ResolutionMode = true;

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
        short                 v    = z * 2;                                   // calculate voice pair
        VOICE_C&              v0   = *(pVoice[v]);                            // Voice pair for this configuration processing
        VOICE_C&              v1   = *(pVoice[v + 1]);
        SYNTH_VOICE_CONFIG_C& sc   = this->SynthConfig.Voice[z];   // Configuration data for this voice pair
        byte                  m    = sc.GetVoiceMidi ();                      // Get the midi value for voice pair
        bool                  newm = false;

        if ( m != lastMidi )
            {
            newm = true;
            DisplayMessage.SetVoicePage(z, m);                 // Tell the display to setup voice display for this MIDI channel
            }
        else
            DisplayMessage.SetVoicePage(z, 0);          // No new midi channel

        lastMidi = m;

        // set voice MIDI channel
        v0.SetMidi (m);
        v1.SetMidi (m);

        // configure hardware LFOs
        v0.SetModMux (0);                                   // set to zero in case none are touched below
        v1.SetModMux (0);
        for ( short v = 0;  v < 2;  v++ )
            {
            if ( m == this->SynthConfig.GetModMidi (v) )    // Detect MIDI matching
                v0.SetModMux (v + 1);
            }
        v0.SetMux (0, false);                               // set to zero to be sure and turn off what isn't to be used
        v0.SetMux (1, false);
        v1.SetMux (0, false);
        v1.SetMux (1, false);
        v0.SetMux (this->SynthConfig.Voice[z].GetOutputEnable (), true);
        v1.SetMux (this->SynthConfig.Voice[z].GetOutputEnable (), true);

        // Set selected noise
        v0.NoiseSelect (this->SynthConfig.Voice[z].GetVoiceNoise ());
        v1.NoiseSelect (this->SynthConfig.Voice[z].GetVoiceNoise ());

        for ( short v = 0;  v < OSC_MIXER_COUNT;  v++ )
            {
            v0.SetAttackTime   (v, sc.GetAttackTime   (v));
            v1.SetAttackTime   (v, sc.GetAttackTime   (v));
            if ( newm )     DisplayMessage.OscAttackTime (v, sc.GetAttackTime   (v) * (1.0/TIME_MULT));
            v0.SetDecayTime    (v, sc.GetDecayTime    (v));
            v1.SetDecayTime    (v, sc.GetDecayTime    (v));
            if ( newm )     DisplayMessage.OscDecayTime  (v, sc.GetDecayTime    (v) * (1.0/TIME_MULT));
            v0.SetReleaseTime  (v, sc.GetReleaseTime  (v));
            v1.SetReleaseTime  (v, sc.GetReleaseTime  (v));
            if ( newm )     DisplayMessage.OscReleaseTime (v, sc.GetReleaseTime  (v) * (1.0/TIME_MULT));
            v0.SetSustainLevel (v, sc.GetSustainLevel (v));
            v1.SetSustainLevel (v, sc.GetSustainLevel (v));
            if ( newm )     DisplayMessage.OscSustainLevel (v, sc.GetSustainLevel (v) * PRS_UNSCALER);
            v0.SetMaxLevel     (v, sc.GetMaxLevel     (v));
            v1.SetMaxLevel     (v, sc.GetMaxLevel     (v));
            if ( newm )     DisplayMessage.OscMaxLevel (v, sc.GetMaxLevel (v) * PRS_UNSCALER);
            sc.SelectedEnvelope[v] = false;
            }
        v0.SetRampDirection     (sc.GetRampDirection ());
        v1.SetRampDirection     (sc.GetRampDirection ());
        if ( newm )     DisplayMessage.OscRampDirection (sc.GetRampDirection ());
        v0.SetPulseWidth        (sc.GetPulseWidth    ());
        v1.SetPulseWidth        (sc.GetPulseWidth    ());
        if ( newm )     DisplayMessage.OscPulseWidth (sc.GetPulseWidth () * PRS_UNSCALER);
        }

    this->Lfo[0].SetMidi (this->SynthConfig.GetModMidi (0));
    this->Lfo[1].SetMidi (this->SynthConfig.GetModMidi (1));
    SoftLFO.SetMidi      (this->SynthConfig.GetModMidi (3));

    for ( short z = 0;  z < OSC_MIXER_COUNT;  z++ )
        this->SelectModVCA (z, this->SynthConfig.GetModSoftMixer (z));

    this->FreqLFO       (0, this->SynthConfig.GetSoftFreq    ());
    this->FreqLFO       (1, this->SynthConfig.GetFrequency   (0));
    this->FreqLFO       (3, this->SynthConfig.GetFrequency   (1));
    this->FreqLFO       (2, this->SynthConfig.GetPulseWidth  (0));
    this->FreqLFO       (4, this->SynthConfig.GetPulseWidth  (1));
    this->SetModRampDir (0, this->SynthConfig.GetRampDir     (0));
    this->SetModRampDir (1, this->SynthConfig.GetRampDir     (1));

    for ( short z = 0;  z < SOURCE_CNT_LFO;  z++ )
        {
        this->SelectModVCO (0, z, this->SynthConfig.GetSelect (0, z));
        this->SelectModVCO (1, z, this->SynthConfig.GetSelect (1, z));
        }

    for ( short z = 0;  z < OSC_MIXER_COUNT;  z++ )
        this->SelectModVCA (z, this->SynthConfig.GetModSoftMixer (z));

    I2cDevices.UpdateDigital ();
    I2cDevices.UpdateAnalog  ();

    this->ResetXL  ();
    this->ResolutionMode = false;
    DisplayMessage.Page (DISP_MESSAGE_N::PAGE_C::PAGE_OSC0);
    }

//#####################################################################
void SYNTH_FRONT_C::SaveDefaultConfig ()
    {
    this->SynthConfig.Save (0);
    }


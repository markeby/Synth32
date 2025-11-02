//#######################################################################
// Module:     FrontEnd.cpp
// Descrption: Synthesizer front end controller
// Creator:    markeby
// Date:       12/9/2024
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

//#####################################################################
static short load_save_selection = 1;       // The Currently selected saved index (except the default at boot)

//#####################################################################
//#####################################################################
static void updateMapModeDisplay (int sel)
    {
    if ( sel < MAP_COUNT )
        DisplayMessage.SendMapVoiceMidi (sel, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, SynthConfig.Voice[sel].GetVoiceMidi ());

    else if ( sel < (MAP_COUNT * 2) )
        DisplayMessage.SendMapVoiceMidi (sel, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, SynthConfig.Voice[sel - MAP_COUNT].GetVoiceNoise ());

    else if ( sel < ((MAP_COUNT * 3) - 1) )
        DisplayMessage.SendMapVoiceMidi (sel, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, SynthConfig.GetModMidi (sel - (MAP_COUNT * 2)));
    }

//#####################################################################
void MidiMapMode ()
    {
    if ( !MapSelectMode )
        {
        LaunchControl.SelectTemplate (XL_MIDI_MAP_MAPPING);
        DisplayMessage.Page (DISP_MESSAGE_N::PAGE_C::PAGE_MIDI_MAP);
        LoadMode     = false;
        SaveMode     = false;
        MapSelectMode = true;
        for ( int z = (MAP_COUNT * GROUP_COUNT) - 1;  z >= 0;  z-- )
            updateMapModeDisplay (z);
        }
    else
        ResolveMapAllocation ();
    }

//#####################################################################
void MapModeBump (short down)
    {
    short z;
    ushort zi = SelectedMap;

    DBG ("Selected bump %d", down);
    while ( true )
        {
        if ( zi < MAP_COUNT )
            {
            z = SynthConfig.Voice[zi].GetVoiceMidi ();
            z += down;
            if ( z < 1 )            z = MAX_MIDI;
            if ( z > MAX_MIDI )     z = 1;
            SynthConfig.Voice[zi].SetVoiceMidi (z);
            break;
            }
        else
            zi -= MAP_COUNT;

        if ( zi < MAP_COUNT )
            {
            z = SynthConfig.Voice[zi].GetVoiceNoise () + down;
            if ( z <  0 )                   z = SOURCE_CNT_NOISE - 1;
            if ( z >= SOURCE_CNT_NOISE )    z = 0;
            SynthConfig.Voice[zi].SetVoiceNoise (z);
            break;
            }
        else
            zi -= MAP_COUNT;

        if ( zi < MAP_COUNT )
            {
            z = SynthConfig.GetModMidi (zi) + down;
            if ( z <  0 )           z = MAX_MIDI;
            if ( z > MAX_MIDI )     z = 0;
            SynthConfig.SetModMidi (zi, z);
            break;
            }
        else
            zi -= MAP_COUNT;
        }

    DisplayMessage.SendMapVoiceMidi (SelectedMap, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, z);
    }

//#####################################################################
void ChangeMapSelect (short right)
    {
    DBG ("Map Change %d", right);
    if ( MapSelectMode )
        {
        if ( right == 0 )   // go left
            {
            if ( --SelectedMap < 0 )
                SelectedMap = (MAP_COUNT * GROUP_COUNT) - 2;
            }
        else                // not left so must be right
            {
            if ( ++SelectedMap >= ((MAP_COUNT * GROUP_COUNT) - 1) )
                SelectedMap = 0;
            }
        updateMapModeDisplay (SelectedMap);
        }
    }

//#####################################################################
void ResolveMapAllocation ()
    {
    LoadMode       = false;
    SaveMode       = false;
    MapSelectMode  = false;
    ResolutionMode = true;
    DisplayMessage.Unlock ();

    for ( short z = 0;  z <= MAP_COUNT;  z++ )              // make sure any subsequent config of the same midi address match completely
        {
        byte  m = SynthConfig.Voice[z].GetVoiceMidi ();
        for ( short zz = z + 1;  zz <= MAP_COUNT;  zz++ )
            {
            if ( m == SynthConfig.Voice[zz].GetVoiceMidi () )
                SynthConfig.Voice[zz] = SynthConfig.Voice[z];
            }
        }

    char lastMidi = 0;
    for ( short z = 0;  z < MAP_COUNT;  z++ )
        {
        short                 v    = z * 2;                         // calculate voice pair
        VOICE_C&              v0   = *(VoiceArray[v]);                  // Voice pair for this configuration processing
        VOICE_C&              v1   = *(VoiceArray[v + 1]);
        SYNTH_VOICE_CONFIG_C& sc   = SynthConfig.Voice[z];    // Configuration data for this voice pair
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
            if ( m == SynthConfig.GetModMidi (v) )        // Detect MIDI matching
                v0.SetModMux (v + 1);
            }

        // Set selected noise
        v0.NoiseSelect (sc.GetVoiceNoise ());
        v1.NoiseSelect (sc.GetVoiceNoise ());

        for ( short v = 0;  v < OSC_MIXER_COUNT;  v++ )
            {
            v0.SetOscAttackTime   (v, sc.GetOscAttackTime (v));
            v1.SetOscAttackTime   (v, sc.GetOscAttackTime (v));
            v0.SetOscDecayTime    (v, sc.GetOscDecayTime (v));
            v1.SetOscDecayTime    (v, sc.GetOscDecayTime (v));
            v0.SetOscReleaseTime  (v, sc.GetOscReleaseTime (v));
            v1.SetOscReleaseTime  (v, sc.GetOscReleaseTime (v));
            v0.SetOscSustainLevel (v, sc.GetOscSustainLevel (v));
            v1.SetOscSustainLevel (v, sc.GetOscSustainLevel (v));
            v0.SetMaxLevel        (v, sc.GetOscMaxLevel (v));
            v1.SetMaxLevel        (v, sc.GetOscMaxLevel (v));
            sc.SelectedOscEnvelope[v] = false;

            if ( v < FILTER_DEVICES )                            // filter envelopes setup here
                {
                v0.SetFltAttackTime  (v, sc.GetFltAttackTime (v));
                v1.SetFltAttackTime  (v, sc.GetFltAttackTime (v));
                v0.SetFltDecayTime   (v, sc.GetFltDecayTime (v));
                v1.SetFltDecayTime   (v, sc.GetFltDecayTime (v));
                v0.SetFltReleaseTime (v, sc.GetFltReleaseTime (v));
                v1.SetFltReleaseTime (v, sc.GetFltReleaseTime (v));
                v0.SetFltSustain     (v, sc.GetFltSustainLevel (v));
                v1.SetFltSustain     (v, sc.GetFltSustainLevel (v));
                v0.SetFltStart       (v, sc.GetFltStart (v));
                v1.SetFltStart       (v, sc.GetFltStart (v));
                v0.SetFltEnd         (v, sc.GetFltEnd (v));
                v1.SetFltEnd         (v, sc.GetFltEnd (v));
                v0.SetFltCtrl        (v, sc.GetFltCtrl (v));
                v1.SetFltCtrl        (v, sc.GetFltCtrl (v));
                sc.SelectedFltEnvelope[v] = false;
                }
            }
        v0.SetRampDirection (sc.GetRampDirection ());
        v1.SetRampDirection (sc.GetRampDirection ());
        v0.SetPulseWidth    (sc.GetPulseWidth    ());
        v1.SetPulseWidth    (sc.GetPulseWidth    ());
        v0.SetOutputMask    (sc.GetOutputMask    ());
        v1.SetOutputMask    (sc.GetOutputMask    ());
        }

    Lfo[0].SetMidi (SynthConfig.GetModMidi (0));
    Lfo[1].SetMidi (SynthConfig.GetModMidi (1));
    SoftLFO.SetMidi      (SynthConfig.GetModMidi (2));
    for ( short z = 0;  z < OSC_MIXER_COUNT;  z++ )
        SelectModVCA (z, SynthConfig.GetModSoftMixer (z));

    SoftLFO.SetFreq       (SynthConfig.GetSoftFreq    ());
    Lfo[0].SetFreq        (SynthConfig.GetFrequency   (0));
    Lfo[1].SetFreq        (SynthConfig.GetFrequency   (1));
    Lfo[0].SetPulseWidth  (SynthConfig.GetPulseWidth  (0));
    Lfo[1].SetPulseWidth  (SynthConfig.GetPulseWidth  (1));
    Lfo[0].SetRampDir     (SynthConfig.GetRampDir     (0));
    Lfo[1].SetRampDir     (SynthConfig.GetRampDir     (1));
    Lfo[0].SetModLevelAlt (SynthConfig.GetModLevelAlt (0));
    Lfo[1].SetModLevelAlt (SynthConfig.GetModLevelAlt (1));

    for ( short z = 0;  z < SOURCE_CNT_LFO;  z++ )
        {
        SelectModVCO (0, z, SynthConfig.GetSelect (0, z));
        SelectModVCO (1, z, SynthConfig.GetSelect (1, z));
        }

    for ( short z = 0;  z < OSC_MIXER_COUNT;  z++ )
        SelectModVCA (z, SynthConfig.GetModSoftMixer (z));

    MuteVoicesReset ();
    I2cDevices.UpdateDigital ();
    I2cDevices.UpdateAnalog  ();
    PageAdvance ();
    }

//#####################################################################
void SaveDefaultConfig ()
    {
    SynthConfig.SaveConfig (0);
    ResolveMapAllocation ();
    }

//#####################################################################
void LoadDefaultConfig ()
    {
    SynthConfig.LoadConfig (0);
    ResolveMapAllocation ();
    }

//#####################################################################
void LoadSelectedConfig ()
    {
    DisplayMessage.LoadMessage ();
    SynthConfig.LoadConfig (load_save_selection);
    ResolveMapAllocation ();
    }

//#####################################################################
void SaveSelectedConfig ()
    {
    DisplayMessage.SaveMessage ();
    SynthConfig.SaveConfig (load_save_selection);
    ResolveMapAllocation ();
    }

//#####################################################################
void LoadSaveBump (short down)
    {
    short z = load_save_selection;
    z += down;
    if ( z < 1 )                z = MAX_LOAD_SAVE;
    if ( z > MAX_LOAD_SAVE )    z = 1;
    load_save_selection = z;
    DisplayMessage.SendLoadSave (z);
    }

//#####################################################################
void OpenLoadSavePage (bool save)
    {
    DisplayMessage.Page (DISP_MESSAGE_N::PAGE_C::PAGE_LOAD_SAVE);
    SaveMode = save;
    LoadMode = !save;
    DisplayMessage.SendLoadSave (load_save_selection);
    LaunchControl.LoadSave (save);
    LaunchControl.SelectTemplate (XL_MIDI_MAP_LOADSAVE);
    }


//#######################################################################
// Module:     Config.cpp
// Descrption: Configuration for synth
// Creator:    markeby
// Date:       2/16/2025
//#######################################################################
#include "Config.h"
#include "Settings.h"

//#######################################################################
//#######################################################################
    SYNTH_VOICE_CONFIG_C::SYNTH_VOICE_CONFIG_C ()
    {
    Mute = false;
    Cs.MapVoiceMidi  = 1;
    Cs.MapVoiceNoise = 0;
    Cs.OutputEnable  = 0;
    Cs.OutputMask  = 0;
    Cs.RampDirection = false;
    Cs.PulseWidth    = 2048;

    for ( short z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        Cs.OscEnv[z].AttackTime   = 1.0;      // All envelopes initialize
        Cs.OscEnv[z].DecayTime    = 1.0;
        Cs.OscEnv[z].ReleaseTime  = 1.0;
        Cs.OscEnv[z].SustainLevel = 0.0;
        Cs.OscEnv[z].MaxLevel     = 0.0;
        Cs.OscEnv[z].MinLevel     = 0.0;
        SelectedOscEnvelope[z]    = false;
        if ( z < FILTER_DEVICES )                                // filter initialization
            {
            Cs.FltEnv[z].AttackTime   = 1.0;
            Cs.FltEnv[z].DecayTime    = 1.0;
            Cs.FltEnv[z].ReleaseTime  = 1.0;
            Cs.FltEnv[z].SustainLevel = 0.0;
            Cs.FltEnv[z].MaxLevel     = 0.0;
            Cs.FltEnv[z].MinLevel     = 0.0;
            Cs.FilterCtrl[2]          = 0;
            SelectedFltEnvelope[z]    = false;
            }
        }

    // Default preload state
    int preset = 1;
    Cs.OscEnv[preset].AttackTime  = 20.0;
    Cs.OscEnv[preset].DecayTime   = 1.0;
    Cs.OscEnv[preset].ReleaseTime = 200.0;
    Cs.OscEnv[preset].MaxLevel    = 0.72;
    }

//#######################################################################
void SYNTH_VOICE_CONFIG_C::Save (const char* name)
    {
    Name = name;
    Settings.PutConfig (name, &(Cs), sizeof (Cs));
    }

//#######################################################################
void SYNTH_VOICE_CONFIG_C::Load (const char* name)
    {
    int zs = sizeof (Cs);

    if ( Settings.GetConfig (name, &Cs, zs) )
        printf ("### Error loading voice config!\n");
    }

//#######################################################################
//#######################################################################
    SYNTH_CONFIG_C::SYNTH_CONFIG_C ()
    {
    Cs.SoftFrequency = 1;
    for ( short z = 0;  z < 4;  z++ )
        Cs.LfoMidi[z];
    for ( short z = 0;  z < OSC_MIXER_COUNT;  z++ )
        Cs.SoftMixerLFO[z] = false;
    for ( short z = 0;  z < 2;  z++ )
        {
        Cs.CfgLFO[z].Frequency  = 1;
        Cs.CfgLFO[z].PulseWidth = 2048;
        Cs.CfgLFO[z].RampDir    = false;
        for ( short z = 0;  z < SOURCE_CNT_LFO;  z++ )
            this->Cs.CfgLFO[z].Select[z] = false;
        }
    }

//#######################################################################
void SYNTH_CONFIG_C::Save (short num)
    {
   String s;
   String sb;

    if ( num == 0 )                     // load default configuration
        sb = "DEF";
    else
        sb = String (num) + "FL";

    Name = sb;
    Settings.PutConfig (sb.c_str (), &(Cs), sizeof (Cs));
    for ( int z = 0;  z < MAP_COUNT;  z++ )
        {
        s = sb + String (z);
        Voice[z].Save(s.c_str());
        }
    }

//#######################################################################
void SYNTH_CONFIG_C::Load (short num)
    {
    String s;
    String sb;
    int    zs = sizeof (Cs);

    if ( num == 0 )                     // load default configuration
        sb = "DEF";
    else
        sb = String (num) + "FL";

    if (Settings.GetConfig (sb.c_str(), &Cs, zs) )
        return;
    for ( int z = 0;  z < MAP_COUNT;  z++ )
        {
        s = sb + String (z);
        Voice[z].Load (s.c_str ());
        }
    }



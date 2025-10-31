//#######################################################################
// Module:     Config.cpp
// Descrption: Configuration for synth
// Creator:    markeby
// Date:       2/16/2025
//#######################################################################
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>

#include "Config.h"
#include "Settings.h"

#include "ConfigJSON.h"

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
        Cs.OscEnv[z].AttackTime   = 1.0;        // All envelopes initialize
        Cs.OscEnv[z].DecayTime    = 1.0;
        Cs.OscEnv[z].ReleaseTime  = 1.0;
        Cs.OscEnv[z].SustainLevel = 0.0;
        Cs.OscEnv[z].MaxLevel     = 0.0;
        Cs.OscEnv[z].MinLevel     = 0.0;
        SelectedOscEnvelope[z]    = false;
        if ( z < FILTER_DEVICES )               // filter initialization
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
void SYNTH_VOICE_CONFIG_C::CreateEnvJSON (JsonVariant cfg, ENVELOPE_T& env)
    {
    cfg[k_Damper]  = env.Damper;
    cfg[k_Max]     = env.MaxLevel;
    cfg[k_Min]     = env.MinLevel;
    cfg[k_Sustain] = env.SustainLevel;
    cfg[k_Attack]  = env.AttackTime;
    cfg[k_Decay]   = env.DecayTime;
    cfg[k_Release] = env.ReleaseTime;
    }

//#######################################################################
void SYNTH_VOICE_CONFIG_C::LoadEnvJSON (JsonVariant cfg, SYNTH_VOICE_CONFIG_C::ENVELOPE_T& env)
    {
    env.Damper       = cfg[k_Damper];
    env.MaxLevel     = cfg[k_Max];
    env.MinLevel     = cfg[k_Min];
    env.SustainLevel = cfg[k_Sustain];
    env.AttackTime   = cfg[k_Attack];
    env.DecayTime    = cfg[k_Decay];
    env.ReleaseTime  = cfg[k_Release];
    }

//#######################################################################
void SYNTH_VOICE_CONFIG_C::CreateJSON (JsonVariant cfg)
    {
    int z;

    cfg[k_Midi]       = Cs.MapVoiceMidi;
    cfg[k_OutEnable]  = Cs.OutputEnable;
    cfg[k_Noise]      = Cs.MapVoiceNoise;
    cfg[k_PulseWidth] = Cs.PulseWidth;
    cfg[k_Ramp]       = Cs.RampDirection;
    cfg[k_OutMask]    = Cs.OutputMask;
    for ( z = 0;  z < FILTER_DEVICES;  z++ )
        cfg[k_FilterCtrl][z] = Cs.FilterCtrl[z];
    for ( z = 0;  z < OSC_MIXER_COUNT;  z++ )
        CreateEnvJSON (cfg[k_OscEnv][z], Cs.OscEnv[z]);
    for ( z = 0;  z < FILTER_DEVICES;  z++ )
        CreateEnvJSON (cfg[k_FltEnv][z], Cs.FltEnv[z]);
    }

//#######################################################################
void SYNTH_VOICE_CONFIG_C::LoadJSON (JsonVariant cfg)
    {
    int z;

    Cs.MapVoiceMidi  = cfg[k_Midi];
    Cs.OutputEnable  = cfg[k_OutEnable];
    Cs.MapVoiceNoise = cfg[k_Noise];
    Cs.PulseWidth    = cfg[k_PulseWidth];
    Cs.RampDirection = cfg[k_Ramp];
    Cs.OutputMask    = cfg[k_OutMask];
    for ( z = 0;  z < FILTER_DEVICES;  z++ )
        Cs.FilterCtrl[z] = cfg[k_FilterCtrl][z];
    for ( z = 0;  z < OSC_MIXER_COUNT;  z++ )
        LoadEnvJSON (cfg[k_OscEnv][z], Cs.OscEnv[z]);
    for ( z = 0;  z < FILTER_DEVICES;  z++ )
        LoadEnvJSON (cfg[k_FltEnv][z], Cs.FltEnv[z]);
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
bool SYNTH_CONFIG_C::Begin ()
    {
    if ( !SPIFFS.begin (true) )         // startup SPIFFS but initialize if failed
        {
         if ( !SPIFFS.begin (false) )   //  let's try again.
             {
             printf ("\n\n*****  File system is not operational *****\n\n\n");
             return true;
             }
        }
    printf("Listing directory\n");
    File root = SPIFFS.open ("/", FILE_READ, true);
    if ( !root.isDirectory() )
        {
        printf ("\n\n*****  File system cannot be opened *****\n\n\n");
        return true;
        }

    File file = root.openNextFile();
    while (file)
        {
        if (file.isDirectory())
            printf ("  DIR : %s/n", file.name());
        else
            printf ("  FILE: %s\tSIZE: %d/n", file.name(), file.size());
        file = root.openNextFile();
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

//#######################################################################
void SYNTH_CONFIG_C::CreateJSON (JsonDocument& cfg, short num)
    {
    int    z;

    cfg[k_SoftFreq] = Cs.SoftFrequency;
    for ( z = 0;  z < MAP_COUNT;  z++ )
        cfg[k_LfoMidi][z] = Cs.LfoMidi[z];
    for ( z = 0;  z < OSC_MIXER_COUNT;  z++ )
        cfg[k_SoftMixerLFO][z] = Cs.SoftMixerLFO[z];
    for ( z = 0;  z < SOURCE_HWD_LFO;  z++ )
        {
        cfg[k_CfgLFO][z][k_Freq]   = Cs.CfgLFO[z].Frequency;
        cfg[k_CfgLFO][z][k_PulseW] = Cs.CfgLFO[z].PulseWidth;
        cfg[k_CfgLFO][z][k_ModAlt] = Cs.CfgLFO[z].ModLevelAlt;
        for ( int zz = 0;  zz < SOURCE_CNT_LFO;  zz++ )
            cfg[k_CfgLFO][z][k_Sel][zz] = Cs.CfgLFO[z].Select[zz];
        }

    for ( z = 0;  z < MAP_COUNT;  z++ )
        {
        String s = String (z);
        Voice[z].CreateJSON (cfg[s.c_str ()]);
        }

    serializeJson (cfg, Serial);
    DbgN;
    }

//#######################################################################
void SYNTH_CONFIG_C::LoadJSON (JsonDocument& cfg, short num)
    {
    int    z;

    Cs.SoftFrequency = cfg[k_SoftFreq];
    for ( z = 0;  z < MAP_COUNT;  z++ )
        Cs.LfoMidi[z] = cfg[k_LfoMidi][z];
    for ( z = 0;  z < OSC_MIXER_COUNT;  z++ )
         Cs.SoftMixerLFO[z] = cfg[k_SoftMixerLFO][z];
    for ( z = 0;  z < SOURCE_HWD_LFO;  z++ )
        {
        Cs.CfgLFO[z].Frequency   = cfg[k_CfgLFO][z][k_Freq];
        Cs.CfgLFO[z].PulseWidth  = cfg[k_CfgLFO][z][k_PulseW];
        Cs.CfgLFO[z].ModLevelAlt = cfg[k_CfgLFO][z][k_ModAlt];
        for ( int zz = 0;  zz < SOURCE_CNT_LFO;  zz++ )
             Cs.CfgLFO[z].Select[zz] = cfg[k_CfgLFO][z][k_Sel][zz];
        }

    for ( z = 0;  z < MAP_COUNT;  z++ )
        {
        String s = String (z);
        Voice[z].LoadJSON (cfg[s.c_str ()]);
        }
    }



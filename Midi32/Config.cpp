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
#include "Debug.h"

static const char* LabelD = "CFG";
#define ERROR(args...) {ErrorMsg (LabelD, __FUNCTION__, args);}
#define DBGM(args...) {if(DebugDisp){ DebugMsg(LabelD,DEBUG_NO_INDEX,args);}}

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
JsonDocument SYNTH_VOICE_CONFIG_C::CreateEnvJSON (ENVELOPE_T& env)
    {
    JsonDocument cfg;

    cfg[k_Damper]  = env.Damper;
    cfg[k_Max]     = env.MaxLevel;
    cfg[k_Min]     = env.MinLevel;
    cfg[k_Sustain] = env.SustainLevel;
    cfg[k_Attack]  = env.AttackTime;
    cfg[k_Decay]   = env.DecayTime;
    cfg[k_Release] = env.ReleaseTime;
    return (cfg);
    }

//#######################################################################
JsonDocument SYNTH_VOICE_CONFIG_C::CreateJSON ()
    {
    int z;
    JsonDocument cfg;

    cfg[k_Midi]       = Cs.MapVoiceMidi;
    cfg[k_OutEnable]  = Cs.OutputEnable;
    cfg[k_Noise]      = Cs.MapVoiceNoise;
    cfg[k_PulseWidth] = Cs.PulseWidth;
    cfg[k_Ramp]       = Cs.RampDirection;
    cfg[k_OutMask]    = Cs.OutputMask;
    for ( z = 0;  z < FILTER_DEVICES;  z++ )
        cfg[k_FilterCtrl][z] = Cs.FilterCtrl[z];
    for ( z = 0;  z < OSC_MIXER_COUNT;  z++ )
        cfg[k_OscEnv][z] = CreateEnvJSON (Cs.OscEnv[z]);
    for ( z = 0;  z < FILTER_DEVICES;  z++ )
        cfg[k_FltEnv][z] = CreateEnvJSON (Cs.FltEnv[z]);
    return (cfg);
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
void SYNTH_VOICE_CONFIG_C::LoadJSON (JsonObject cfg)
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
    if ( !SPIFFS.begin (true) )
        {
        printf ("SPIFFS Mount Failed\n");
        return true;
        }
    return (DirectoryMonitor ());
    }

//#######################################################################
void SYNTH_CONFIG_C::SaveConfig (short num)
    {
   String s;

    JsonDocument cfg = CreateJSON ();

    if ( num == 0 )                     // load default configuration
        s = "/DEFAULT.syc";
    else
        s = "/PRE-" + String (num) + ".syc";

    File file = SPIFFS.open (s, FILE_WRITE);
    if ( !file )
        {
        Serial.println("- failed to open file for writing");
        return;
        }

    serializeJson (cfg, file);
    file.close();
    }

//#######################################################################
JsonDocument SYNTH_CONFIG_C::CreateJSON ()
    {
    int    z;
    JsonDocument cfg;

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
        cfg[s.c_str ()] = Voice[z].CreateJSON ();
        }

    return (cfg);
    }

//#######################################################################
void SYNTH_CONFIG_C::LoadConfig (short num)
    {
    String s;

    JsonDocument cfg;

    if ( num == 0 )                     // load default configuration
        s = "/DEFAULT.syc";
    else
        s = "/PRE-" + String (num) + ".syc";

    File file = SPIFFS.open (s);
    if ( !file )
        ERROR ("Error openeing $s", s.c_str ());
    if ( file.available () )
        {
        file.readBytes (SpiffsBuffer,  MAX_BUFFER -2);
        DeserializationError error = deserializeJson (cfg, SpiffsBuffer);
        if (error)
            {
            printf ("deserializeJson() failed: %s", error.f_str ());
            return;
            }
        }
    file.close ();
    LoadConfigJSON (cfg);
    }

//#######################################################################
void SYNTH_CONFIG_C::LoadConfigJSON (JsonDocument cfg)
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

//#######################################################################
bool SYNTH_CONFIG_C::DirectoryMonitor ()
    {
    bool bigflag = false;

    File root = SPIFFS.open("/");
    if ( !root )
        {
        printf ("\t\t>## failed to open root directory\n");
        return true;
        }
    if ( !root.isDirectory() )
        {
        printf ("\t>## not a directory\n");
        return true;
        }

    printf ("\n\t      Synth Directory\n");
    File fname = root.openNextFile();
    while (fname)
        {
        if ( !fname.isDirectory() )
            {
            printf ("\t   %s\t    %d\n", fname.name (), fname.size ());
            if ( fname.size () > (MAX_BUFFER- 4) )
                bigflag = true;
            String s = fname.name();
            }
        fname = root.openNextFile ();
        }
    printf("\n");
    if ( bigflag )
        printf("\n#### ERROR -- SPIFFS read buffer allocation exceeds the current size of %d\n\n", MAX_BUFFER);
    return false;
    }

//#######################################################################
void SYNTH_CONFIG_C::DumpFiles ()
    {
    printf("\tSynth configuration file dump\n\n");
    File root = SPIFFS.open("/");
    if ( root )
        {
        if ( root.isDirectory() )
            {
            File fname = root.openNextFile();
            while (fname)
                {
                if ( !fname.isDirectory() )
                    {
                    String name = fname.name ();
                    String path = String("/") + name;
                    printf ("> %s\n", name.c_str ());
                    File file = SPIFFS.open (path.c_str ());
                    if ( !file )
                        ERROR ("Error openeing $s", name.c_str ());
                    while ( file.available () )
                        Serial.write (file.read());
                    printf ("\n\n");
                    file.close ();
                    }
                fname = root.openNextFile ();
                }
            }
        }
    }


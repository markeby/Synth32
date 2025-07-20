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
    this->Cs.MapVoiceMidi  = 1;
    this->Cs.MapVoiceNoise = 0;
    this->Cs.OutputEnable  = 0;
    this->Cs.RampDirection    = false;
    this->Cs.PulseWidth       = 2048;

    for ( short z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        this->Cs.Env[z].AttackTime   = 1.0;
        this->Cs.Env[z].DecayTime    = 1.0;
        this->Cs.Env[z].ReleaseTime  = 1.0;
        this->Cs.Env[z].SustainLevel = 0.0;
        this->Cs.Env[z].MaxLevel     = 0.0;
        this->SelectedEnvelope[z]    = false;
        }

    // Default preload state
    int preset                       = 1;
    this->Cs.Env[preset].AttackTime  = 20.0;
    this->Cs.Env[preset].DecayTime   = 1.0;
    this->Cs.Env[preset].ReleaseTime = 200.0;
    this->Cs.Env[preset].MaxLevel    = 0.72;
    }

//#######################################################################
void SYNTH_VOICE_CONFIG_C::Save (const char* name)
    {
    this->Name = name;
    Settings.PutConfig (name, &(this->Cs), sizeof (Cs));
    }

//#######################################################################
void SYNTH_VOICE_CONFIG_C::Load (const char* name)
    {
    if ( Settings.GetConfig (name, &(this->Cs), sizeof (Cs)) )
        printf ("### Error loading voice config!\n");
    this->InitButtonsXL ();
    }

//#######################################################################
void SYNTH_VOICE_CONFIG_C::InitButtonsXL ()
    {
    for ( short z = 0;  z < XL_BUTTON_COUNT;  z++ )
        this->ButtonState[z] = XL_MidiMapArray[XL_MIDI_MAP_OSC][XL_BUTTON_START + z].Color;
    }

//#######################################################################
//#######################################################################
    SYNTH_CONFIG_C::SYNTH_CONFIG_C ()
    {
    this->Cs.SoftFrequency = 1;
    for ( short z = 0;  z < 4;  z++ )
        this->Cs.LfoMidi[z];
    for ( short z = 0;  z < OSC_MIXER_COUNT;  z++ )
        this->Cs.SoftMixerLFO[z] = false;
    for ( short z = 0;  z < 2;  z++ )
        {
        this->Cs.CfgLFO[z].Frequency  = 1;
        this->Cs.CfgLFO[z].PulseWidth = 2048;
        this->Cs.CfgLFO[z].RampDir    = false;
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

    this->Name = sb;
    Settings.PutConfig (sb.c_str (), &(this->Cs), sizeof (Cs));
    for ( int z = 0;  z < MAP_COUNT;  z++ )
        {
        s = sb + String (z);
        this->Voice[z].Save (s.c_str ());
        }
    }

//#######################################################################
void SYNTH_CONFIG_C::Load (short num)
    {
    String s;
    String sb;

    if ( num == 0 )                     // load default configuration
        sb = "DEF";
    else
        sb = String (num) + "FL";

    if (Settings.GetConfig (sb.c_str(), &(this->Cs), sizeof(Cs)) )
        return;
    for ( int z = 0;  z < MAP_COUNT;  z++ )
        {
        s = sb + String (z);
        this->Voice[z].Load (s.c_str ());
        }
    }


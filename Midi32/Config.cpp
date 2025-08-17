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
    this->Cs.FilterEnables  = 0;
    this->Cs.RampDirection = false;
    this->Cs.PulseWidth    = 2048;

    for ( short z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        this->Cs.OscEnv[z].AttackTime   = 1.0;      // All envelopes initialize
        this->Cs.OscEnv[z].DecayTime    = 1.0;
        this->Cs.OscEnv[z].ReleaseTime  = 1.0;
        this->Cs.OscEnv[z].SustainLevel = 0.0;
        this->Cs.OscEnv[z].MaxLevel     = 0.0;
        this->Cs.OscEnv[z].MinLevel     = 0.0;
        this->SelectedOscEnvelope[z]    = false;
        if ( z < FILTER_DEVICES )                                // filter initialization
            {
            this->Cs.FltEnv[z].AttackTime   = 1.0;
            this->Cs.FltEnv[z].DecayTime    = 1.0;
            this->Cs.FltEnv[z].ReleaseTime  = 1.0;
            this->Cs.FltEnv[z].SustainLevel = 0.0;
            this->Cs.FltEnv[z].MaxLevel     = 0.0;
            this->Cs.FltEnv[z].MinLevel     = 0.0;
            this->Cs.FilterCtrl[2]          = 0;
            this->SelectedFltEnvelope[z]    = false;
            }
        }

    // Default preload state
    int preset                       = 1;
    this->Cs.OscEnv[preset].AttackTime  = 20.0;
    this->Cs.OscEnv[preset].DecayTime   = 1.0;
    this->Cs.OscEnv[preset].ReleaseTime = 200.0;
    this->Cs.OscEnv[preset].MaxLevel    = 0.72;
    }

//#######################################################################
void SYNTH_VOICE_CONFIG_C::Save (const char* name)
    {
    this->Name = name;
    Settings.PutConfig (name, &(this->Cs), sizeof (this->Cs));
    }

//#######################################################################
void SYNTH_VOICE_CONFIG_C::Load (const char* name)
    {
    if ( Settings.GetConfig (name, &(this->Cs), sizeof (this->Cs)) )
        printf ("### Error loading voice config!\n");
    this->InitButtonsXL ();
    }

//#######################################################################
void SYNTH_VOICE_CONFIG_C::InitButtonsXL ()
    {
    for ( short z = 0;  z < XL_BUTTON_COUNT;  z++ )
        {
        this->ButtonStateOsc[z] = XL_MidiMapArray[XL_MIDI_MAP_OSC][XL_BUTTON_START + z].Color;
        this->ButtonStateFlt[z] = XL_MidiMapArray[XL_MIDI_MAP_FLT][XL_BUTTON_START + z].Color;
        }
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
        this->Voice[z].Save(s.c_str());
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
    this->InitButtonsXL ();
    }

//#######################################################################
void SYNTH_CONFIG_C::InitButtonsXL ()
    {
    for ( short z = 0;  z < XL_BUTTON_COUNT;  z++ )
        this->ButtonStateLfo[z] = XL_MidiMapArray[XL_MIDI_MAP_LFO][XL_BUTTON_START + z].Color;
    }



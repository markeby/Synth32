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
    SYNTH_CONFIG_C::SYNTH_CONFIG_C ()
    {
    S0.MapVoiceMidi  = 1;
    S0.MapNoiseVoice = 0;
    S0.MasterLevel   = 0.99;
    for ( short z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        S0.AttackTime[z]       = 1.0;
        S0.DecayTime[z]        = 1.0;
        S0.ReleaseTime[z]      = 1.0;
        S0.SustainLevel[z]     = 0.0;
        S0.MaxLevel[z]         = 0.0;
        SelectedEnvelope[z]    = false;
        }

    int preset = 1;
    S0.AttackTime[preset]      = 20.0;
    S0.DecayTime[preset]       = 1.0;
    S0.ReleaseTime[preset]     = 200.0;
    S0.MaxLevel[preset]        = 0.72;
    }

//#######################################################################
void SYNTH_CONFIG_C::Save (int num)
    {
    Settings.PutDefaultConfig (num, &(this->S0), sizeof (CONFIG_SAVE_T));
    }

//#######################################################################
void SYNTH_CONFIG_C::Load (int num)
    {
    Settings.GetDefaultConfig (num, &(this->S0), sizeof (CONFIG_SAVE_T));
    }



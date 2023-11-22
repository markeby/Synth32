//#######################################################################
// Module:     Graphics.cpp
// Descrption: Graphics module
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>

#include "config.h"
#include "SynthData.h"
#include "Graphics.h"

static  const char* MixerNames[] = { "sine", "triangle", "square", "saw", "pulse" };
extern  int  OscMixerColor[OSC_MIXER_COUNT];

//#######################################################################
SYNTH_DATA_C::SYNTH_DATA_C ()
    {
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        MIXER_T& m = Mix[z];

        m.Name             = MixerNames[z];
        m.LimitLevel       = 0;
        m.AttackTime       = 0;
        m.DecayTime        = 0;
        m.SustainLevel     = 0;
        m.SustainTime      = 0;
        m.ReleaseTime      = 0;
        m.Selected         = false;
        m.Color            = OscMixerColor[z];
        InitSaveVectors (z);
        }
    }

//#######################################################################
SYNTH_DATA_C  SynthD;

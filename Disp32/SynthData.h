//#######################################################################
// Module:     SynthData.cpp
// Descrption: Transaction data for display
// Creator:    markeby
// Date:       7/8/2024
//#######################################################################
#pragma once

#include "Widgets.h"

struct VCA_S
    {
    bool            Active;         // Status of in use
    bool            Selected;       // Currently in the selected state
    float           LimitLevel;     // Maximum amplitude in percent 0.0 - 1.0
    float           AttackTime;     // Attack time 0.0 - 1.0
    float           DecayTime;      // Decay time 0.0 - 1.0
    float           SustainLevel;   // Sustain aplitude in percent 0.0 - 1.0
    float           SustainTime;    // How long to hold the sustain level 0.0 - 1.0 (0.0 = hold with key down)
    float           ReleaseTime;    // Release time 0.0 - 1.0
    String          Title;          // Name of oscillator
    ADSR_WIDGET_C*  Meter;          // Graphic meter widget
        VCA_S () : Active(false), Selected(false), LimitLevel(0.0), AttackTime(0.0),
                   DecayTime(0.0), SustainLevel(0.0), SustainTime(0.0), ReleaseTime(0.0)
            {}
    };

typedef struct VCA_S   VCA_T;

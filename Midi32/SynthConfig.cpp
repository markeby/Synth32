//#######################################################################
// Module:     SyntConfit.ino
// Descrption: Synthesizer midi configuration
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include <SynthCommon.h>
#include "config.h"
#include "SynthFront.h"

//########################################################
void SetOscMixers (byte ch, byte data)
    {
    SynthFront.SetOscMaxLevel (ch, data);
    }

//########################################################
void SetOscSustain (byte ch, byte data)
    {
    SynthFront.SetOscSustainLevel (ch - 8, data);
    }

//########################################################
void SetOscTimeSwitch (byte ch, byte state)
    {
    SynthFront.OscChannelSelect (ch, state);
    }

//########################################################
void SetModSwitch (byte ch, byte state)
    {
    SynthFront.SelectWaveLFO (ch - 8, state);
    }

//########################################################
void SetAttckTime (byte ch, byte data)
    {
    SynthFront.SetOscAttackTime (data);
    }

//########################################################
void  SetDecayTime (byte ch, byte data)
    {
    SynthFront.SetOscDecayTime (data);
    }

//########################################################
void  SetSustainTime (byte ch, byte data)
    {
    SynthFront.SetOscSustainTime(data);
    }

//########################################################
void  SetReleaseTime (byte ch, byte data)
    {
    SynthFront.SetOscReleaseTime (data);
    }

//########################################################
void SetLfoFreq (byte ch, byte data)
    {
    SynthFront.FreqSelectLFO (0, data);
    }

//########################################################
void SetPlusRangeLFO (byte ch, byte state)
    {
    SynthFront.LFOrange (true);
    }

//########################################################
void SetMinusRangeLFO (byte ch, byte state)
    {
    SynthFront.LFOrange (false);
    }

//########################################################
MIDI_VALUE_MAP    FaderMapArray[SM_FADER] =
    {   {  0, "Sine Max ",         SetOscMixers  },         // 0-100%
        {  1, "Triangle Max ",     SetOscMixers  },
        {  2, "Sqiare Max ",       SetOscMixers  },
        {  3, "Sawtooth Max",      SetOscMixers  },
        {  4, "Pulse Max",         SetOscMixers  },
        {  5, "N ",                nullptr       },
        {  6, "N ",                nullptr       },
        {  7, "N ",                nullptr       },
        {  8, "Sine Sustain ",     SetOscSustain },
        {  9, "Triangle Sustain ", SetOscSustain },
        { 10, "Sqiare Sustain ",   SetOscSustain },
        { 11, "Sawtooth Sustain",  SetOscSustain },
        { 12, "Pulse Sustain",     SetOscSustain },
        { 13, "Max ",              nullptr       },
        { 14, "Max ",              nullptr       },
        { 15, "Max ",              nullptr       },
    };

//########################################################
MIDI_VALUE_MAP    KnobMapArray[SM_CONTROL] =
    {   {  0, "Attack",     SetAttckTime   },
        {  1, "Decay",      SetDecayTime   },
        {  2, "Sustain",    SetSustainTime },
        {  3, "Release",    SetReleaseTime },
        {  4, "N ",         nullptr        },
        {  5, "N ",         nullptr        },
        {  6, "N ",         nullptr        },
        {  7, "LFO Freq ",  SetLfoFreq     },
        {  8, "N ",         nullptr        },
        {  9, "N ",         nullptr        },
        { 10, "N ",         nullptr        },
        { 11, "N ",         nullptr        },
        { 12, "N ",         nullptr        },
        { 14, "N ",         nullptr        },
        { 14, "N ",         nullptr        },
        { 15, "N ",         nullptr        },
    };

//########################################################
MIDI_SWITCH_MAP SwitchMapArray[SM_SWITCH] =
    {   {  0, "Sine",         SetOscTimeSwitch },
        {  1, "Triangle",     SetOscTimeSwitch },
        {  2, "Sqiare",       SetOscTimeSwitch },
        {  3, "Sawtooth",     SetOscTimeSwitch },
        {  4, "Pulse",        SetOscTimeSwitch },
        {  5, "Switch #6",    nullptr          },
        {  6, "Switch #7",    nullptr          },
        {  7, "Switch #8",    nullptr          },
        {  8, "Mod Sine",     SetModSwitch     },
        {  9, "Mod Triangle", SetModSwitch     },
        { 10, "Mod Sawtooth", SetModSwitch     },
        { 11, "Mod Pulse   ", SetModSwitch     },
        { 12, "Switch #14",   nullptr          },
        { 13, "Switch #14",   nullptr          },
        { 14, "Switch #15",   nullptr          },
        { 15, "Switch #16",   nullptr          },
        { 16, "Switch #17",   nullptr          },
        { 17, "Switch #18",   nullptr          },
        { 18, "Switch #18",   nullptr          },
        { 19, "Switch #18",   nullptr          },
        { 20, "LFO Range -",  SetPlusRangeLFO  },
        { 21, "LFO Range +",  SetMinusRangeLFO },
        { 22, "Switch #18",   nullptr          },
        { 23, "Switch #18",   nullptr          },
    };






//#######################################################################
// Module:     SyntConfig.ino
// Descrption: Synthesizer midi configuration
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include "../Common/SynthCommon.h"
#include "config.h"
#include "FrontEnd.h"
#include "SerialMonitor.h"

//########################################################
static void SetMaxLevel (uint8_t ch, uint8_t data)
    {
    SynthFront.SetMaxLevel (ch, data);
    }

//########################################################
static void SetSustain (uint8_t ch, uint8_t data)
    {
    SynthFront.SetSustainLevel (ch - 8, data);
    }

//########################################################
static void SetTimeSetSelect (uint8_t ch, uint8_t state)
    {
    SynthFront.ChannelSetSelect (ch, state);
    }

//########################################################
static void SetModSwitch (uint8_t ch, uint8_t state)
    {
    SynthFront.SelectWaveLFO (ch - 8, state);
    }

//########################################################
static void SetAttckTime (uint8_t ch, uint8_t data)
    {
    SynthFront.SetAttackTime (data);
    }

//########################################################
static void SetDecayTime (uint8_t ch, uint8_t data)
    {
    SynthFront.SetDecayTime (data);
    }

//########################################################
static void SetSustainTime (uint8_t ch, uint8_t data)
    {
    SynthFront.SetSustainTime (data);
    }

//########################################################
static void SetReleaseTime (uint8_t ch, uint8_t data)
    {
    SynthFront.SetReleaseTime (data);
    }

//########################################################
static void SawtoothDirection (uint8_t ch, uint8_t data)
    {
    SynthFront.SawtoothDirection (data != 0);
    }

//########################################################
static void PulseWidth (uint8_t ch, uint8_t data)
    {
    if ( data == 0 )
        data = 1;
    SynthFront.SetPulseWidth (data);
    }

//########################################################
static void SoftLfoFreq(uint8_t ch, uint8_t data)
    {
    SynthFront.SetSoftSineLFO (data);
    }

//########################################################
static void SetLfoFreq (uint8_t ch, uint8_t data)
    {
    SynthFront.FreqSelectLFO (0, data);
    }

//########################################################
static void SetPlusRangeLFO (uint8_t ch, uint8_t state)
    {
    SynthFront.LFOrange (true);
    }

//########################################################
static void SetMinusRangeLFO (uint8_t ch, uint8_t state)
    {
    SynthFront.LFOrange (false);
    }

//########################################################
static void SetNoiseFilter (uint8_t ch, uint8_t state)
    {
    SynthFront.NoiseFilter (ch - 14, state > 120);      // offset of SwitchMapArray to get 0 or 1
    }

//########################################################
static void SetWhiteNoise (uint8_t ch, uint8_t state)
    {
    SynthFront.NoiseColor (state);
    }

//########################################################
static void SetNoisFilterMin (uint8_t ch, uint8_t data)
    {
    SynthFront.SetNoiseFilterMin(data);
    }

//########################################################
static void SetNoiseFilterMax (uint8_t ch, uint8_t data)
    {
    SynthFront.SetNoiseFilterMax (data);
    }

//########################################################
static void TuneReset (uint8_t ch, uint8_t data)
    {
    if ( data )
        Monitor.Tuning ();
    else
        Monitor.Reset ();
    }

//########################################################
MIDI_VALUE_MAP    FaderMapArray[SM_FADER] =
    {   {  0, "Sine max level",         SetMaxLevel       },         // 0-100%
        {  1, "Triangle max level",     SetMaxLevel       },
        {  2, "Sqiare max level",       SetMaxLevel       },
        {  3, "Sawtooth max level",     SetMaxLevel       },
        {  4, "Pulse max level",        SetMaxLevel       },
        {  5, "Noise max level",        SetMaxLevel       },
        {  6, "Noise filter Min freq",  SetNoisFilterMin  },
        {  7, "Noise filter Max freq",  SetNoiseFilterMax },
        {  8, "Sine Sustain",           SetSustain        },
        {  9, "Triangle Sustain",       SetSustain        },
        { 10, "Sqiare Sustain",         SetSustain        },
        { 11, "Sawtooth Sustain",       SetSustain        },
        { 12, "Pulse Sustain",          SetSustain        },
        { 13, "Noise Sustain",          SetSustain        },
        { 14, "Noise filter Sustain",   SetSustain        },
        { 15, "N ",                     nullptr           },
    };

//########################################################
MIDI_VALUE_MAP    KnobMapArray[SM_CONTROL] =
    {   {  0, "Attack",         SetAttckTime   },
        {  1, "Decay",          SetDecayTime   },
        {  2, "Sustain",        SetSustainTime },
        {  3, "Release",        SetReleaseTime },
        {  4, "Pulse Width",    PulseWidth     },
        {  5, "N ",             nullptr        },
        {  6, "SoftLFO Freq",   SoftLfoFreq    },
        {  7, "LFO Freq ",      SetLfoFreq     },
        {  8, "N ",             nullptr        },
        {  9, "N ",             nullptr        },
        { 10, "N ",             nullptr        },
        { 11, "N ",             nullptr        },
        { 12, "N ",             nullptr        },
        { 14, "N ",             nullptr        },
        { 14, "N ",             nullptr        },
        { 15, "N ",             nullptr        },
    };

//########################################################
MIDI_SWITCH_MAP SwitchMapArray[SM_SWITCH] =
    {   {  0, "Sine",              SetTimeSetSelect  },
        {  1, "Triangle",          SetTimeSetSelect  },
        {  2, "Sawtooth",          SetTimeSetSelect  },
        {  3, "Pulse",             SetTimeSetSelect  },
        {  4, "Square",            SetTimeSetSelect  },
        {  5, "Noise VCA",         SetTimeSetSelect  },
        {  6, "Noise VCF",         SetTimeSetSelect  },
        {  7, "Sawtooth Dir",      SawtoothDirection },
        {  8, "Mod Sine",          SetModSwitch      },
        {  9, "Mod Triangle",      SetModSwitch      },
        { 10, "Mod Sawtooth",      SetModSwitch      },
        { 11, "Mod Pulse   ",      SetModSwitch      },
        { 12, "Mod Square",        SetModSwitch      },
        { 13, "White/!Pink noise", SetWhiteNoise     },
        { 14, "Noise filter 0",    SetNoiseFilter    },
        { 15, "Noise filter 1",    SetNoiseFilter    },
        { 16, "Switch #17",        nullptr           },
        { 17, "Switch #18",        nullptr           },
        { 18, "Switch #19",        nullptr           },
        { 19, "Switch #20",        nullptr           },
        { 20, "LFO Range -",       SetPlusRangeLFO   },
        { 21, "LFO Range +",       SetMinusRangeLFO  },
        { 22, "Tune/Reset",        TuneReset         },
        { 23, "Switch #24",        nullptr           },
    };






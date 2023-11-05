//#######################################################################
// Module:     SyntConfit.ino
// Descrption: Synthesizer midi configuration
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include "config.h"
#include "SynthFront.h"

//########################################################
void SetOscMixers (uint8_t ch, float control)
    {
    SynthFront.SetMaxLevel (ch, control);
    }

//########################################################
void SetOscSustain (uint8_t ch, float control)
    {
    SynthFront.SetSustainLevel (ch - 8, control);
    }

//########################################################
void SetOscTimeSwitch (uint8_t ch, uint8_t state)
    {
    SynthFront.ChannelSelect (ch, state);
    }

//########################################################
void SetModSwitch (uint8_t ch, uint8_t state)
    {
    SynthFront.SelectWaveLFO (ch - 8, state);
    }

//########################################################
void SetAttck (uint8_t ch, float data)
    {
    SynthFront.SetAttack (data);
    }

//########################################################
void  SetDecay (uint8_t ch, float data)
    {
    SynthFront.SetDecay (data);
    }

//########################################################
void  SetSustain (uint8_t ch, float data)
    {
    if ( data == 0.0 )
        data =-1.0;
    SynthFront.SetSustainTime(data);
    }

//########################################################
void  SetRelease (uint8_t ch, float data)
    {
    SynthFront.SetRelease (data);
    }

//########################################################
void Modulation (uint8_t ch, float data)
    {
    SynthFront.SetLevelLFO (data);
    }

//########################################################
void SetLfoFreq (uint8_t ch, float data)
    {
    SynthFront.FreqSelectLFO (0, data);
    }

//########################################################
void PitchBend (uint8_t ch, float data)
    {
    SynthFront.PitchBend (data);
    }

//########################################################
void SetPlusRangeLFO (uint8_t ch, uint8_t state)
    {
    SynthFront.LFOrange (true);
    }

//########################################################
void SetMinusRangeLFO (uint8_t ch, uint8_t state)
    {
    SynthFront.LFOrange (false);
    }

//########################################################
MIDI_VALUE_MAP    FaderMapArray[SM_FADER] =
    {   {  0, "Sine Max ",         SetOscMixers,  0.7874 },         // 0-100%
        {  1, "Triangle Max ",     SetOscMixers,  0.7874 },
        {  2, "Sqiare Max ",       SetOscMixers,  0.7874 },
        {  3, "Sawtooth Max",      SetOscMixers,  0.7874 },
        {  4, "Pulse Max",         SetOscMixers,  0.7874 },
        {  5, "N ",                nullptr,       0.7874 },
        {  6, "N ",                nullptr,       0.7874 },
        {  7, "N ",                nullptr,       0.7874 },
        {  8, "Sine Sustain ",     SetOscSustain, 0.7874 },
        {  9, "Triangle Sustain ", SetOscSustain, 0.7874 },
        { 10, "Sqiare Sustain ",   SetOscSustain, 0.7874 },
        { 11, "Sawtooth Sustain",  SetOscSustain, 0.7874 },
        { 12, "Pulse Sustain",     SetOscSustain, 0.7874 },
        { 13, "Max ",              nullptr,       0.7874 },
        { 14, "Max ",              nullptr,       0.7874 },
        { 15, "Max ",              nullptr,       0.7874 },
    };

//########################################################
MIDI_VALUE_MAP    KnobMapArray[SM_CONTROL] =
    {   {  0, "Attack",     SetAttck,   10     },
        {  1, "Decay",      SetDecay,   10     },
        {  2, "Sustain",    SetSustain, 10     },
        {  3, "Release",    SetRelease, 10     },
        {  4, "N ",         nullptr,    0.7874 },
        {  5, "N ",         nullptr,    0.7874 },
        {  6, "N ",         nullptr,    0.7874 },
        {  7, "LFO Freq ",  SetLfoFreq, 0.7874 },
        {  8, "N ",         nullptr,    0.7874 },
        {  9, "N ",         nullptr,    0.7874 },
        { 10, "N ",         nullptr,    0.7874 },
        { 11, "N ",         nullptr,    0.7874 },
        { 12, "N ",         nullptr,    0.7874 },
        { 14, "N ",         nullptr,    0.7874 },
        { 14, "N ",         nullptr,    0.7874 },
        { 15, "N ",         nullptr,    0.7874 },
    };

//########################################################
MIDI_VALUE_MAP PitchMapArray[SM_MOD] =
    {   { 0, "Modulation",  Modulation, 0.78740 },
        { 1, "Pitch Bend",  PitchBend,  0.00305 },
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






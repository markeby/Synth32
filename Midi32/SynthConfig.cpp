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
void SetOscMixers (byte ch, float control)
    {
    SynthFront.SetOscMaxLevel (ch, control);
    }

//########################################################
void SetOscSustain (byte ch, float control)
    {
    SynthFront.SetOscSustainLevel (ch - 8, control);
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
void SetAttckTime (byte ch, float data)
    {
    SynthFront.SetOscAttackTime (data);
    }

//########################################################
void  SetDecayTime (byte ch, float data)
    {
    SynthFront.SetOscDecayTime (data);
    }

//########################################################
void  SetSustainTime (byte ch, float data)
    {
    if ( data == 0.0 )
        data =-1.0;
    SynthFront.SetOscSustainTime(data);
    }

//########################################################
void  SetReleaseTime (byte ch, float data)
    {
    SynthFront.SetOscReleaseTime (data);
    }

//########################################################
void Modulation (byte ch, float data)
    {
    SynthFront.SetLevelLFO (data);
    }

//########################################################
void SetLfoFreq (byte ch, float data)
    {
    SynthFront.FreqSelectLFO (0, data);
    }

//########################################################
void PitchBend (byte ch, float data)
    {
    SynthFront.PitchBend (data);
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
    {   {  0, "Sine Max ",         SetOscMixers,  PERS_SCALER },         // 0-100%
        {  1, "Triangle Max ",     SetOscMixers,  PERS_SCALER },
        {  2, "Sqiare Max ",       SetOscMixers,  PERS_SCALER },
        {  3, "Sawtooth Max",      SetOscMixers,  PERS_SCALER },
        {  4, "Pulse Max",         SetOscMixers,  PERS_SCALER },
        {  5, "N ",                nullptr,       PERS_SCALER },
        {  6, "N ",                nullptr,       PERS_SCALER },
        {  7, "N ",                nullptr,       PERS_SCALER },
        {  8, "Sine Sustain ",     SetOscSustain, PERS_SCALER },
        {  9, "Triangle Sustain ", SetOscSustain, PERS_SCALER },
        { 10, "Sqiare Sustain ",   SetOscSustain, PERS_SCALER },
        { 11, "Sawtooth Sustain",  SetOscSustain, PERS_SCALER },
        { 12, "Pulse Sustain",     SetOscSustain, PERS_SCALER },
        { 13, "Max ",              nullptr,       PERS_SCALER },
        { 14, "Max ",              nullptr,       PERS_SCALER },
        { 15, "Max ",              nullptr,       PERS_SCALER },
    };

//########################################################
MIDI_VALUE_MAP    KnobMapArray[SM_CONTROL] =
    {   {  0, "Attack",     SetAttckTime,   TIME_MULT   },
        {  1, "Decay",      SetDecayTime,   TIME_MULT   },
        {  2, "Sustain",    SetSustainTime, TIME_MULT   },
        {  3, "Release",    SetReleaseTime, TIME_MULT   },
        {  4, "N ",         nullptr,        PERS_SCALER },
        {  5, "N ",         nullptr,        PERS_SCALER },
        {  6, "N ",         nullptr,        PERS_SCALER },
        {  7, "LFO Freq ",  SetLfoFreq,     PERS_SCALER },
        {  8, "N ",         nullptr,        PERS_SCALER },
        {  9, "N ",         nullptr,        PERS_SCALER },
        { 10, "N ",         nullptr,        PERS_SCALER },
        { 11, "N ",         nullptr,        PERS_SCALER },
        { 12, "N ",         nullptr,        PERS_SCALER },
        { 14, "N ",         nullptr,        PERS_SCALER },
        { 14, "N ",         nullptr,        PERS_SCALER },
        { 15, "N ",         nullptr,        PERS_SCALER },
    };

//########################################################
MIDI_VALUE_MAP PitchMapArray[SM_MOD] =
    {   { 0, "Modulation",  Modulation, PERS_SCALER },
        { 1, "Pitch Bend",  PitchBend,  0.00305     },
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






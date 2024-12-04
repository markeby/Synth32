//#######################################################################
// Module:     Controls.cpp
// Descrption: Synthesizer midi control routing
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include "../Common/SynthCommon.h"
#include "config.h"
#include "FrontEnd.h"
#include "I2Cmessages.h"
#include "SerialMonitor.h"
#include "SoftLFO.h"

//########################################################
static void SetMaxLevel (byte ch, byte data)
    {
    SynthFront.SetMaxLevel (ch, data);
    }

//########################################################
static void SetSustain (byte ch, byte data)
    {
    SynthFront.SetSustainLevel (ch, data);
    }

//########################################################
static void SetTimeSetSelect (byte ch, byte state)
    {
    SynthFront.ChannelSetSelect (ch, state);
    }

//########################################################
static void SetModVCA (byte ch, byte state)
    {
    SynthFront.SelectWaveVCA (ch, state);
    }

//########################################################
static void SetSrcVCF (byte ch, byte state)
    {
    SynthFront.SelectWaveVCF (ch, state);
    }

//########################################################
static void SetAttckTime (byte ch, byte data)
    {
    SynthFront.SetAttackTime (data);
    }

//########################################################
static void SetDecayTime (byte ch, byte data)
    {
    SynthFront.SetDecayTime (data);
    }

//########################################################
static void SetSustainTime (byte ch, byte data)
    {
    SynthFront.SetSustainTime (data);
    }

//########################################################
static void SetReleaseTime (byte ch, byte data)
    {
    SynthFront.SetReleaseTime (data);
    }

//########################################################
static void SawtoothDirection (byte ch, byte data)
    {
    SynthFront.SawtoothDirection (data != 0);
    }

//########################################################
static void PulseWidth (byte ch, byte data)
    {
    if ( data == 0 )
        data = 1;
    SynthFront.SetPulseWidth (data);
    }

//########################################################
static void FreqLFO (byte ch, byte data)
    {
    SynthFront.FreqLFO (ch, data);
    }

//########################################################
static void TuneReset (byte ch, byte data)
    {
    if ( data )
        Monitor.Tuning ();
    else
        Monitor.Reset ();
    }

//########################################################
static void PageAdvance (byte ch, byte data)
    {
    if ( data )
        DisplayMessage.PageAdvance();
    else
        SynthFront.SetClearKeyRed (0x64);
    }

//########################################################
static void NoiseSet (byte ch, byte data)
    {
   printf("NOISE -> %X  %X\n", ch, data);
    SynthFront.SetNoise (ch, data);
    }

//########################################################
MIDI_XL_MAP    XlMapArray[SIZE_CL_MAP] =
    {   {    0,    0, "N ",                     nullptr           },    // 01  30  xx
        {    1,    0, "N ",                     nullptr           },    // 01  31  xx
        {    2,    0, "N ",                     nullptr           },    // 01  32  xx
        {    3,    0, "N ",                     nullptr           },    // 01  33  xx
        {    4,    0, "N ",                     nullptr           },    // 01  34  xx
        {    5,    0, "N ",                     nullptr           },    // 01  35  xx
        {    6,    0, "N ",                     nullptr           },    // 01  36  xx
        {    7,    0, "N ",                     nullptr           },    // 01  37  xx
        {    8,    0, "N ",                     nullptr           },    // 01  38  xx
        {    9,    0, "N ",                     nullptr           },    // 01  39  xx
        {   10,    0, "N ",                     nullptr           },    // 01  3A  xx
        {   11,    0, "N ",                     nullptr           },    // 01  3B  xx
        {   12,    0, "N ",                     nullptr           },    // 01  3C  xx
        {   13,    0, "N ",                     nullptr           },    // 01  3D  xx
        {   14,    0, "N ",                     nullptr           },    // 01  3E  xx
        {   15,    0, "N ",                     nullptr           },    // 01  3F  xx
        {    0,    0, "Attack",                 SetAttckTime      },    // 01  40  xx
        {    1,    0, "Decay",                  SetDecayTime      },    // 01  41  xx
        {    2,    0, "Sustain",                SetSustainTime    },    // 01  42  xx
        {    3,    0, "Release",                SetReleaseTime    },    // 01  43  xx
        {   20,    0, "N ",                     nullptr           },    // 01  44  xx
        {   21,    0, "N ",                     nullptr           },    // 01  45  xx
        {   22,    0, "N ",                     nullptr           },    // 01  46  xx
        {  127, 0x1D, "Pulse Width",            PulseWidth        },    // 01  47  xx
        {    0,    0, "Sine max level",         SetMaxLevel       },    // 01  48  xx
        {    1,    0, "Triangle max level",     SetMaxLevel       },    // 01  49  xx
        {    2,    0, "Sqiare max level",       SetMaxLevel       },    // 01  4A  xx
        {    3,    0, "Sawtooth max level",     SetMaxLevel       },    // 01  4B  xx
        {    4,    0, "Pulse max level",        SetMaxLevel       },    // 01  4C  xx
        {   29,    0, "N ",                     nullptr           },    // 01  4D  xx
        {   30,    0, "N ",                     nullptr           },    // 01  4E  xx
        {   31,    0, "N ",                     nullptr           },    // 01  4F  xx
        {    0,    0, "Sine",                   SetTimeSetSelect  },    // 01  50  xx
        {    1,    0, "Triangle",               SetTimeSetSelect  },    // 01  51  xx
        {    2,    0, "Sawtooth",               SetTimeSetSelect  },    // 01  52  xx
        {    3,    0, "Pulse",                  SetTimeSetSelect  },    // 01  53  xx
        {    4,    0, "Square",                 SetTimeSetSelect  },    // 01  54  xx
        { 0x55, 0x0D, "Sawtooth Dir",           SawtoothDirection },    // 01  55  xx
        { 0x56,                0, "Dual Zone",          nullptr           },    // 01  56  xx
        {   39,    0, "N ",                     nullptr           },    // 01  57  xx
        {   DUCT_BLUE,         0, "0-1 Blue ",          NoiseSet          },    // 01  58  xx
        {   DUCT_WHITE,        0, "0-1 White",          NoiseSet          },    // 01  59  xx
        {   DUCT_PINK,         0, "0-1 Pink ",          NoiseSet          },    // 01  5A  xx
        {   DUCT_RED,          0, "0-1 Red  ",          NoiseSet          },    // 01  5B  xx
        {   0x80 | DUCT_BLUE,  0, "2 Blue ",            NoiseSet          },    // 01  5C  xx
        {   0x80 | DUCT_WHITE, 0, "2 White",            NoiseSet          },    // 01  5D  xx
        {   0x80 | DUCT_PINK,  0, "2 Pink ",            NoiseSet          },    // 01  5E  xx
        {   0x80 | DUCT_RED,   0, "2 Red  ",            NoiseSet          },    // 01  5F  xx
        {   47,    0, "N ",                     nullptr           },    // 01  60  xx
        {   47,    0, "N ",                     nullptr           },    // 01  61  xx
        {   50,    0, "N ",                     nullptr           },    // 01  62  xx
        {   51,    0, "N ",                     nullptr           },    // 01  63  xx
        { 0x64, 0x3F, "Page Advance",           PageAdvance       },    // 01  64  xx
        {   53,    0, "N ",                     nullptr           },    // 01  65  xx
        {   54,    0, "N ",                     nullptr           },    // 01  66  xx
        {   55,    0, "N ",                     nullptr           },    // 01  67  xx
    };

LED_NOTE_MAP SendA[]     = { 13, 29, 45, 61, 77, 93, 109, 125 };
LED_NOTE_MAP SendB[]     = { 14, 30, 46, 62, 78, 94, 110, 126 };
LED_NOTE_MAP PanDevice[] = { 15, 31, 47, 63, 79, 95, 111, 127 };

//########################################################
//########################################################
MIDI_MAP    FaderMapArray[] =
    {   {  0, "Sine Sustain",             SetSustain        },          // 01  07  xx
        {  1, "Triangle Sustain",         SetSustain        },          // 02  07  xx
        {  2, "Sawtooth Sustain",         SetSustain        },          // 03  07  xx
        {  3, "Pulse Sustain",            SetSustain        },          // 04  07  xx
        {  4, "Noise Sustain",            SetSustain       },           // 05  07  xx
        {  5, "N ",                       nullptr           },          // 06  07  xx
        {  6, "N ",                       nullptr           },          // 07  07  xx
        {  7, "N ",                       nullptr           },          // 08  07  xx
        {  8, "N ",                       nullptr           },          // 09  07  xx
        {  9, "N ",                       nullptr           },          // 0A  07  xx
        { 10, "N ",                       nullptr           },          // 0B  07  xx
        { 11, "N ",                       nullptr           },          // 0C  07  xx
        { 12, "N ",                       nullptr           },          // 0D  07  xx
        { 13, "N ",                       nullptr           },          // 0E  07  xx
        { 14, "N ",                       nullptr           },          // 0F  07  xx
        { 15, "N ",                       nullptr           },          // 10  07  xx
    };

//########################################################
MIDI_MAP    KnobMapArray[] =
    {   {  0, "Soft LFO freq",            FreqLFO          },           //  01  0A  xx
        {  0, "Hard LFO freq",            FreqLFO          },           //  02  0A  xx
        {  1, "N ",                       nullptr          },           //  03  0A  xx
        {  1, "N ",                       nullptr          },           //  04  0A  xx
        {  4, "N ",                       nullptr          },           //  05  0A  xx
        {  5, "N ",                       nullptr          },           //  06  0A  xx
        {  6, "N ",                       nullptr          },           //  07  0A  xx
        {  7, "N ",                       nullptr          },           //  08  0A  xx
        {  8, "N ",                       nullptr          },           //  09  0A  xx
        {  9, "N ",                       nullptr          },           //  0A  0A  xx
        { 10, "N ",                       nullptr          },           //  0B  0A  xx
        { 11, "N ",                       nullptr          },           //  0C  0A  xx
        { 12, "N ",                       nullptr          },           //  0D  0A  xx
        { 14, "N ",                       nullptr          },           //  0E  0A  xx
        { 14, "N ",                       nullptr          },           //  0F  0A  xx
        { 15, "N ",                       nullptr          },           //  10  0A  xx
    };

//########################################################
MIDI_MAP SwitchMapArray[] =
    {   {  0, "VCA Mod Sine",             SetModVCA         },          //  01  10  xx
        {  1, "VCA Mod Triangle",         SetModVCA         },          //  01  11  xx
        {  2, "VCA Mod Sawtooth",         SetModVCA         },          //  01  12  xx
        {  3, "VCA Mod Pulse   ",         SetModVCA         },          //  01  13  xx
        {  4, "VCA Mod Noise",            SetModVCA         },          //  01  14  xx
        {  5, "Switch f6",                nullptr           },          //  01  15  xx
        {  6, "Switch f7",                nullptr           },          //  01  16  xx
        {  7, "Switch f8",                nullptr           },          //  01  17  xx
        {  0, "VCF Src Mod Sine",         SetSrcVCF         },          //  01  18  xx
        {  1, "VCF Src Mod Triangle",     SetSrcVCF         },          //  01  19  xx
        {  2, "VCF Src Mod Sawtooth",     SetSrcVCF         },          //  01  1A  xx
        {  3, "VCF Src Mod Pulse   ",     SetSrcVCF         },          //  01  1B  xx
        {  4, "VCF Src Mod Noise",        SetSrcVCF         },          //  01  1C  xx
        { 13, "Switch #14",               nullptr           },          //  01  1D  xx
        { 14, "Switch #15",               nullptr           },          //  01  1E  xx
        { 15, "Switch #16",               nullptr           },          //  01  1F  xx
        { 24, "Switch #25",               nullptr           },          //  01  72  xx
        { 24, "Switch #25",               nullptr           },          //  01  73  xx
        { 20, "Switch #21",               nullptr           },          //  01  74  xx
        { 21, "Switch #22",               nullptr           },          //  01  75  xx
        { 22, "Tune/Reset",               TuneReset         },          //  01  76  xx
        { 23, "Switch #24",               nullptr           },          //  01  77  xx
     };















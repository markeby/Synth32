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
#include "SineWave.h"

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
static void SetModVCA (uint8_t ch, uint8_t state)
    {
    SynthFront.SelectWaveVCA (ch, state);
    }

//########################################################
static void SetSrcVCF (uint8_t ch, uint8_t state)
    {
    SynthFront.SelectWaveVCF (ch, state);
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
    SineWave.SetFrequency (data);
    }

//########################################################
static void SetLfoFreq (uint8_t ch, uint8_t data)
    {
    SynthFront.FreqSelectLFO (0, data);
    }

//########################################################
static void SetPlusRangeLFO (uint8_t ch, uint8_t state)
    {
    SynthFront.LFOrange (true, state);
    }

//########################################################
static void SetMinusRangeLFO (uint8_t ch, uint8_t state)
    {
    SynthFront.LFOrange (false, state);
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
MIDI_XL_MAP    XlMapArray[SIZE_CL_MAP] =
    {   {    0,    0, "N ",                     nullptr           },  // 01  30  xx
        {    1,    0, "N ",                     nullptr           },  // 01  31  xx
        {    2,    0, "N ",                     nullptr           },  // 01  32  xx
        {    3,    0, "N ",                     nullptr           },  // 01  33  xx
        {    4,    0, "N ",                     nullptr           },  // 01  34  xx
        {    5,    0, "N ",                     nullptr           },  // 01  35  xx
        {    6,    0, "N ",                     nullptr           },  // 01  36  xx
        {  125, 0x0F, "SoftLFO Freq",           SoftLfoFreq       },  // 01  37  xx
        {    8,    0, "N ",                     nullptr           },  // 01  38  xx
        {    9,    0, "N ",                     nullptr           },  // 01  39  xx
        {   10,    0, "N ",                     nullptr           },  // 01  3A  xx
        {   11,    0, "N ",                     nullptr           },  // 01  3B  xx
        {   12,    0, "N ",                     nullptr           },  // 01  3C  xx
        {   13,    0, "N ",                     nullptr           },  // 01  3D  xx
        {   14,    0, "N ",                     nullptr           },  // 01  3E  xx
        {  126, 0x0F, "LFO Freq ",              SetLfoFreq        },  // 01  3F  xx
        {    0,    0, "Attack",                 SetAttckTime      },  // 01  40  xx
        {    1,    0, "Decay",                  SetDecayTime      },  // 01  41  xx
        {    2,    0, "Sustain",                SetSustainTime    },  // 01  42  xx
        {    3,    0, "Release",                SetReleaseTime    },  // 01  43  xx
        {   20,    0, "N ",                     nullptr           },  // 01  44  xx
        {   21,    0, "N ",                     nullptr           },  // 01  45  xx
        {   22,    0, "N ",                     nullptr           },  // 01  46  xx
        {  127, 0x1D, "Pulse Width",            PulseWidth        },  // 01  47  xx
        {    0,    0, "Sine max level",         SetMaxLevel       },  // 01  48  xx
        {    1,    0, "Triangle max level",     SetMaxLevel       },  // 01  49  xx
        {    2,    0, "Sqiare max level",       SetMaxLevel       },  // 01  4A  xx
        {    3,    0, "Sawtooth max level",     SetMaxLevel       },  // 01  4B  xx
        {    4,    0, "Pulse max level",        SetMaxLevel       },  // 01  4C  xx
        {   29,    0, "N ",                     nullptr           },  // 01  4D  xx
        {   30,    0, "N ",                     nullptr           },  // 01  4E  xx
        {   31,    0, "N ",                     nullptr           },  // 01  4F  xx
        {    0,    0, "Sine",                   SetTimeSetSelect  },  // 01  50  xx
        {    1,    0, "Triangle",               SetTimeSetSelect  },  // 01  51  xx
        {    2,    0, "Sawtooth",               SetTimeSetSelect  },  // 01  52  xx
        {    3,    0, "Pulse",                  SetTimeSetSelect  },  // 01  53  xx
        {    4,    0, "Square",                 SetTimeSetSelect  },  // 01  54  xx
        { 0x55, 0x0D, "Sawtooth Dir",           SawtoothDirection },  // 01  55  xx
        { 0x56, 0x0D, "White/!Pink noise",      SetWhiteNoise     },  // 01  56  xx
        {   39,    0, "N ",                     nullptr           },  // 01  57  xx
        {   40,    0, "N ",                     nullptr           },  // 01  58  xx
        {   41,    0, "N ",                     nullptr           },  // 01  59  xx
        {   42,    0, "N ",                     nullptr           },  // 01  5A  xx
        {   43,    0, "N ",                     nullptr           },  // 01  5B  xx
        {   44,    0, "N ",                     nullptr           },  // 01  5C  xx
        {   45,    0, "N ",                     nullptr           },  // 01  5D  xx
        {   46,    0, "N ",                     nullptr           },  // 01  5E  xx
        {   47,    0, "N ",                     nullptr           },  // 01  5F  xx
        { 0x60, 0x3F, "LFO Range -",            SetPlusRangeLFO   },  // 01  60  xx
        { 0x61, 0x3F, "LFO Range +",            SetMinusRangeLFO  },  // 01  61  xx
        {   50,    0, "N ",                     nullptr           },  // 01  62  xx
        {   51,    0, "N ",                     nullptr           },  // 01  63  xx
        {   52,    0, "N ",                     nullptr           },  // 01  64  xx
        {   53,    0, "N ",                     nullptr           },  // 01  65  xx
        {   54,    0, "N ",                     nullptr           },  // 01  66  xx
        {   55,    0, "N ",                     nullptr           },  // 01  67  xx
    };

LED_NOTE_MAP SendA[]     = { 13, 29, 45, 61, 77, 93, 109, 125 };
LED_NOTE_MAP SendB[]     = { 14, 30, 46, 62, 78, 94, 110, 126 };
LED_NOTE_MAP PanDevice[] = { 15, 31, 47, 63, 79, 95, 111, 127 };

//########################################################
//########################################################
MIDI_MAP    FaderMapArray[] =
    {   {  0, "Sine Sustain",             SetSustain        },    // 01  07  xx
        {  1, "Triangle Sustain",         SetSustain        },    // 02  07  xx
        {  2, "Sqiare Sustain",           SetSustain        },    // 03  07  xx
        {  3, "Sawtooth Sustain",         SetSustain        },    // 04  07  xx
        {  4, "Pulse Sustain",            SetSustain        },    // 05  07  xx
        {  5, "Noise max level",          SetMaxLevel       },    // 06  07  xx
        {  6, "Noise Sustain",            SetSustain        },    // 07  07  xx
        {  7, "N ",                       nullptr           },    // 08  07  xx
        {  8, "N ",                       nullptr           },    // 09  07  xx
        {  9, "N ",                       nullptr           },    // 0A  07  xx
        { 10, "N ",                       nullptr           },    // 0B  07  xx
        { 11, "N ",                       nullptr           },    // 0C  07  xx
        { 12, "N ",                       nullptr           },    // 0D  07  xx
        { 13, "N ",                       nullptr           },    // 0E  07  xx
        { 14, "N ",                       nullptr           },    // 0F  07  xx
        { 15, "N ",                       nullptr           },    // 10  07  xx
    };

//########################################################
MIDI_MAP    KnobMapArray[] =
    {   {  0, "N ",                       nullptr          },
        {  1, "N ",                       nullptr          },               //  01  0A  xx
        {  2, "N ",                       nullptr          },               //  02  0A  xx
        {  3, "N ",                       nullptr          },               //  03  0A  xx
        {  4, "Pulse Width",              PulseWidth       },               //  04  0A  xx
        {  5, "N ",                       nullptr          },               //  05  0A  xx
        {  6, "N ",                       nullptr          },               //  06  0A  xx
        {  7, "N ",                       nullptr          },               //  07  0A  xx
        {  8, "N ",                       nullptr          },               //  08  0A  xx
        {  9, "N ",                       nullptr          },               //  09  0A  xx
        { 10, "N ",                       nullptr          },               //  0A  0A  xx
        { 11, "N ",                       nullptr          },               //  0B  0A  xx
        { 12, "N ",                       nullptr          },               //  0C  0A  xx
        { 14, "N ",                       nullptr          },               //  0D  0A  xx
        { 14, "N ",                       nullptr          },               //  0E  0A  xx
        { 15, "N ",                       nullptr          },               //  0F  0A  xx
    };

//########################################################
MIDI_MAP SwitchMapArray[] =
    {   {  0, "VCA Mod Sine",             SetModVCA         },         //  01  10  xx
        {  1, "VCA Mod Triangle",         SetModVCA         },         //  01  11  xx
        {  2, "VCA Mod Sawtooth",         SetModVCA         },         //  01  12  xx
        {  3, "VCA Mod Pulse   ",         SetModVCA         },         //  01  13  xx
        {  4, "VCA Mod Square",           SetModVCA         },         //  01  14  xx
        {  5, "Switch f6",                nullptr           },         //  01  15  xx
        {  6, "Switch f7",                nullptr           },         //  01  16  xx
        {  7, "Switch f8",                nullptr           },         //  01  17  xx
        {  0, "VCF Src Mod Sine",         SetSrcVCF         },         //  01  18  xx
        {  1, "VCF Src Mod Triangle",     SetSrcVCF         },         //  01  19  xx
        {  2, "VCF Src Mod Sawtooth",     SetSrcVCF         },         //  01  1A  xx
        {  3, "VCF Src Mod Pulse   ",     SetSrcVCF         },         //  01  1B  xx
        {  4, "VCF Src Mod Square",       SetSrcVCF         },         //  01  1C  xx
        { 13, "Switch #14",               nullptr           },         //  01  1D  xx
        { 14, "Switch #15",               nullptr           },         //  01  1E  xx
        { 15, "Switch #16",               nullptr           },         //  01  1F  xx
        { 16, "Switch #17",               nullptr           },         //
        { 17, "Switch #18",               nullptr           },         //
        { 18, "Switch #19",               nullptr           },         //
        { 19, "Switch #20",               nullptr           },         //
        { 20, "Switch #21",               nullptr           },         //  01  74  xx
        { 21, "Switch #22",               nullptr           },         //  01  75  xx
        { 22, "Tune/Reset",               TuneReset         },         //  01  76  xx
        { 23, "Switch #24",               nullptr           },         //  01  77  xx
        { 24, "Switch #25",               nullptr           },         //  01  72  xx
     };















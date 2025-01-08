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
static void SetMaxLevel (short ch, short data)
    {
    SynthFront.SetMaxLevel (ch, data);
    }

//########################################################
static void SetSustain (short ch, short data)
    {
    SynthFront.SetSustainLevel (ch, data);
    }

//########################################################
static void SetTimeSetSelect (short ch, short state)
    {
    SynthFront.ChannelSetSelect (ch, state);
    }

//########################################################
static void ToggleModVCA (short ch)
    {
    SynthFront.ToggleSelectModVCA (ch);
    }

//########################################################
static void ToggleModVCO (short ch)
    {
    SynthFront.ToggleSelectModVCO (ch);
    }

//########################################################
static void ToggleRampDir (short ch)
    {
    SynthFront.ToggleRampDir ();
    }

//########################################################
static void SetAttckTime (short ch, short data)
    {
    SynthFront.SetAttackTime (data);
    }

//########################################################
static void SetDecayTime (short ch, short data)
    {
    SynthFront.SetDecayTime (data);
    }

//########################################################
static void SetReleaseTime (short ch, short data)
    {
    SynthFront.SetReleaseTime (data);
    }

//########################################################
static void SawtoothDirection (short ch, short data)
    {
    SynthFront.SawtoothDirection (data != 0);
    }

//########################################################
static void PulseWidth (short ch, short data)
    {
    if ( data == 0 )
        data = 1;
    SynthFront.SetPulseWidth (data);
    }

//########################################################
static void FreqLFO (short ch, short data)
    {
    SynthFront.FreqLFO (ch, data);
    }

//########################################################
static void TuneReset (short ch)
    {
    if ( SynthFront.IsInTuning () )
        Monitor.Reset ();
    else
        Monitor.Tuning ();
    }

//########################################################
static void PageAdvance (short ch, short data)
    {
    if ( data )
        DisplayMessage.PageAdvance();
    else
        SynthFront.SetClearKeyRed (0x64);
    }

//########################################################
static void NoiseSet (short ch, short data)
    {
    SynthFront.SetNoise (ch, data);
    }

//########################################################
static void DualZone (short ch, short data)
    {
    SynthFront.DualZone (ch, data);
    }

//########################################################
MIDI_XL_MAP    XlMapArray[SIZE_CL_MAP] =
    {   {    0,                0, "N ",                 nullptr           },    // 01  30  xx
        {    1,                0, "N ",                 nullptr           },    // 01  31  xx
        {    2,                0, "N ",                 nullptr           },    // 01  32  xx
        {    3,                0, "N ",                 nullptr           },    // 01  33  xx
        {    4,                0, "N ",                 nullptr           },    // 01  34  xx
        {    5,                0, "N ",                 nullptr           },    // 01  35  xx
        {    6,                0, "N ",                 nullptr           },    // 01  36  xx
        {    7,                0, "N ",                 nullptr           },    // 01  37  xx
        {    8,                0, "N ",                 nullptr           },    // 01  38  xx
        {    9,                0, "N ",                 nullptr           },    // 01  39  xx
        {   10,                0, "N ",                 nullptr           },    // 01  3A  xx
        {   11,                0, "N ",                 nullptr           },    // 01  3B  xx
        {   12,                0, "N ",                 nullptr           },    // 01  3C  xx
        {   13,                0, "N ",                 nullptr           },    // 01  3D  xx
        {   14,                0, "N ",                 nullptr           },    // 01  3E  xx
        {   15,                0, "N ",                 nullptr           },    // 01  3F  xx
        {    0,                0, "Attack",             SetAttckTime      },    // 01  40  xx
        {    1,                0, "Decay",              SetDecayTime      },    // 01  41  xx
        {    2,                0, "Release",            SetReleaseTime    },    // 01  42  xx
        {    3,                0, "N ",                 nullptr           },    // 01  43  xx
        {   20,                0, "N ",                 nullptr           },    // 01  44  xx
        {   21,                0, "N ",                 nullptr           },    // 01  45  xx
        {   22,                0, "N ",                 nullptr           },    // 01  46  xx
        {  127,             0x1D, "Pulse Width",        PulseWidth        },    // 01  47  xx
        {    0,                0, "Sine max level",     SetMaxLevel       },    // 01  48  xx
        {    1,                0, "Triangle max level", SetMaxLevel       },    // 01  49  xx
        {    2,                0, "Sqiare max level",   SetMaxLevel       },    // 01  4A  xx
        {    3,                0, "Sawtooth max level", SetMaxLevel       },    // 01  4B  xx
        {    4,                0, "Pulse max level",    SetMaxLevel       },    // 01  4C  xx
        {   29,                0, "N ",                 nullptr           },    // 01  4D  xx
        {   30,                0, "N ",                 nullptr           },    // 01  4E  xx
        {   31,                0, "N ",                 nullptr           },    // 01  4F  xx
        {    0,                0, "Sine",               SetTimeSetSelect  },    // 01  50  xx
        {    1,                0, "Triangle",           SetTimeSetSelect  },    // 01  51  xx
        {    2,                0, "Sawtooth",           SetTimeSetSelect  },    // 01  52  xx
        {    3,                0, "Pulse",              SetTimeSetSelect  },    // 01  53  xx
        {    4,                0, "Square",             SetTimeSetSelect  },    // 01  54  xx
        { 0x55,             0x0D, "Sawtooth Dir",       SawtoothDirection },    // 01  55  xx
        {    1,                0, "Dual Zone 1",        DualZone          },    // 01  56  xx
        {    2,                0, "Dual Zone 2",        DualZone          },    // 01  57  xx
        {   DUCT_BLUE,         0, "0-1 Blue ",          NoiseSet          },    // 01  58  xx
        {   DUCT_WHITE,        0, "0-1 White",          NoiseSet          },    // 01  59  xx
        {   DUCT_PINK,         0, "0-1 Pink ",          NoiseSet          },    // 01  5A  xx
        {   DUCT_RED,          0, "0-1 Red  ",          NoiseSet          },    // 01  5B  xx
        {   0x80 | DUCT_BLUE,  0, "2 Blue ",            NoiseSet          },    // 01  5C  xx
        {   0x80 | DUCT_WHITE, 0, "2 White",            NoiseSet          },    // 01  5D  xx
        {   0x80 | DUCT_PINK,  0, "2 Pink ",            NoiseSet          },    // 01  5E  xx
        {   0x80 | DUCT_RED,   0, "2 Red  ",            NoiseSet          },    // 01  5F  xx
        {   47,                0, "N ",                 nullptr           },    // 01  60  xx
        {   47,                0, "N ",                 nullptr           },    // 01  61  xx
        {   50,                0, "N ",                 nullptr           },    // 01  62  xx
        {   51,                0, "N ",                 nullptr           },    // 01  63  xx
        { 0x64,             0x3F, "Page Advance",       PageAdvance       },    // 01  64  xx
        {   53,                0, "N ",                 nullptr           },    // 01  65  xx
        {   54,                0, "N ",                 nullptr           },    // 01  66  xx
        {   55,                0, "N ",                 nullptr           },    // 01  67  xx
    };

LED_NOTE_MAP SendA[]     = { 13, 29, 45, 61, 77, 93, 109, 125 };
LED_NOTE_MAP SendB[]     = { 14, 30, 46, 62, 78, 94, 110, 126 };
LED_NOTE_MAP PanDevice[] = { 15, 31, 47, 63, 79, 95, 111, 127 };

//########################################################
//########################################################
MIDI_MAP FaderMapArray[] =
    {   {  0, "Sine Sustain",           SetSustain  },  // 01  07  xx
        {  1, "Triangle Sustain",       SetSustain  },  // 02  07  xx
        {  2, "Sawtooth Sustain",       SetSustain  },  // 03  07  xx
        {  3, "Pulse Sustain",          SetSustain  },  // 04  07  xx
        {  4, "Noise Sustain",          SetSustain  },  // 05  07  xx
        {  5, "N ",                     nullptr     },  // 06  07  xx
        {  6, "N ",                     nullptr     },  // 07  07  xx
        {  7, "N ",                     nullptr     },  // 08  07  xx
        {  8, "N ",                     nullptr     },  // 09  07  xx
        {  9, "N ",                     nullptr     },  // 0A  07  xx
        { 10, "N ",                     nullptr     },  // 0B  07  xx
        { 11, "N ",                     nullptr     },  // 0C  07  xx
        { 12, "N ",                     nullptr     },  // 0D  07  xx
        { 13, "N ",                     nullptr     },  // 0E  07  xx
        { 14, "N ",                     nullptr     },  // 0F  07  xx
        { 15, "N ",                     nullptr     },  // 10  07  xx
    };

//########################################################
MIDI_ENCODER_MAP KnobMapArray[] =
    {   {  0, "Soft LFO freq",          FreqLFO, 0  },  //  01  0A  xx
        {  1, "Hard LFO freq",          FreqLFO, 0  },  //  02  0A  xx
        {  2, "Hard LFO pulse width",   FreqLFO, 0  },  //  03  0A  xx
        {  3, "N ",                     nullptr, 0  },  //  04  0A  xx
        {  4, "N ",                     nullptr, 0  },  //  05  0A  xx
        {  5, "N ",                     nullptr, 0  },  //  06  0A  xx
        {  6, "N ",                     nullptr, 0  },  //  07  0A  xx
        {  7, "N ",                     nullptr, 0  },  //  08  0A  xx
        {  8, "N ",                     nullptr, 0  },  //  09  0A  xx
        {  9, "N ",                     nullptr, 0  },  //  0A  0A  xx
        { 10, "N ",                     nullptr, 0  },  //  0B  0A  xx
        { 11, "N ",                     nullptr, 0  },  //  0C  0A  xx
        { 12, "N ",                     nullptr, 0  },  //  0D  0A  xx
        { 14, "N ",                     nullptr, 0  },  //  0E  0A  xx
        { 14, "N ",                     nullptr, 0  },  //  0F  0A  xx
        { 15, "N ",                     nullptr, 0  },  //  10  0A  xx
    };

//########################################################
MIDI_BUTTON_MAP SwitchMapArray[] =
    {   {  0, "VCA Mod Sine    ",       ToggleModVCA    },  //  01  10  xx
        {  1, "VCA Mod Triangle",       ToggleModVCA    },  //  01  11  xx
        {  2, "VCA Mod Sawtooth",       ToggleModVCA    },  //  01  12  xx
        {  3, "VCA Mod Pulse   ",       ToggleModVCA    },  //  01  13  xx
        {  4, "VCA Mod Noise   ",       ToggleModVCA    },  //  01  14  xx
        {  5, "Switch f6",              nullptr         },  //  01  15  xx
        {  6, "Switch f7",              nullptr         },  //  01  16  xx
        {  7, "Switch f8",              nullptr         },  //  01  17  xx
        {  0, "VCO freq Mod Sine",      ToggleModVCO    },  //  01  18  xx
        {  1, "VCO freq Mod Ramp",      ToggleModVCO    },  //  01  19  xx
        {  2, "VCO freq Pulse   ",      ToggleModVCO    },  //  01  1A  xx
        {  3, "Toggle Ramp Direction",  ToggleRampDir   },  //  01  1B  xx
        {  4, "Switch #13",             nullptr         },  //  01  1C  xx
        { 13, "Switch #14",             nullptr         },  //  01  1D  xx
        { 14, "Switch #15",             nullptr         },  //  01  1E  xx
        { 15, "Switch #16",             nullptr         },  //  01  1F  xx
        { 24, "Switch #25",             nullptr         },  //  01  72  xx
        { 24, "Switch #25",             nullptr         },  //  01  73  xx
        { 20, "Switch #21",             nullptr         },  //  01  74  xx
        { 21, "Switch #22",             nullptr         },  //  01  75  xx
        { 22, "Tune/Reset",             TuneReset       },  //  01  76  xx
        { 23, "Switch #24",             nullptr         },  //  01  77  xx
     };















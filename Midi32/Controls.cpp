//#######################################################################
// Module:     Controls.cpp
// Descrption: Synthesizer midi control routing
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include "../Common/SynthCommon.h"
#include "FrontEnd.h"
#include "I2Cmessages.h"
#include "SerialMonitor.h"
#include "SoftLFO.h"

//########################################################
//   VCO & VCA controls
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
    SynthFront.VoiceComponentSetSelected (ch, state);
    }

//########################################################
static void ToggleRampDir (short ch, bool state)
    {
    SynthFront.SetRampDir (state);
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
//  LFO controls
//########################################################
static void FreqLFO (short ch, short data)
    {
    SynthFront.FreqLFO (ch, data);
    }

//########################################################
static void ToggleModVCA (short ch, bool state)
    {
    SynthFront.SelectModVCA (ch, state);
    }

//########################################################
static void ToggleModVCO (short ch, bool state)
    {
    SynthFront.SelectModVCO (ch, state);
    }

//########################################################
//  Noise control
//########################################################
static void ToggleNoise (short ch, bool state)
    {
    SynthFront.SetNoise (ch, state);
    }

//########################################################
//  Tuning control
//########################################################
static void TuneReset (short ch, bool state)
    {
    if ( state )
        Monitor.Tuning ();
    else
        Monitor.Reset ();
    }

//########################################################
static void TuneUp (short ch, bool state)
    {
    SynthFront.TuningAdjust (true);
    }

//########################################################
static void TuneDown (short ch, bool state)
    {
    SynthFront.TuningAdjust (false);
    }

//########################################################
static void TuneBump (short ch, bool state)
    {
    SynthFront.TuningBump (state);
    }

//########################################################
static void TunningSave (short ch, bool state)
    {
    SynthFront.SaveTuning ();
    }

//########################################################
// Debug to advance page selection
//########################################################
static void PageAdvance (short ch, short data)
    {
    if ( data )
        DisplayMessage.PageAdvance();
    else
        SynthFront.SetClearKeyRed (0x64);
    }

//########################################################
//  Channel to voice mapping controls
//########################################################
static void TrackSel (short ch, short data)
    {
    static byte count = 0;
    static byte last = 0;

    count += ( data ) ? 1 : -1;
    switch ( count )
        {
        case 0:
            if ( SynthFront.GetMidiMapMode () && (last == 0) )
                SynthFront.ChangeMapSelect (ch);
            else
                last--;
            break;
        case 2:
            SynthFront.MidiMapMode ();
            last = 1;
            break;
        }
    }

//########################################################
static void SendDir (short index, short data)
    {
    if ( SynthFront.GetMidiMapMode () )
        {
        if ( data )
            SynthFront.MapModeBump (( index ) ? -1 : 1);
        }
    }

static void SaveConfig (short index, short data)
    {
    if ( SynthFront.GetMidiMapMode () )
        {
        if ( data )
            SynthFront.SaveDefaultConfig ();
        }
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
        {    0,                0, "Set Sine",           SetTimeSetSelect  },    // 01  50  xx
        {    1,                0, "Set Triangle",       SetTimeSetSelect  },    // 01  51  xx
        {    2,                0, "Set Ramp",           SetTimeSetSelect  },    // 01  52  xx
        {    3,                0, "Set Pulse",          SetTimeSetSelect  },    // 01  53  xx
        {    4,                0, "Set Square",         SetTimeSetSelect  },    // 01  54  xx
        { 0x55,             0x0D, "Sawtooth Dir",       SawtoothDirection },    // 01  55  xx
        {    1,                0, "N ",                 nullptr           },    // 01  56  xx
        {    2,                0, "N ",                 nullptr           },    // 01  57  xx
        {   40,                0, "N ",                 nullptr           },    // 01  58  xx
        {   41,                0, "N ",                 nullptr           },    // 01  59  xx
        {   42,                0, "N ",                 nullptr           },    // 01  5A  xx
        {   43,                0, "N ",                 nullptr           },    // 01  5B  xx
        {   44,                0, "N ",                 nullptr           },    // 01  5C  xx
        {   45,                0, "N ",                 nullptr           },    // 01  5D  xx
        {   46,                0, "N ",                 nullptr           },    // 01  5E  xx
        {   47,                0, "N ",                 nullptr           },    // 01  5F  xx
        {    0,                0, "Send Sel UP",        SendDir           },    // 01  60  xx
        {    1,                0, "Send Sel DN",        SendDir           },    // 01  61  xx
        {    0,                0, "Track Sel Left",     TrackSel          },    // 01  62  xx
        {    1,                0, "Track Sel Right",    TrackSel          },    // 01  63  xx
        { 0x64,             0x3F, "Page Advance",       PageAdvance       },    // 01  64  xx
        {   53,                0, "N ",                 nullptr           },    // 01  65  xx
        {   54,                0, "N ",                 nullptr           },    // 01  66  xx
        {   55,                0, "Save Configuration", SaveConfig        },    // 01  67  xx
    };

LED_NOTE_MAP SendA[]     = { 13, 29, 45, 61, 77, 93, 109, 125 };
LED_NOTE_MAP SendB[]     = { 14, 30, 46, 62, 78, 94, 110, 126 };
LED_NOTE_MAP PanDevice[] = { 15, 31, 47, 63, 79, 95, 111, 127 };

//########################################################
//########################################################
MIDI_MAP FaderMapArray[] =
    {   {  0, "Sine Sustain",           SetSustain  },  // 01  07  xx
        {  1, "Triangle Sustain",       SetSustain  },  // 02  07  xx
        {  2, "Ramp Sustain",           SetSustain  },  // 03  07  xx
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
    {   {  0, "Soft LFO freq",          FreqLFO, 1  },  //  01  0A  xx
        {  1, "Hard LFO freq",          FreqLFO, 1  },  //  02  0A  xx
        {  2, "Hard LFO pulse width",   FreqLFO, 1  },  //  03  0A  xx
        {  3, "N ",                     nullptr, 1  },  //  04  0A  xx
        {  4, "N ",                     nullptr, 1  },  //  05  0A  xx
        {  5, "N ",                     nullptr, 1  },  //  06  0A  xx
        {  6, "N ",                     nullptr, 1  },  //  07  0A  xx
        {  7, "N ",                     nullptr, 1  },  //  08  0A  xx
        {  8, "N ",                     nullptr, 1  },  //  09  0A  xx
        {  9, "N ",                     nullptr, 1  },  //  0A  0A  xx
        { 10, "N ",                     nullptr, 1  },  //  0B  0A  xx
        { 11, "N ",                     nullptr, 1  },  //  0C  0A  xx
        { 12, "N ",                     nullptr, 1  },  //  0D  0A  xx
        { 14, "N ",                     nullptr, 1  },  //  0E  0A  xx
        { 14, "N ",                     nullptr, 1  },  //  0F  0A  xx
        { 15, "N ",                     nullptr, 1  },  //  10  0A  xx
    };

//########################################################
MIDI_BUTTON_MAP SwitchMapArray[] =
    {   {  0,           false,  "VCA Mod Sine    ",         ToggleModVCA  },  //  01  10  xx
        {  1,           false,  "VCA Mod Triangle",         ToggleModVCA  },  //  01  11  xx
        {  2,           false,  "VCA Mod Ramp",             ToggleModVCA  },  //  01  12  xx
        {  3,           false,  "VCA Mod Pulse   ",         ToggleModVCA  },  //  01  13  xx
        {  4,           false,  "VCA Mod Noise   ",         ToggleModVCA  },  //  01  14  xx
        {  5,           false,  "Switch f6",                nullptr       },  //  01  15  xx
        {  6,           false,  "Switch f7",                nullptr       },  //  01  16  xx
        {  7,           false,  "Switch f8",                nullptr       },  //  01  17  xx
        {  0,           false,  "VCO freq Mod Sine",        ToggleModVCO  },  //  01  18  xx
        {  1,           false,  "VCO freq Mod Ramp",        ToggleModVCO  },  //  01  19  xx
        {  2,           false,  "VCO freq Pulse   ",        ToggleModVCO  },  //  01  1A  xx
        {  3,           false,  "Toggle Ramp Direction",    ToggleRampDir },  //  01  1B  xx
        { DUCT_BLUE,    false,  "0-1 Blue ",                ToggleNoise   },  //  01  1C  xx
        { DUCT_WHITE,   false,  "0-1 White",                ToggleNoise   },  //  01  1D  xx
        { DUCT_PINK,    false,  "0-1 Pink ",                ToggleNoise   },  //  01  1E  xx
        { DUCT_RED,     false,  "0-1 Red  ",                ToggleNoise   },  //  01  1F  xx
        { 24,           false,  "Tuning save",              TunningSave   },  //  01  72  xx
        { 24,           false,  "",                         nullptr       },  //  01  73  xx
        { 20,           false,  "Tune +",                   TuneUp        },  //  01  74  xx
        { 21,           false,  "Tune -",                   TuneDown      },  //  01  75  xx
        { 22,           false,  "Tune/Reset",               TuneReset     },  //  01  76  xx
        { 23,           false,  "Tune -/+",                 TuneBump      },  //  01  77  xx
     };















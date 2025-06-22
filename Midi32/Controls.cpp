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
    if ( data )
        SynthFront.ToggleRampDirection ();
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
static void ToggleModRampDir (short ch, bool state)
    {
    if ( ch < 5 )
        SynthFront.SetModRampDir (0, state);
    else
        SynthFront.SetModRampDir (1, state);
    }

//########################################################
static void ToggleModVCO (short ch, bool state)
    {
    if ( ch < 3 )
        SynthFront.SelectModVCO (0, ch, state);
    else
        SynthFront.SelectModVCO (1, ch - 4, state);
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
        SynthFront.PageAdvance ();
    }

//########################################################
//  Channel to voice mapping controls
//########################################################
static void TrackSel (short ch, short data)
    {
    static byte count = 0;
    static byte last = 0;

    if ( !(!data && !count) )
        count += ( data ) ? 1 : -1;

    switch ( count )
        {
        case 0:
            if ( last == 0 )
                SynthFront.ChangeMapSelect (ch - 2);
            else
                last--;
            break;
        case 2:
            SynthFront.MidiMapMode ();
            last = 2;
            count = 0;
            break;
        }
    }

//########################################################
static void SendDir (short index, short data)
    {
    if ( data )
        {
        if ( SynthFront.GetMidiMapMode () )
            SynthFront.MapModeBump (( index ) ? -1 : 1);
        else if ( SynthFront.GetLoadSaveMode () )
            SynthFront.LoadSaveBump (( index ) ? -1 : 1);
        }
    }

//########################################################
static void LoadConfig (short index, short data)
    {
    if ( data )
        {
        if ( SynthFront.GetLoadSaveMode () )
            SynthFront.LoadSelectedConfig ();
        else
            SynthFront.OpenLoadSavePage ();
        }
    }

//########################################################
static void SaveConfig (short index, short data)
    {
    if ( data )
        {
        if ( SynthFront.GetMidiMapMode() )
            SynthFront.SaveDefaultConfig ();
        else if ( SynthFront.GetLoadSaveMode () )
            SynthFront.SaveSelectedConfig ();
        else
            SynthFront.OpenLoadSavePage ();

        }
    }

//########################################################
static void SetFilterQ (short index, short data)
    {
    I2cDevices.D2Analog (212, data * MIDI_MULTIPLIER);
    I2cDevices.UpdateAnalog  ();     // Update D/A ports
    }

//########################################################
static void SetFilterF (short index, short data)
    {
    I2cDevices.D2Analog (213, data * MIDI_MULTIPLIER);
    I2cDevices.UpdateAnalog  ();     // Update D/A ports
    }

//########################################################
//########################################################
MIDI_XL_MAP    XlMapArray[SIZE_CL_MAP] =
    {   {    0,                0, "N ",                 nullptr           },    // 30
        {    1,                0, "N ",                 nullptr           },    // 31
        {    2,                0, "N ",                 nullptr           },    // 32
        {    3,                0, "N ",                 nullptr           },    // 33
        {    4,                0, "N ",                 nullptr           },    // 34
        {    5,                0, "N ",                 nullptr           },    // 35
        {    6,                0, "N ",                 nullptr           },    // 36
        {    7,                0, "N ",                 nullptr           },    // 37
        {    8,                0, "N ",                 nullptr           },    // 38
        {    9,                0, "N ",                 nullptr           },    // 39
        {   10,                0, "N ",                 nullptr           },    // 3A
        {   11,                0, "N ",                 nullptr           },    // 3B
        {   12,                0, "N ",                 nullptr           },    // 3C
        {   13,                0, "N ",                 nullptr           },    // 3D
        {   14,                0, "N ",                 nullptr           },    // 3E
        {   15,                0, "N ",                 nullptr           },    // 3F
        {    0,                0, "Attack",             SetAttckTime      },    // 40
        {    1,                0, "Decay",              SetDecayTime      },    // 41
        {    2,                0, "Release",            SetReleaseTime    },    // 42
        {    3,                0, "N ",                 nullptr           },    // 43
        {   20,                0, "N ",                 nullptr           },    // 44
        {   21,                0, "N ",                 nullptr           },    // 45
        {   22,                0, "N ",                 nullptr           },    // 46
        {  127,             0x1D, "Pulse Width",        PulseWidth        },    // 47
        {    0,                0, "Sine max level",     SetMaxLevel       },    // 48
        {    1,                0, "Triangle max level", SetMaxLevel       },    // 49
        {    2,                0, "Sqiare max level",   SetMaxLevel       },    // 4A
        {    3,                0, "Sawtooth max level", SetMaxLevel       },    // 4B
        {    4,                0, "Pulse max level",    SetMaxLevel       },    // 4C
        {   29,                0, "N ",                 nullptr           },    // 4D
        {   30,                0, "Test Q ",            SetFilterQ        },    // 4E
        {   31,                0, "Fest F",             SetFilterF        },    // 4F
        { 0x50,             0x1F, "Set Sine",           SetTimeSetSelect  },    // 50
        { 0x51,             0x1F, "Set Triangle",       SetTimeSetSelect  },    // 51
        { 0x52,             0x1F, "Set Ramp",           SetTimeSetSelect  },    // 52
        { 0x53,             0x1F, "Set Pulse",          SetTimeSetSelect  },    // 53
        { 0x54,             0x1F, "Set Noise",          SetTimeSetSelect  },    // 54
        { 0x55,                0, "N ",                 nullptr           },    // 55
        { 0x56,                0, "N ",                 nullptr           },    // 56
        { 0x57,             0x1F, "Sawtooth Dir",       SawtoothDirection },    // 57
        { 0x58,                0, "N ",                 nullptr           },    // 58
        { 0x59,                0, "N ",                 nullptr           },    // 59
        { 0x5A,                0, "N ",                 nullptr           },    // 5A
        { 0x5B,                0, "N ",                 nullptr           },    // 5B
        { 0x5C,                0, "N ",                 nullptr           },    // 5C
        { 0x5D,                0, "N ",                 nullptr           },    // 5D
        { 0x5E,                0, "N ",                 nullptr           },    // 5E
        { 0x5F,                0, "N ",                 nullptr           },    // 5F
        { 0x60,             0x1F, "Send Sel UP",        SendDir           },    // 60
        { 0x61,             0x1F, "Send Sel DN",        SendDir           },    // 61
        { 0x62,             0x1F, "Track Sel Left",     TrackSel          },    // 62
        { 0x63,             0x1F, "Track Sel Right",    TrackSel          },    // 63
        { 0x64,             0x1F, "Page Advance",       PageAdvance       },    // 64
        { 0x65,             0x1F, "Save Configuration", SaveConfig        },    // 65
        { 0x66,             0x1F, "Load Configuration", LoadConfig        },    // 66
        { 0x67,                0, "N ",                 nullptr           },    // 67
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
        {  1, "Hard LFO 1 freq",        FreqLFO, 1  },  //  02  0A  xx
        {  2, "Hard LFO 1 pulse width", FreqLFO, 1  },  //  03  0A  xx
        {  3, "Hard LFO 2 freq",        FreqLFO, 1  },  //  04  0A  xx
        {  4, "Hard LFO 2 pulse width", FreqLFO, 1  },  //  05  0A  xx
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
    {   {  0,           false,  "VCA Mod Sine    ",         ToggleModVCA     },  //  01  10  xx
        {  1,           false,  "VCA Mod Triangle",         ToggleModVCA     },  //  01  11  xx
        {  2,           false,  "VCA Mod Ramp",             ToggleModVCA     },  //  01  12  xx
        {  3,           false,  "VCA Mod Pulse   ",         ToggleModVCA     },  //  01  13  xx
        {  4,           false,  "VCA Mod Noise   ",         ToggleModVCA     },  //  01  14  xx
        {  5,           false,  "Switch f6",                nullptr          },  //  01  15  xx
        {  6,           false,  "Switch f7",                nullptr          },  //  01  16  xx
        {  7,           false,  "Switch f8",                nullptr          },  //  01  17  xx
        {  0,           false,  "VCO freq Mod Sine",        ToggleModVCO     },  //  01  18  xx
        {  1,           false,  "VCO freq Mod Ramp",        ToggleModVCO     },  //  01  19  xx
        {  2,           false,  "VCO freq Pulse   ",        ToggleModVCO     },  //  01  1A  xx
        {  3,           false,  "Toggle Ramp Direction",    ToggleModRampDir },  //  01  1B  xx
        {  4,           false,  "VCO freq Mod Sine",        ToggleModVCO     },  //  01  1C  xx
        {  5,           false,  "VCO freq Mod Ramp",        ToggleModVCO     },  //  01  1D  xx
        {  6,           false,  "VCO freq Pulse   ",        ToggleModVCO     },  //  01  1E  xx
        {  7,           false,  "Toggle Ramp Direction",    ToggleModRampDir },  //  01  1F  xx
        { 24,           false,  "Tuning save",              TunningSave      },  //  01  72  xx
        { 24,           false,  "",                         nullptr          },  //  01  73  xx
        { 20,           false,  "Tune +",                   TuneUp           },  //  01  74  xx
        { 21,           false,  "Tune -",                   TuneDown         },  //  01  75  xx
        { 22,           false,  "Tune/Reset",               TuneReset        },  //  01  76  xx
        { 23,           false,  "Tune -/+",                 TuneBump         },  //  01  77  xx
     };















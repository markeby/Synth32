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
//   VCO / VCA / VCF controls
//########################################################
static void SetLevel (short ch, short data)
    {
    SynthFront.SetLevel (ch, data);
    }

//########################################################
static void SetLevelSelect (short ch, short state)
    {
    if ( !state )
        SynthFront.VoiceLevelSelect(ch, state);
    }

//########################################################
static void SetAttackTime (short ch, short data)
    {
    SynthFront.SetAttackTime (ch, data);
    }

//########################################################
static void SetDecayTime (short ch, short data)
    {
    SynthFront.SetDecayTime (ch, data);
    }

//########################################################
static void SetReleaseTime (short ch, short data)
    {
    SynthFront.SetReleaseTime (ch, data);
    }

//########################################################
static void ToogleRamp (short ch, short data)
    {
    if ( !data )
        SynthFront.ToggleRampDirection (ch);
    }

//########################################################
static void PulseWidth (short ch, short data)
    {
    if ( data == 0 )
        data = 1;
    SynthFront.SetPulseWidth (data);
    }

//########################################################
//########################################################
static void FltStart (short ch, short data)
    {
    SynthFront.FltStart (ch, data);
    }

//########################################################
static void FltEnd  (short ch, short data)
    {
    SynthFront.FltEnd (ch, data);
    }

//########################################################
static void FltSustain (short ch, short data)
    {
    SynthFront.SetSustainLevel (ch, data);
    }

//########################################################
//  LFO controls
//########################################################
static void FreqLFO (short ch, short data)
    {
    SynthFront.FreqLFO (ch, data);
    }

//########################################################
static void ToggleModVCA (short ch, short data)
    {
    if ( !data )
        SynthFront.SelectModVCA (ch);
    }

//########################################################
static void ToggleModLevelAlt (short ch, short data)
    {
    if ( !data )
        SynthFront.ToggleModLevelAlt (ch);
    }

//########################################################
static void ToggleModRampDir (short ch, short data)
    {
    if ( !data )
        SynthFront.ToggleModRampDir (ch);
    }

//########################################################
static void ToggleModVCO (short ch, short data)
    {
    if ( !data )
        {
        if ( ch < 3 )
            SynthFront.SelectModVCO (0, ch);
        else
            SynthFront.SelectModVCO (1, ch - 4);
        }
    }

//########################################################
//  Tuning control
//########################################################
static void TuneReset (short ch, bool state)
    {
    if ( state )
        SynthFront.StartCalibration ();
    else
        Monitor.Reset ();
    }

//########################################################
static void SetTuningLevel (short ch, short data)
    {
    SynthFront.SetTuningLevel (ch, data);
    }

//########################################################
static void TuneUpDown (short ch, bool state)
    {
    SynthFront.TuningAdjust (ch);
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
    if ( !data )
        SynthFront.PageAdvance ();
    }

//########################################################
//  Channel to voice mapping controls
//########################################################
static void TrackSel (short ch, short data)
    {
    if ( !data )
        {
        if ( SynthFront.GetMidiMapMode () )
            SynthFront.ChangeMapSelect (ch);
        }
    }

//########################################################
static void MappingSelect (short index, short data)
    {
    if ( !data )
        SynthFront.MidiMapMode ();
    }

//########################################################
static void SendDir (short index, short data)
    {
    if ( !data )
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
    if ( !data )
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
    if ( !data )
        {
        if ( SynthFront.GetMidiMapMode () )
            SynthFront.SaveDefaultConfig ();
        else if ( SynthFront.GetLoadSaveMode () )
            SynthFront.SaveSelectedConfig ();
        else
            SynthFront.OpenLoadSavePage ();
        }
    }

//########################################################
static void SelectFilter (short index, short data)
    {
    if ( !data )
        SynthFront.SelectFilter (index);
    }

//########################################################
static void DummyButton (short index, short data)
    {
    SynthFront.TemplateRefresh ();
    }

//########################################################
//########################################################
XL_MIDI_MAP    XL_MidiMapArray[XL_MIDI_MAP_PAGES][XL_MIDI_MAP_SIZE] =
    {
// XL_MIDI_MAP_OSC
      { {     0,   0x3C, "Attack Sine",             SetAttackTime       },  // 48 0x30  0
        {     1,   0x3C, "Attack Triangle",         SetAttackTime       },  // 49 0x31  1
        {     2,   0x3C, "Attack Ramp",             SetAttackTime       },  // 50 0x32  2
        {     3,   0x3C, "Attack Pulse",            SetAttackTime       },  // 51 0x33  3
        {     4,   0x3C, "Attack Noise",            SetAttackTime       },  // 52 0x34  4
        {     5,   0x0C, "N ",                      nullptr             },  // 53 0x35  5
        {     6,   0x0C, "N ",                      nullptr             },  // 54 0x36  6
        {     7,   0x0C, "N ",                      nullptr             },  // 55 0x37  7
        {     0,   0x3C, "Decay Sine",              SetDecayTime        },  // 56 0x38  8
        {     1,   0x3C, "Decay Triangle",          SetDecayTime        },  // 57 0x39  9
        {     2,   0x3C, "Decay Ramp",              SetDecayTime        },  // 58 0x3A  10
        {     3,   0x3C, "Decay Pulse",             SetDecayTime        },  // 59 0x3B  11
        {     4,   0x3C, "Decay Noise",             SetDecayTime        },  // 60 0x3C  12
        {     5,   0x0C, "N ",                      nullptr             },  // 61 0x3D  13
        {     6,   0x0C, "N ",                      nullptr             },  // 62 0x3E  14
        {     7,   0x0C, "N ",                      nullptr             },  // 63 0x3F  15
        {     0,   0x3C, "Release Sine",            SetReleaseTime      },  // 64 0x40  16
        {     1,   0x3C, "Release Triangle",        SetReleaseTime      },  // 65 0x41  17
        {     2,   0x3C, "Release Ramp",            SetReleaseTime      },  // 66 0x42  18
        {     3,   0x3C, "Release Pulse",           SetReleaseTime      },  // 67 0x43  19
        {     4,   0x3C, "Release Noise",           SetReleaseTime      },  // 68 0x44  20
        {     5,   0x0C, "N ",                      nullptr             },  // 69 0x45  21
        {     6,   0x0C, "N ",                      nullptr             },  // 70 0x46  22
        {   127,   0x3C, "Pulse Width",             PulseWidth          },  // 71 0x47  23
        {     0,   0x3C, "Set Sine L/S",            SetLevelSelect      },  // 72 0x48  24  First button
        {     1,   0x3C, "Set Triangle L/S",        SetLevelSelect      },  // 73 0x49  25
        {     2,   0x3C, "Set Ramp  L/S",           SetLevelSelect      },  // 74 0x4A  26
        {     3,   0x3C, "Set Pulse L/S ",          SetLevelSelect      },  // 75 0x4B  27
        {     4,   0x3C, "Set Noise L/S ",          SetLevelSelect      },  // 76 0x4C  28
        {     5,   0x0C, "N ",                      nullptr             },  // 77 0x4D  29
        {     6,   0x0C, "N ",                      nullptr             },  // 78 0x4E  30
        {     7,   0x3C, "Sawtooth Dir",            ToogleRamp          },  // 79 0x4D  31
        {     8,   0x0C, "N ",                      nullptr             },  // 80 0x50  32
        {     9,   0x0C, "N ",                      nullptr             },  // 81 0x51  33
        {    10,   0x0C, "N ",                      nullptr             },  // 82 0x52  34
        {    11,   0x0C, "N ",                      nullptr             },  // 83 0x53  35
        {    12,   0x0C, "N ",                      nullptr             },  // 84 0x54  36
        {    13,   0x0C, "N ",                      nullptr             },  // 85 0x55  37
        {    14,   0x0C, "N ",                      nullptr             },  // 86 0x56  38
        {    15,   0x0C, "N ",                      nullptr             },  // 87 0x57  39  last button
        {     0,   0x3F, "Map mode select",         MappingSelect       },  // 88 0x58  40
        {     1,   0x3F, "Save Configuration",      SaveConfig          },  // 89 0x59  41
        {     2,   0x3F, "Load Configuration",      LoadConfig          },  // 90 0x5A  42
        {     3,   0x3F, "Page Advance",            PageAdvance         },  // 91 0x5B  43
        {     0,   0x0C, "N ",                      nullptr             },  // 92 0x5C  44
        {     1,   0x0C, "N ",                      nullptr             },  // 93 0x5D  45
        {     0,   0x0C, "N ",                      nullptr             },  // 94 0x5E  46
        {     1,   0x0C, "N ",                      nullptr             },  // 95 0x5F  47
        {     0,      0, "Level Sine",              SetLevel            },  // 96 0x60  48
        {     1,      0, "Level Triangle",          SetLevel            },  // 97 0x61  49
        {     2,      0, "Level Ramp",              SetLevel            },  // 98 0x62  50
        {     3,      0, "Level Pulse",             SetLevel            },  // 99 0x63  51
        {     4,      0, "Level Noise",             SetLevel            },  //100 0x64  52
        {     5,      0, "N ",                      nullptr             },  //101 0x65  53
        {     6,      0, "N ",                      nullptr             },  //102 0x66  54
        {     7,      0, "N ",                      nullptr             },  //103 0x67  55
      },
// XL_MIDI_MAP_FILTER
      { {     8,   0x3C, "Attack Freq",             SetAttackTime       },  // 48 0x30  0
        {     0,   0x0C, "N ",                      nullptr             },  // 49 0x31  1
        {     0,   0x0C, "N ",                      nullptr             },  // 50 0x32  2
        {     9,   0x0F, "Attack Q",                SetAttackTime       },  // 51 0x33  3
        {     0,   0x0C, "N ",                      nullptr             },  // 52 0x34  4
        {     0,   0x0C, "N ",                      nullptr             },  // 53 0x35  5
        {     0,   0x0C, "N ",                      nullptr             },  // 54 0x36  6
        {     0,   0x0C, "N ",                      nullptr             },  // 55 0x37  7
        {     8,   0x3C, "Decay Freq",              SetDecayTime        },  // 56 0x38  8
        {     0,   0x0C, "N ",                      nullptr             },  // 57 0x39  9
        {     0,   0x0C, "N ",                      nullptr             },  // 58 0x3A  10
        {     9,   0x0F, "Decay Q",                 SetDecayTime        },  // 59 0x3B  11
        {     0,   0x0C, "N ",                      nullptr             },  // 60 0x3C  12
        {     0,   0x0C, "N ",                      nullptr             },  // 61 0x3D  13
        {     0,   0x0C, "N ",                      nullptr             },  // 62 0x3E  14
        {     0,   0x0C, "N ",                      nullptr             },  // 63 0x3F  15
        {     8,   0x3C, "Release Freq",            SetReleaseTime      },  // 64 0x40  16
        {     0,   0x0C, "N ",                      nullptr             },  // 65 0x41  17
        {     0,   0x0C, "N ",                      nullptr             },  // 66 0x42  18
        {     9,   0x0F, "Release Q",               SetReleaseTime      },  // 67 0x43  19
        {     0,   0x0C, "N ",                      nullptr             },  // 68 0x44  20
        {     0,   0x0C, "N ",                      nullptr             },  // 69 0x45  21
        {     0,   0x0C, "N ",                      nullptr             },  // 70 0x46  22
        {     0,   0x0C, "N ",                      nullptr             },  // 71 0x47  23
        {     0,   0x1C, "N ",                      DummyButton         },  // 72 0x48  24  First button
        {     0,   0x1C, "N ",                      DummyButton         },  // 73 0x49  25
        {     0,   0x1C, "N ",                      DummyButton         },  // 74 0x4A  26
        {     0,   0x0D, "N ",                      DummyButton         },  // 75 0x4B  27
        {     0,   0x0D, "N ",                      DummyButton         },  // 76 0x4C  28
        {     0,   0x0D, "N ",                      DummyButton         },  // 77 0x4D  29
        {     0,   0x0C, "N ",                      nullptr             },  // 78 0x4E  30
        {     0,   0x0C, "N ",                      nullptr             },  // 79 0x4D  31
        {     0,   0x3C, "Select LP",               SelectFilter        },  // 80 0x50  32
        {     1,   0x3C, "Select LBP",              SelectFilter        },  // 81 0x51  33
        {     2,   0x3C, "Select UBP",              SelectFilter        },  // 82 0x52  34
        {     3,   0x3C, "Select HP",               SelectFilter        },  // 83 0x53  35
        {     0,   0x0C, "N ",                      nullptr             },  // 84 0x54  36
        {     0,   0x0C, "N ",                      nullptr             },  // 85 0x55  37
        {     0,   0x0C, "N ",                      nullptr             },  // 86 0x56  38
        {     0,   0x0C, "N ",                      nullptr             },  // 87 0x57  39  last button
        {     0,   0x3F, "Map mode select",         MappingSelect       },  // 88 0x58  40
        {     1,   0x3F, "Save Configuration",      SaveConfig          },  // 89 0x59  41
        {     2,   0x3F, "Load Configuration",      LoadConfig          },  // 90 0x5A  42
        {     3,   0x3F, "Page Advance",            PageAdvance         },  // 91 0x5B  43
        {     0,   0x0C, "N ",                      nullptr             },  // 92 0x5C  44
        {     1,   0x0C, "N ",                      nullptr             },  // 93 0x5D  45
        {     0,   0x0C, "N ",                      nullptr             },  // 94 0x5E  46
        {     1,   0x0C, "N ",                      nullptr             },  // 95 0x5F  47
        {     8,      0, "FLT FREQ START",          FltStart            },  // 96 0x60  48
        {     8,      0, "FLT FREQ END",            FltEnd              },  // 97 0x61  49
        {     8,      0, "FLT FREQ SUSTAIN",        FltSustain          },  // 98 0x62  50
        {     9,      0, "FLT Q START",             FltStart            },  // 99 0x63  51
        {     9,      0, "FLT Q END",               FltEnd              },  //100 0x64  52
        {     9,      0, "FLT Q SUSTAIN",           FltSustain          },  //101 0x65  53
        {     0,      0, "N ",                      nullptr             },  //102 0x66  54
        {     0,      0, "N ",                      nullptr             },  //103 0x67  55
      },
// XL_MIDI_MAP_LFO
      { {     0,   0x2C, "Hard LFO 1 course freq",  FreqLFO             },  // 48 0x30  0
        {     1,   0x3C, "Hard LFO 1 fine freq",    FreqLFO             },  // 49 0x31  1
        {     2,   0x1C, "Hard LFO 1 pulse width",  FreqLFO             },  // 50 0x32  2
        {     0,      0, "N ",                      nullptr             },  // 51 0x33  3
        {     3,   0x0E, "Hard LFO 2 course freq",  FreqLFO             },  // 52 0x34  4
        {     4,   0x0F, "Hard LFO 2 fine freq",    FreqLFO             },  // 53 0x35  5
        {     5,   0x0D, "Hard LFO 2 pulse width",  FreqLFO             },  // 54 0x36  6
        {     0,      0, "N ",                      nullptr             },  // 55 0x37  7
        {     0,      0, "N ",                      nullptr             },  // 56 0x38  8
        {     0,      0, "N ",                      nullptr             },  // 57 0x39  9
        {     0,      0, "N ",                      nullptr             },  // 58 0x3A  10
        {     0,      0, "N ",                      nullptr             },  // 59 0x3B  11
        {     0,      0, "N ",                      nullptr             },  // 60 0x3C  12
        {     0,      0, "N ",                      nullptr             },  // 61 0x3D  13
        {     0,      0, "N ",                      nullptr             },  // 62 0x3E  14
        {     0,      0, "N ",                      nullptr             },  // 63 0x3F  15
        {     6,   0x2C, "Soft LFO course freq",    FreqLFO             },  // 64 0x40  16
        {     7,   0x3C, "Soft LFO fine freq",      FreqLFO             },  // 65 0x41  17
        {     0,      0, "N ",                      nullptr             },  // 66 0x42  18
        {     0,      0, "N ",                      nullptr             },  // 67 0x43  19
        {     0,      0, "N ",                      nullptr             },  // 68 0x44  20
        {     0,      0, "N ",                      nullptr             },  // 69 0x45  21
        {     0,      0, "N ",                      nullptr             },  // 70 0x46  22
        {     0,      0, "N ",                      nullptr             },  // 71 0x47  23
        {     0,   0x3C, "LFO 1 freq Mod Sine",     ToggleModVCO        },  // 72 0x48  24  First button
        {     1,   0x3C, "LFO 1 freq Mod Ramp",     ToggleModVCO        },  // 73 0x49  25
        {     2,   0x3C, "LFO 1 freq Pulse",        ToggleModVCO        },  // 74 0x4A  26
        {     0,   0x3C, "Toggle Ramp 1 Direction", ToggleModRampDir    },  // 75 0x4B  27
        {     4,   0x3C, "LFO 2 freq Mod Sine",     ToggleModVCO        },  // 76 0x4C  28
        {     5,   0x3C, "LFO 2 freq Mod Ramp",     ToggleModVCO        },  // 77 0x4D  29
        {     6,   0x3C, "LFO 2 freq Pulse",        ToggleModVCO        },  // 78 0x4E  30
        {     1,   0x3C, "Toggle Ramp2  Direction", ToggleModRampDir    },  // 79 0x4D  31
        {     0,   0x3C, "VCA Mod Sine",            ToggleModVCA        },  // 80 0x50  32
        {     1,   0x3C, "VCA Mod Triangle",        ToggleModVCA        },  // 81 0x51  33
        {     2,   0x3C, "VCA Mod Ramp",            ToggleModVCA        },  // 82 0x52  34
        {     3,   0x3C, "VCA Mod Pulse",           ToggleModVCA        },  // 83 0x53  35
        {     4,   0x3C, "VCA Mod Noise",           ToggleModVCA        },  // 84 0x54  36
        {     0,      0, "N ",                      nullptr             },  // 85 0x55  37
        {     0,   0x3C, "Toggle LFO 0 Mod Level",  ToggleModLevelAlt   },  // 86 0x56  38
        {     1,   0x3C, "Toggle LFO 1 Mod Level",  ToggleModLevelAlt   },  // 87 0x57  39  last button
        {     0,   0x3F, "Map mode select",         MappingSelect       },  // 88 0x58  40
        {     1,   0x3F, "Save Configuration",      SaveConfig          },  // 89 0x59  41
        {     2,   0x3F, "Load Configuration",      LoadConfig          },  // 90 0x5A  42
        {     3,   0x3F, "Page Advance",            PageAdvance         },  // 91 0x5B  43
        {     0,      0, "N ",                      nullptr             },  // 92 0x5C  44
        {     1,      0, "N ",                      nullptr             },  // 93 0x5D  45
        {     0,      0, "N ",                      nullptr             },  // 94 0x5E  46
        {     1,      0, "N ",                      nullptr             },  // 95 0x5F  47
        {     0,      0, "N ",                      nullptr             },  // 96 0x60  48
        {     0,      0, "N ",                      nullptr             },  // 97 0x61  49
        {     0,      0, "N ",                      nullptr             },  // 98 0x62  50
        {     0,      0, "N ",                      nullptr             },  // 99 0x63  51
        {     0,      0, "N ",                      nullptr             },  //100 0x64  52
        {     0,      0, "N ",                      nullptr             },  //101 0x65  53
        {     0,      0, "N ",                      nullptr             },  //102 0x66  54
        {     0,      0, "N ",                      nullptr             },  //103 0x67  55
      },
// XL_MIDI_MAP_MAPPING
      { {     0,   0x0C, "N ",                      nullptr             },  // 48 0x30  0
        {     0,   0x0C, "N ",                      nullptr             },  // 49 0x31  1
        {     0,   0x0C, "N ",                      nullptr             },  // 50 0x32  2
        {     0,   0x0C, "N ",                      nullptr             },  // 51 0x33  3
        {     0,   0x0C, "N ",                      nullptr             },  // 52 0x34  4
        {     0,   0x0C, "N ",                      nullptr             },  // 53 0x35  5
        {     0,   0x0C, "N ",                      nullptr             },  // 54 0x36  6
        {     0,   0x0C, "N ",                      nullptr             },  // 55 0x37  7
        {     0,   0x0C, "N ",                      nullptr             },  // 56 0x38  8
        {     0,   0x0C, "N ",                      nullptr             },  // 57 0x39  9
        {     0,   0x0C, "N ",                      nullptr             },  // 58 0x3A  10
        {     0,   0x0C, "N ",                      nullptr             },  // 59 0x3B  11
        {     0,   0x0C, "N ",                      nullptr             },  // 60 0x3C  12
        {     0,   0x0C, "N ",                      nullptr             },  // 61 0x3D  13
        {     0,   0x0C, "N ",                      nullptr             },  // 62 0x3E  14
        {     0,   0x0C, "N ",                      nullptr             },  // 63 0x3F  15
        {     0,   0x0C, "N ",                      nullptr             },  // 64 0x40  16
        {     0,   0x0C, "N ",                      nullptr             },  // 65 0x41  17
        {     0,   0x0C, "N ",                      nullptr             },  // 66 0x42  18
        {     0,   0x0C, "N ",                      nullptr             },  // 67 0x43  19
        {     0,   0x0C, "N ",                      nullptr             },  // 68 0x44  20
        {     0,   0x0C, "N ",                      nullptr             },  // 69 0x45  21
        {     0,   0x0C, "N ",                      nullptr             },  // 70 0x46  22
        {     0,   0x0C, "N ",                      nullptr             },  // 71 0x47  23
        {     0,   0x0C, "N ",                      nullptr             },  // 72 0x48  24  First button
        {     0,   0x0C, "N ",                      nullptr             },  // 73 0x49  25
        {     0,   0x0C, "N ",                      nullptr             },  // 74 0x4A  26
        {     0,   0x0C, "N ",                      nullptr             },  // 75 0x4B  27
        {     0,   0x0C, "N ",                      nullptr             },  // 76 0x4C  28
        {     0,   0x0C, "N ",                      nullptr             },  // 77 0x4D  29
        {     0,   0x0C, "N ",                      nullptr             },  // 78 0x4E  30
        {     0,   0x0C, "N ",                      nullptr             },  // 79 0x4D  31
        {     0,   0x0C, "N ",                      nullptr             },  // 80 0x50  32
        {     0,   0x0C, "N ",                      nullptr             },  // 81 0x51  33
        {     0,   0x0C, "N ",                      nullptr             },  // 82 0x52  34
        {     0,   0x0C, "N ",                      nullptr             },  // 83 0x53  35
        {     0,   0x0C, "N ",                      nullptr             },  // 84 0x54  36
        {     0,   0x0C, "N ",                      nullptr             },  // 85 0x55  37
        {     0,   0x0C, "N ",                      nullptr             },  // 86 0x56  38
        {     0,   0x0C, "N ",                      nullptr             },  // 87 0x57  39  last button
        {     0,   0x3F, "Map mode select",         MappingSelect       },  // 88 0x58  40
        {     1,   0x3F, "Save Configuration",      SaveConfig          },  // 89 0x59  41
        {     2,   0x0C, "Load Configuration",      nullptr             },  // 90 0x5A  42
        {     3,   0x0C, "Page Advance",            nullptr             },  // 91 0x5B  43
        {     0,   0x3F, "Send Sel UP",             SendDir             },  // 92 0x5C  44
        {     1,   0x3F, "Send Sel DN",             SendDir             },  // 93 0x5D  45
        {     0,   0x3F, "Track Sel Left",          TrackSel            },  // 94 0x5E  46
        {     1,   0x3F, "Track Sel Right",         TrackSel            },  // 95 0x5F  47
        {     0,      0, "N ",                      nullptr             },  // 96 0x60  48
        {     0,      0, "N ",                      nullptr             },  // 97 0x61  49
        {     0,      0, "N ",                      nullptr             },  // 98 0x62  50
        {     0,      0, "N ",                      nullptr             },  // 99 0x63  51
        {     0,      0, "N ",                      nullptr             },  //100 0x64  52
        {     0,      0, "N ",                      nullptr             },  //101 0x65  53
        {     0,      0, "N ",                      nullptr             },  //102 0x66  54
        {     0,      0, "N ",                      nullptr             },  //103 0x67  55
      },
// XL_MIDI_MAP_LOADSAVE
      { {     0,   0x0C, "N ",                      nullptr             },  // 48 0x30  0
        {     0,   0x0C, "N ",                      nullptr             },  // 49 0x31  1
        {     0,   0x0C, "N ",                      nullptr             },  // 50 0x32  2
        {     0,   0x0C, "N ",                      nullptr             },  // 51 0x33  3
        {     0,   0x0C, "N ",                      nullptr             },  // 52 0x34  4
        {     0,   0x0C, "N ",                      nullptr             },  // 53 0x35  5
        {     0,   0x0C, "N ",                      nullptr             },  // 54 0x36  6
        {     0,   0x0C, "N ",                      nullptr             },  // 55 0x37  7
        {     0,   0x0C, "N ",                      nullptr             },  // 56 0x38  8
        {     0,   0x0C, "N ",                      nullptr             },  // 57 0x39  9
        {     0,   0x0C, "N ",                      nullptr             },  // 58 0x3A  10
        {     0,   0x0C, "N ",                      nullptr             },  // 59 0x3B  11
        {     0,   0x0C, "N ",                      nullptr             },  // 60 0x3C  12
        {     0,   0x0C, "N ",                      nullptr             },  // 61 0x3D  13
        {     0,   0x0C, "N ",                      nullptr             },  // 62 0x3E  14
        {     0,   0x0C, "N ",                      nullptr             },  // 63 0x3F  15
        {     0,   0x0C, "N ",                      nullptr             },  // 64 0x40  16
        {     0,   0x0C, "N ",                      nullptr             },  // 65 0x41  17
        {     0,   0x0C, "N ",                      nullptr             },  // 66 0x42  18
        {     0,   0x0C, "N ",                      nullptr             },  // 67 0x43  19
        {     0,   0x0C, "N ",                      nullptr             },  // 68 0x44  20
        {     0,   0x0C, "N ",                      nullptr             },  // 69 0x45  21
        {     0,   0x0C, "N ",                      nullptr             },  // 70 0x46  22
        {     0,   0x0C, "N ",                      nullptr             },  // 71 0x47  23
        {     0,   0x0C, "N ",                      nullptr             },  // 72 0x48  24  First button
        {     0,   0x0C, "N ",                      nullptr             },  // 73 0x49  25
        {     0,   0x0C, "N ",                      nullptr             },  // 74 0x4A  26
        {     0,   0x0C, "N ",                      nullptr             },  // 75 0x4B  27
        {     0,   0x0C, "N ",                      nullptr             },  // 76 0x4C  28
        {     0,   0x0C, "N ",                      nullptr             },  // 77 0x4D  29
        {     0,   0x0C, "N ",                      nullptr             },  // 78 0x4E  30
        {     0,   0x0C, "N ",                      nullptr             },  // 79 0x4D  31
        {     0,   0x0C, "N ",                      nullptr             },  // 80 0x50  32
        {     0,   0x0C, "N ",                      nullptr             },  // 81 0x51  33
        {     0,   0x0C, "N ",                      nullptr             },  // 82 0x52  34
        {     0,   0x0C, "N ",                      nullptr             },  // 83 0x53  35
        {     0,   0x0C, "N ",                      nullptr             },  // 84 0x54  36
        {     0,   0x0C, "N ",                      nullptr             },  // 85 0x55  37
        {     0,   0x0C, "N ",                      nullptr             },  // 86 0x56  38
        {     0,   0x0C, "N ",                      nullptr             },  // 87 0x57  39  last button
        {     0,   0x3F, "Map mode select",         MappingSelect       },  // 88 0x58  40
        {     1,   0x3F, "Save Configuration",      SaveConfig          },  // 89 0x59  41
        {     2,   0x3F, "Load Configuration",      LoadConfig          },  // 90 0x5A  42
        {     3,   0x3F, "Page Advance",            PageAdvance         },  // 91 0x5B  43
        {     0,   0x3F, "Send Sel UP",             SendDir             },  // 92 0x5C  44
        {     1,   0x3F, "Send Sel DN",             SendDir             },  // 93 0x5D  45
        {     0,      0, "N ",                      nullptr             },  // 94 0x5E  46
        {     1,      0, "N ",                      nullptr             },  // 95 0x5F  47
        {     0,      0, "N ",                      nullptr             },  // 96 0x60  48
        {     0,      0, "N ",                      nullptr             },  // 97 0x61  49
        {     0,      0, "N ",                      nullptr             },  // 98 0x62  50
        {     0,      0, "N ",                      nullptr             },  // 99 0x63  51
        {     0,      0, "N ",                      nullptr             },  //100 0x64  52
        {     0,      0, "N ",                      nullptr             },  //101 0x65  53
        {     0,      0, "N ",                      nullptr             },  //102 0x66  54
        {     0,      0, "N ",                      nullptr             },  //103 0x67  55
      },
// XL_MIDI_MAP_SPARE
      { {     0,   0x0C, "N ",                      nullptr             },  // 48 0x30  0
        {     0,   0x0C, "N ",                      nullptr             },  // 49 0x31  1
        {     0,   0x0C, "N ",                      nullptr             },  // 50 0x32  2
        {     0,   0x0C, "N ",                      nullptr             },  // 51 0x33  3
        {     0,   0x0C, "N ",                      nullptr             },  // 52 0x34  4
        {     0,   0x0C, "N ",                      nullptr             },  // 53 0x35  5
        {     0,   0x0C, "N ",                      nullptr             },  // 54 0x36  6
        {     0,   0x0C, "N ",                      nullptr             },  // 55 0x37  7
        {     0,   0x0C, "N ",                      nullptr             },  // 56 0x38  8
        {     0,   0x0C, "N ",                      nullptr             },  // 57 0x39  9
        {     0,   0x0C, "N ",                      nullptr             },  // 58 0x3A  10
        {     0,   0x0C, "N ",                      nullptr             },  // 59 0x3B  11
        {     0,   0x0C, "N ",                      nullptr             },  // 60 0x3C  12
        {     0,   0x0C, "N ",                      nullptr             },  // 61 0x3D  13
        {     0,   0x0C, "N ",                      nullptr             },  // 62 0x3E  14
        {     0,   0x0C, "N ",                      nullptr             },  // 63 0x3F  15
        {     0,   0x0C, "N ",                      nullptr             },  // 64 0x40  16
        {     0,   0x0C, "N ",                      nullptr             },  // 65 0x41  17
        {     0,   0x0C, "N ",                      nullptr             },  // 66 0x42  18
        {     0,   0x0C, "N ",                      nullptr             },  // 67 0x43  19
        {     0,   0x0C, "N ",                      nullptr             },  // 68 0x44  20
        {     0,   0x0C, "N ",                      nullptr             },  // 69 0x45  21
        {     0,   0x0C, "N ",                      nullptr             },  // 70 0x46  22
        {     0,   0x0C, "N ",                      nullptr             },  // 71 0x47  23
        {     0,   0x0C, "N ",                      nullptr             },  // 72 0x48  24  First button
        {     0,   0x0C, "N ",                      nullptr             },  // 73 0x49  25
        {     0,   0x0C, "N ",                      nullptr             },  // 74 0x4A  26
        {     0,   0x0C, "N ",                      nullptr             },  // 75 0x4B  27
        {     0,   0x0C, "N ",                      nullptr             },  // 76 0x4C  28
        {     0,   0x0C, "N ",                      nullptr             },  // 77 0x4D  29
        {     0,   0x0C, "N ",                      nullptr             },  // 78 0x4E  30
        {     0,   0x0C, "N ",                      nullptr             },  // 79 0x4D  31
        {     0,   0x0C, "N ",                      nullptr             },  // 80 0x50  32
        {     0,   0x0C, "N ",                      nullptr             },  // 81 0x51  33
        {     0,   0x0C, "N ",                      nullptr             },  // 82 0x52  34
        {     0,   0x0C, "N ",                      nullptr             },  // 83 0x53  35
        {     0,   0x0C, "N ",                      nullptr             },  // 84 0x54  36
        {     0,   0x0C, "N ",                      nullptr             },  // 85 0x55  37
        {     0,   0x0C, "N ",                      nullptr             },  // 86 0x56  38
        {     0,   0x0C, "N ",                      nullptr             },  // 87 0x57  39  last button
        {     0,   0x0C, "N ",                      nullptr             },  // 88 0x58  40
        {     0,   0x0C, "N ",                      nullptr             },  // 89 0x59  41
        {     0,   0x0C, "N ",                      nullptr             },  // 90 0x5A  42
        {     0,   0x0C, "N ",                      nullptr             },  // 91 0x5B  43
        {     0,   0x0C, "N ",                      nullptr             },  // 92 0x5C  44
        {     0,   0x0C, "N ",                      nullptr             },  // 93 0x5D  45
        {     0,   0x0C, "N ",                      nullptr             },  // 94 0x5E  46
        {     0,   0x0C, "N ",                      nullptr             },  // 95 0x5F  47
        {     0,      0, "Level Sine",              SetTuningLevel      },  // 96 0x60  48
        {     1,      0, "Level Triangle",          SetTuningLevel      },  // 97 0x61  49
        {     2,      0, "Level Ramp",              SetTuningLevel      },  // 98 0x62  50
        {     3,      0, "Level Pulse",             SetTuningLevel      },  // 99 0x63  51
        {     4,      0, "Level Noise",             SetTuningLevel      },  //100 0x64  52
        {     0,      0, "N ",                      nullptr             },  //101 0x65  53
        {     0,      0, "N ",                      nullptr             },  //102 0x66  54
        {     0,      0, "N ",                      nullptr             },  //103 0x67  55
      },
  };

//########################################################
//########################################################
G49_FADER_MIDI_MAP FaderMidiMapArray[] =
    {   {  0, "N ",                     nullptr     },  // 01  07  xx
        {  1, "N ",                     nullptr     },  // 02  07  xx
        {  2, "N ",                     nullptr     },  // 03  07  xx
        {  3, "N ",                     nullptr     },  // 04  07  xx
        {  4, "N ",                     nullptr     },  // 05  07  xx
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
G49_ENCODER_MIDI_MAP KnobMidiMapArray[] =
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
G49_BUTTON_MIDI_MAP SwitchMidiMapArray[] =
    {   {  0,   false,  "N ",           nullptr,    },  //  16  1
        {  1,   false,  "N ",           nullptr,    },  //  16  2
        {  2,   false,  "N ",           nullptr,    },  //  16  3
        {  3,   false,  "N ",           nullptr,    },  //  16  4
        {  4,   false,  "N ",           nullptr,    },  //  16  5
        {  5,   false,  "N ",           nullptr,    },  //  16  6
        {  6,   false,  "N ",           nullptr,    },  //  16  7
        {  7,   false,  "N ",           nullptr,    },  //  16  8
        {  0,   false,  "N ",           nullptr,    },  //  16  9
        {  1,   false,  "N ",           nullptr,    },  //  16  10
        {  2,   false,  "N ",           nullptr,    },  //  16  11
        {  3,   false,  "N ",           nullptr,    },  //  16  12
        {  4,   false,  "N ",           nullptr,    },  //  16  13
        {  5,   false,  "N ",           nullptr,    },  //  16  14
        {  6,   false,  "N ",           nullptr,    },  //  16  15
        {  7,   false,  "N ",           nullptr,    },  //  16  16
        {  0,   false,  "Tune -",       TuneUpDown  },  //  17  1
        {  1,   false,  "Tune +",       TuneUpDown  },  //  17  2
        { 22,   false,  "Tune/Reset",   TuneReset   },  //  17  3
        { 23,   false,  "Tune -/+",     TuneBump    },  //  17  4
        { 24,   false,  "Tuning save",  TunningSave },  //  17  5
     };















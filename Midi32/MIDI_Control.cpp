//#######################################################################
// Module:     Controls.cpp
// Descrption: Synthesizer midi control routing
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>

#include "Config.h"
#include "FrontEnd.h"
#include "Debug.h"
#include "MidiConf.h"

#ifdef DEBUG_SYNTH
static const char* Label  = "TOP";
static const char* LabelM = "M";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#define DBGM(args...) {if(DebugMidi){DebugMsg(LabelM,mchan,args);}}
#else
#define DBG(args...)
#define DBGM(args...)
#endif

//-------------------------------------------------------------------
//-------------------------------------------------------------------
static void cb_Message_Novation (const MidiMessage& msg)
    {
    printf ("*** Novation MESSAGE:  type = 0x%02X  channel = %2d   data1 = 0x%02X   data2 = 0x%02X   length = 0x%02X\n",
            msg.type, msg.channel, msg.data1, msg.data2, msg.length);
    }

//-------------------------------------------------------------------
static void cb_SystemEx_Novation (byte* array, unsigned size)
    {
    printf ("\n\n*** Novation SYSEX");
    SystemExDebug (array, size);
    }

//-------------------------------------------------------------------
static void cb_SystemReset_Novation (void)
    {
    printf ("\n\n*** Novation RESET\n");
    }

//-------------------------------------------------------------------
static void cb_Error_Novation (int8_t err)
    {
    printf ("\n\n*** Novation ERROR %d\n", err);
    }

//-------------------------------------------------------------------
//-------------------------------------------------------------------
void cb_ControllerControl (byte mchan, byte type, byte value)
    {
    int index;

    switch ( type )
        {
        case 0x30 ... 0x47:
        case 0x60 ... 0x67:
            {
            index = type - 0x30;
            XL_MIDI_MAP& m = XL_MidiMapArray[LaunchControl.GetCurrentMap()] [index];
            DBG ("%s > %d    ", m.Desc, value);
            if ( m.CallBack != nullptr )
                m.CallBack (m.Index, value);
            }
            break;
        case 0x48 ... 0x5F:
            {
            index     = type - 0x30;                         // offset to start of control map
            bool tgl = value > 0x3C;                         // use color green as threshold for button down
            XL_MIDI_MAP& m = XL_MidiMapArray[LaunchControl.GetCurrentMap()] [index];
            DBG ("%s %s", m.Desc, (( tgl ) ? "Dn" : "Up"));

            if ( !tgl && (m.CallBack != nullptr) )
                m.CallBack (m.Index, (short)tgl);
            }
            break;

        default:
            DBG ("Invalid type code = %d [%x]\n", type, type);
            break;
        }
    }

//########################################################
//########################################################
//   VCO / VCA / VCF controls
//########################################################
//########################################################
static void setLevelSelect (short ch, short state)
    {
    VoiceLevelSelect (ch, state);
    }

//########################################################
static void DamperToggle (short ch, short state)
    {
    VoiceDamperToggle (ch);
    }

//########################################################
static void MuteVoiceToggle (short ch, short data)
    {
    MuteVoiceToggle ();
    }

//########################################################
static void pulseWidth (short ch, short data)
    {
    if ( data == 0 )
        data = 1;
    SetPulseWidth (data);
    }

//########################################################
//########################################################
//  Filter controls
//########################################################
//########################################################
static void freqCtrlModeAdv (short ch, short data)
    {
    FreqCtrlModeAdv (ch);
    }

//########################################################
//########################################################
//  LFO controls
//########################################################
//########################################################
static void toggleModVCA (short ch, short data)
    {
    SelectModVCA (ch);
    }

//########################################################
static void toggleModLevelAlt (short ch, short data)
    {
    ToggleModLevelAlt (ch);
    }

//########################################################
static void toggleModRampDir (short ch, short data)
    {
    ToggleModRampDir (ch);
    }

//########################################################
static void toggleModVCO (short index, short data)
    {
    if ( index < 3 )
        SelectModVCO (0, index);
    else
        SelectModVCO (1, index - 4);
    }

//########################################################
//########################################################
//########################################################
//  Channel to voice mapping controls
//########################################################
static void trackSel (short index, short data)
    {
    if ( MapSelectMode )
        ChangeMapSelect (index);
    }

//########################################################
static void mappingSelect (short index, short data)
    {
    MidiMapMode ();
    }

//########################################################
static void sendDir (short index, short data)
    {
    if ( MapSelectMode )
        MapModeBump (( index ) ? -1 : 1);
    else if ( LoadMode || SaveMode)
        LoadSaveBump (( index ) ? -1 : 1);
    }

//########################################################
static void loadConfig (short index, short data)
    {
    if ( LoadMode )
        LoadSelectedConfig ();
    else if ( !SaveMode )
        OpenLoadSavePage(false);
    }

//########################################################
static void saveConfig (short index, short data)
    {
    if ( MapSelectMode )
        SaveDefaultConfig ();
    else if ( SaveMode )
        SaveSelectedConfig ();
    else if ( !LoadMode )
        OpenLoadSavePage (true);
     }

//########################################################
static void selectFilter (short index, short data)
    {
    SelectFilter (index);
    }

//########################################################
static void dummyButton (short index, short data)
    {
    LaunchControl.TemplateRefresh ();
    }

//########################################################
//########################################################
// Page advance selection (next page)
//########################################################
static void pageAdvance (short ch, short data)
    {
    PageAdvance ();
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
        {   127,   0x3C, "Pulse Width",             pulseWidth          },  // 71 0x47  23
        {     0,   0x3C, "Set Sine L/S",            setLevelSelect      },  // 72 0x48  24  First button
        {     1,   0x3C, "Set Triangle L/S",        setLevelSelect      },  // 73 0x49  25
        {     2,   0x3C, "Set Ramp  L/S",           setLevelSelect      },  // 74 0x4A  26
        {     3,   0x3C, "Set Pulse L/S ",          setLevelSelect      },  // 75 0x4B  27
        {     4,   0x3C, "Set Noise L/S ",          setLevelSelect      },  // 76 0x4C  28
        {     5,   0x0C, "N ",                      nullptr             },  // 77 0x4D  29
        {     6,   0x0C, "N ",                      nullptr             },  // 78 0x4E  30
        {     7,   0x3C, "Sawtooth Dir",            ToggleRampDirection },  // 79 0x4D  31
        {     0,   0x3C, "Sine Damper on",          DamperToggle        },  // 80 0x50  32
        {     1,   0x3C, "Triangle Damper on",      DamperToggle        },  // 81 0x51  33
        {     2,   0x3C, "Ramp Damper on",          DamperToggle        },  // 82 0x52  34
        {     3,   0x3C, "Pulse Damper on",         DamperToggle        },  // 83 0x53  35
        {     4,   0x3C, "Noise Damper on",         DamperToggle        },  // 84 0x54  36
        {    13,   0x0C, "N ",                      nullptr             },  // 85 0x55  37
        {    14,   0x0C, "N ",                      nullptr             },  // 86 0x56  38
        {    15,   0x3C, "Mute Voice",              MuteVoiceToggle     },  // 87 0x57  39  last button
        {     0,   0x3F, "Map mode select",         mappingSelect       },  // 88 0x58  40
        {     1,   0x3F, "Save Configuration",      saveConfig          },  // 89 0x59  41
        {     2,   0x3F, "Load Configuration",      loadConfig          },  // 90 0x5A  42
        {     3,   0x3F, "Page Advance",            pageAdvance         },  // 91 0x5B  43
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
        {     7,      0, "Master level",            SetMasterLevel      },  //103 0x67  55
      },
// XL_MIDI_MAP_FILTER
      { {     8,   0x3C, "Attack Freq",             SetAttackTime       },  // 48 0x30  0
        {     0,   0x0C, "N ",                      nullptr             },  // 49 0x31  1
        {     0,   0x0C, "N ",                      nullptr             },  // 50 0x32  2
        {     9,   0x2E, "Attack Q",                SetAttackTime       },  // 51 0x33  3
        {     0,   0x0C, "N ",                      nullptr             },  // 52 0x34  4
        {     0,   0x0C, "N ",                      nullptr             },  // 53 0x35  5
        {     0,   0x0C, "N ",                      nullptr             },  // 54 0x36  6
        {     0,   0x0C, "N ",                      nullptr             },  // 55 0x37  7
        {     8,   0x3C, "Decay Freq",              SetDecayTime        },  // 56 0x38  8
        {     0,   0x0C, "N ",                      nullptr             },  // 57 0x39  9
        {     0,   0x0C, "N ",                      nullptr             },  // 58 0x3A  10
        {     9,   0x2E, "Decay Q",                 SetDecayTime        },  // 59 0x3B  11
        {     0,   0x0C, "N ",                      nullptr             },  // 60 0x3C  12
        {     0,   0x0C, "N ",                      nullptr             },  // 61 0x3D  13
        {     0,   0x0C, "N ",                      nullptr             },  // 62 0x3E  14
        {     0,   0x0C, "N ",                      nullptr             },  // 63 0x3F  15
        {     8,   0x3C, "Release Freq",            SetReleaseTime      },  // 64 0x40  16
        {     0,   0x0C, "N ",                      nullptr             },  // 65 0x41  17
        {     0,   0x0C, "N ",                      nullptr             },  // 66 0x42  18
        {     9,   0x2E, "Release Q",               SetReleaseTime      },  // 67 0x43  19
        {     0,   0x0C, "N ",                      nullptr             },  // 68 0x44  20
        {     0,   0x0C, "N ",                      nullptr             },  // 69 0x45  21
        {     0,   0x0C, "N ",                      nullptr             },  // 70 0x46  22
        {     0,   0x0C, "N ",                      nullptr             },  // 71 0x47  23
        {     0,   0x1C, "N ",                      dummyButton         },  // 72 0x48  24  First button
        {     0,   0x1C, "N ",                      dummyButton         },  // 73 0x49  25
        {     0,   0x1C, "N ",                      dummyButton         },  // 74 0x4A  26
        {     0,   0x1D, "N ",                      dummyButton         },  // 75 0x4B  27
        {     0,   0x1D, "N ",                      dummyButton         },  // 76 0x4C  28
        {     0,   0x1D, "N ",                      dummyButton         },  // 77 0x4D  29
        {     0,   0x0C, "N ",                      nullptr             },  // 78 0x4E  30
        {     0,   0x3C, "Freq ctrl mode advance",  freqCtrlModeAdv     },  // 79 0x4D  31
        {     0,   0x3C, "Select Bypass",           selectFilter        },  // 80 0x50  32
        {     1,   0x3C, "Select LP",               selectFilter        },  // 81 0x51  33
        {     2,   0x3C, "Select LBP",              selectFilter        },  // 82 0x52  34
        {     3,   0x3C, "Select UBP",              selectFilter        },  // 83 0x53  35
        {     4,   0x3C, "Select HP",               selectFilter        },  // 84 0x54  36
        {     0,   0x0C, "N ",                      nullptr             },  // 85 0x55  37
        {     0,   0x0C, "N ",                      nullptr             },  // 86 0x56  38
        {     1,   0x3C, "Q ctrl mode advance",     freqCtrlModeAdv     },  // 87 0x57  39  last button
        {     0,   0x3F, "Map mode select",         mappingSelect       },  // 88 0x58  40
        {     1,   0x3F, "Save Configuration",      saveConfig          },  // 89 0x59  41
        {     2,   0x3F, "Load Configuration",      loadConfig          },  // 90 0x5A  42
        {     3,   0x3F, "Page Advance",            pageAdvance         },  // 91 0x5B  43
        {     0,   0x0C, "N ",                      nullptr             },  // 92 0x5C  44
        {     1,   0x0C, "N ",                      nullptr             },  // 93 0x5D  45
        {     0,   0x0C, "N ",                      nullptr             },  // 94 0x5E  46
        {     1,   0x0C, "N ",                      nullptr             },  // 95 0x5F  47
        {     8,      0, "FLT FREQ START",          FltStart            },  // 96 0x60  48
        {     8,      0, "FLT FREQ END",            FltEnd              },  // 97 0x61  49
        {     8,      0, "FLT FREQ SUSTAIN",        SetSustainLevel     },  // 98 0x62  50
        {     9,      0, "FLT Q START",             FltStart            },  // 99 0x63  51
        {     9,      0, "FLT Q END",               FltEnd              },  //100 0x64  52
        {     9,      0, "FLT Q SUSTAIN",           SetSustainLevel     },  //101 0x65  53
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
        {     0,   0x3C, "LFO 1 freq Mod Sine",     toggleModVCO        },  // 72 0x48  24  First button
        {     1,   0x3C, "LFO 1 freq Mod Ramp",     toggleModVCO        },  // 73 0x49  25
        {     2,   0x3C, "LFO 1 freq Pulse",        toggleModVCO        },  // 74 0x4A  26
        {     0,   0x3C, "Toggle Ramp 1 Direction", toggleModRampDir    },  // 75 0x4B  27
        {     4,   0x3C, "LFO 2 freq Mod Sine",     toggleModVCO        },  // 76 0x4C  28
        {     5,   0x3C, "LFO 2 freq Mod Ramp",     toggleModVCO        },  // 77 0x4D  29
        {     6,   0x3C, "LFO 2 freq Pulse",        toggleModVCO        },  // 78 0x4E  30
        {     1,   0x3C, "Toggle Ramp2  Direction", toggleModRampDir    },  // 79 0x4D  31
        {     0,   0x3C, "VCA Mod Sine",            toggleModVCA        },  // 80 0x50  32
        {     1,   0x3C, "VCA Mod Triangle",        toggleModVCA        },  // 81 0x51  33
        {     2,   0x3C, "VCA Mod Ramp",            toggleModVCA        },  // 82 0x52  34
        {     3,   0x3C, "VCA Mod Pulse",           toggleModVCA        },  // 83 0x53  35
        {     4,   0x3C, "VCA Mod Noise",           toggleModVCA        },  // 84 0x54  36
        {     0,      0, "N ",                      nullptr             },  // 85 0x55  37
        {     0,   0x3C, "Toggle LFO 0 Mod Level",  toggleModLevelAlt   },  // 86 0x56  38
        {     1,   0x3C, "Toggle LFO 1 Mod Level",  toggleModLevelAlt   },  // 87 0x57  39  last button
        {     0,   0x3F, "Map mode select",         mappingSelect       },  // 88 0x58  40
        {     1,   0x3F, "Save Configuration",      saveConfig          },  // 89 0x59  41
        {     2,   0x3F, "Load Configuration",      loadConfig          },  // 90 0x5A  42
        {     3,   0x3F, "Page Advance",            pageAdvance         },  // 91 0x5B  43
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
        {     0,   0x3F, "Map mode select",         mappingSelect       },  // 88 0x58  40
        {     1,   0x3F, "Save Configuration",      saveConfig          },  // 89 0x59  41
        {     2,   0x0C, "Load Configuration",      nullptr             },  // 90 0x5A  42
        {     3,   0x0C, "Page Advance",            nullptr             },  // 91 0x5B  43
        {     0,   0x3F, "Send Sel UP",             sendDir             },  // 92 0x5C  44
        {     1,   0x3F, "Send Sel DN",             sendDir             },  // 93 0x5D  45
        {     0,   0x3F, "Track Sel Left",          trackSel            },  // 94 0x5E  46
        {     1,   0x3F, "Track Sel Right",         trackSel            },  // 95 0x5F  47
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
        {     0,   0x3F, "Map mode select",         mappingSelect       },  // 88 0x58  40
        {     1,   0x3F, "Save Configuration",      saveConfig          },  // 89 0x59  41
        {     2,   0x3F, "Load Configuration",      loadConfig          },  // 90 0x5A  42
        {     3,   0x3F, "Page Advance",            pageAdvance         },  // 91 0x5B  43
        {     0,   0x3F, "Send Sel UP",             sendDir             },  // 92 0x5C  44
        {     1,   0x3F, "Send Sel DN",             sendDir             },  // 93 0x5D  45
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
// XL_MIDI_MAP_TUNING
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
        {     0,   0x3C, "N ",                      nullptr             },  // 72 0x48  24  First button
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
        {     0,      0, "N ",                      nullptr             },  // 96 0x60  48  First Fader
        {     1,      0, "N ",                      nullptr             },  // 97 0x61  49
        {     2,      0, "N ",                      nullptr             },  // 98 0x62  50
        {     3,      0, "N ",                      nullptr             },  // 99 0x63  51
        {     4,      0, "N ",                      nullptr             },  //100 0x64  52
        {     0,      0, "N ",                      nullptr             },  //101 0x65  53
        {     0,      0, "N ",                      nullptr             },  //102 0x66  54
        {     0,      0, "N ",                      nullptr             },  //103 0x67  55
      },
  };

//#######################################################################
void InitMidiControl ()
    {
    LaunchControl.Begin ();

//    Midi_0.setHandleNoteOn               (cb_KeyDown_Novation);
//    Midi_0.setHandleNoteOff              (cb_KeyUp_Novation);
    Midi_0.setHandleControlChange        (cb_ControllerControl);
//    Midi_0.setHandlePitchBend            (cb_PitchBendNovaton);
    Midi_0.setHandleError                (cb_Error_Novation);
//    Midi_0.setHandleAfterTouchPoly       (AfterTouchPolyCallback fptr);
//    Midi_0.setHandleProgramChange        (ProgramChangeCallback fptr);
//    Midi_0.setHandleAfterTouchChannel    (AfterTouchChannelCallback fptr);
//    Midi_0.setHandleTimeCodeQuarterFrame (TimeCodeQuarterFrameCallback fptr);
//    Midi_0.setHandleSongPosition         (SongPositionCallback fptr);
//    Midi_0.setHandleSongSelect           (SongSelectCallback fptr);
//    Midi_0.setHandleTuneRequest          (TuneRequestCallback fptr);
//    Midi_0.setHandleClock                (ClockCallback fptr);
//    Midi_0.setHandleStart                (StartCallback fptr);
//    Midi_0.setHandleTick                 (TickCallback fptr);
//    Midi_0.setHandleContinue             (ContinueCallback fptr);
//    Midi_0.setHandleStop                 (StopCallback fptr);
//    Midi_0.setHandleActiveSensing        (ActiveSensingCallback fptr)
//    Midi_0.setHandleSystemExclusive      (Cb_SystemEx_Novation);
    Midi_0.setHandleSystemReset          (cb_SystemReset_Novation);
#ifdef DEBUG_MIDI_MSG           // Enable all messages to print on debug terminal
    Midi_0.setHandleMessage              (cb_Message_Novation);
#endif
    }


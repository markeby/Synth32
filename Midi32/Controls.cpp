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
//########################################################
//   VCO / VCA / VCF controls
//########################################################
static void setLevel (short ch, short data)
    {
    SynthFront.SetLevel (ch, data);
    }

//########################################################
static void setLevelSelect (short ch, short state)
    {
    SynthFront.VoiceLevelSelect (ch, state);
    }

//########################################################
static void DamperToggle (short ch, short state)
    {
    SynthFront.VoiceDamperToggle (ch, state);
    }

//########################################################
static void setAttackTime (short ch, short data)
    {
    SynthFront.SetAttackTime (ch, data);
    }

//########################################################
static void setDecayTime (short ch, short data)
    {
    SynthFront.SetDecayTime (ch, data);
    }

//########################################################
static void setReleaseTime (short ch, short data)
    {
    SynthFront.SetReleaseTime (ch, data);
    }

//########################################################
static void MuteVoiceToggle (short ch, short data)
    {
    SynthFront.MuteVoiceToggle ();
    }

//########################################################
static void toogleRamp (short ch, short data)
    {
    SynthFront.ToggleRampDirection (ch);
    }

//########################################################
static void pulseWidth (short ch, short data)
    {
    if ( data == 0 )
        data = 1;
    SynthFront.SetPulseWidth (data);
    }

//########################################################
//########################################################
//  Filter controls
//########################################################
static void fltStart (short ch, short data)
    {
    SynthFront.FltStart (ch, data);
    }

//########################################################
static void fltEnd  (short ch, short data)
    {
    SynthFront.FltEnd (ch, data);
    }

//########################################################
static void fltSustain (short ch, short data)
    {
    SynthFront.SetSustainLevel (ch, data);
    }

//########################################################
static void freqCtrlModeAdv (short ch, short data)
    {
    SynthFront.FreqCtrlModeAdv (ch);
    }

//########################################################
//########################################################
//  LFO controls
//########################################################
static void freqLFO (short ch, short data)
    {
    SynthFront.FreqLFO (ch, data);
    }

//########################################################
static void toggleModVCA (short ch, short data)
    {
    SynthFront.SelectModVCA (ch);
    }

//########################################################
static void toggleModLevelAlt (short ch, short data)
    {
    SynthFront.ToggleModLevelAlt (ch);
    }

//########################################################
static void toggleModRampDir (short ch, short data)
    {
    SynthFront.ToggleModRampDir (ch);
    }

//########################################################
static void toggleModVCO (short ch, short data)
    {
    if ( ch < 3 )
        SynthFront.SelectModVCO (0, ch);
    else
        SynthFront.SelectModVCO (1, ch - 4);
    }

//########################################################
//########################################################
//  Tuning control
//########################################################
static void tuneReset (short ch, bool state)
    {
    if ( state )
        SynthFront.StartCalibration ();
    else
        Monitor.Reset ();
    }

//########################################################
static void faderG49 (short ch, short data)
    {
    if ( ch == 8 )
        SynthFront.MasterVolume (data);
    else if ( SynthFront.IsInTuning () )
        {
        switch ( ch )
            {
            case 0 ... 4:
                SynthFront.SetTuningLevel (ch, data);
                break;
            case 6 ... 7:
                SynthFront.SetTuningFilter (ch - 6, data);
                break;
            default:
                break;
            }
        }
    }

//########################################################
static void tuneUpDown (short ch, bool state)
    {
    SynthFront.TuningAdjust (ch);
    }

//########################################################
static void tuneBump (short ch, bool state)
    {
    if ( SynthFront.IsInTuning () )
        SynthFront.TuningBump (state);
    else
        SynthFront.KeyboardKapture (state);
    }

//########################################################
static void tunningSave (short ch, bool state)
    {
    SynthFront.SaveTuning ();
    }

//########################################################
//########################################################
//  Channel to voice mapping controls
//########################################################
static void trackSel (short ch, short data)
    {
    if ( SynthFront.GetMidiMapMode () )
        SynthFront.ChangeMapSelect (ch);
    }

//########################################################
static void mappingSelect (short index, short data)
    {
    SynthFront.MidiMapMode ();
    }

//########################################################
static void sendDir (short index, short data)
    {
    if ( SynthFront.GetMidiMapMode () )
        SynthFront.MapModeBump (( index ) ? -1 : 1);
    else if ( SynthFront.GetLoadSaveMode () )
        SynthFront.LoadSaveBump (( index ) ? -1 : 1);
    }

//########################################################
static void loadConfig (short index, short data)
    {
    if ( SynthFront.GetLoadSaveMode () )
        SynthFront.LoadSelectedConfig ();
    else
        SynthFront.OpenLoadSavePage ();
    }

//########################################################
static void saveConfig (short index, short data)
    {
    if ( SynthFront.GetMidiMapMode () )
        SynthFront.SaveDefaultConfig ();
    else if ( SynthFront.GetLoadSaveMode () )
        SynthFront.SaveSelectedConfig ();
    else
        SynthFront.OpenLoadSavePage ();
     }

//########################################################
static void selectFilter (short index, short data)
    {
    SynthFront.SelectFilter (index);
    }

//########################################################
static void dummyButton (short index, short data)
    {
    SynthFront.TemplateRefresh ();
    }

//########################################################
//########################################################
// Page advance selection (next page)
//########################################################
static void pageAdvance (short ch, short data)
    {
    SynthFront.PageAdvance ();
    }

//########################################################
//########################################################
XL_MIDI_MAP    XL_MidiMapArray[XL_MIDI_MAP_PAGES][XL_MIDI_MAP_SIZE] =
    {
// XL_MIDI_MAP_OSC
      { {     0,   0x3C, "Attack Sine",             setAttackTime       },  // 48 0x30  0
        {     1,   0x3C, "Attack Triangle",         setAttackTime       },  // 49 0x31  1
        {     2,   0x3C, "Attack Ramp",             setAttackTime       },  // 50 0x32  2
        {     3,   0x3C, "Attack Pulse",            setAttackTime       },  // 51 0x33  3
        {     4,   0x3C, "Attack Noise",            setAttackTime       },  // 52 0x34  4
        {     5,   0x0C, "N ",                      nullptr             },  // 53 0x35  5
        {     6,   0x0C, "N ",                      nullptr             },  // 54 0x36  6
        {     7,   0x0C, "N ",                      nullptr             },  // 55 0x37  7
        {     0,   0x3C, "Decay Sine",              setDecayTime        },  // 56 0x38  8
        {     1,   0x3C, "Decay Triangle",          setDecayTime        },  // 57 0x39  9
        {     2,   0x3C, "Decay Ramp",              setDecayTime        },  // 58 0x3A  10
        {     3,   0x3C, "Decay Pulse",             setDecayTime        },  // 59 0x3B  11
        {     4,   0x3C, "Decay Noise",             setDecayTime        },  // 60 0x3C  12
        {     5,   0x0C, "N ",                      nullptr             },  // 61 0x3D  13
        {     6,   0x0C, "N ",                      nullptr             },  // 62 0x3E  14
        {     7,   0x0C, "N ",                      nullptr             },  // 63 0x3F  15
        {     0,   0x3C, "Release Sine",            setReleaseTime      },  // 64 0x40  16
        {     1,   0x3C, "Release Triangle",        setReleaseTime      },  // 65 0x41  17
        {     2,   0x3C, "Release Ramp",            setReleaseTime      },  // 66 0x42  18
        {     3,   0x3C, "Release Pulse",           setReleaseTime      },  // 67 0x43  19
        {     4,   0x3C, "Release Noise",           setReleaseTime      },  // 68 0x44  20
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
        {     7,   0x3C, "Sawtooth Dir",            toogleRamp          },  // 79 0x4D  31
        {     0,   0x3C, "Sine Damper on",          DamperToggle        },  // 80 0x50  32
        {     1,   0x3C, "Triangle Damper on",      DamperToggle        },  // 81 0x51  33
        {     2,   0x3C, "Ramp Damper on",          DamperToggle        },  // 82 0x52  34
        {     3,   0x0C, "Pulse Damper on",         DamperToggle        },  // 83 0x53  35
        {     4,   0x0C, "Noise Damper on",         DamperToggle        },  // 84 0x54  36
        {    13,   0x0C, "N ",                      nullptr             },  // 85 0x55  37
        {    14,   0x0C, "N ",                      nullptr             },  // 86 0x56  38
        {    15,   0x3C, "Mute Voice",              nullptr             },  // 87 0x57  39  last button
        {     0,   0x3F, "Map mode select",         mappingSelect       },  // 88 0x58  40
        {     1,   0x3F, "Save Configuration",      saveConfig          },  // 89 0x59  41
        {     2,   0x3F, "Load Configuration",      loadConfig          },  // 90 0x5A  42
        {     3,   0x3F, "Page Advance",            pageAdvance         },  // 91 0x5B  43
        {     0,   0x0C, "N ",                      nullptr             },  // 92 0x5C  44
        {     1,   0x0C, "N ",                      nullptr             },  // 93 0x5D  45
        {     0,   0x0C, "N ",                      nullptr             },  // 94 0x5E  46
        {     1,   0x0C, "N ",                      nullptr             },  // 95 0x5F  47
        {     0,      0, "Level Sine",              setLevel            },  // 96 0x60  48
        {     1,      0, "Level Triangle",          setLevel            },  // 97 0x61  49
        {     2,      0, "Level Ramp",              setLevel            },  // 98 0x62  50
        {     3,      0, "Level Pulse",             setLevel            },  // 99 0x63  51
        {     4,      0, "Level Noise",             setLevel            },  //100 0x64  52
        {     5,      0, "N ",                      nullptr             },  //101 0x65  53
        {     6,      0, "N ",                      nullptr             },  //102 0x66  54
        {     7,      0, "N ",                      nullptr             },  //103 0x67  55
      },
// XL_MIDI_MAP_FILTER
      { {     8,   0x3C, "Attack Freq",             setAttackTime       },  // 48 0x30  0
        {     0,   0x0C, "N ",                      nullptr             },  // 49 0x31  1
        {     0,   0x0C, "N ",                      nullptr             },  // 50 0x32  2
        {     9,   0x2E, "Attack Q",                setAttackTime       },  // 51 0x33  3
        {     0,   0x0C, "N ",                      nullptr             },  // 52 0x34  4
        {     0,   0x0C, "N ",                      nullptr             },  // 53 0x35  5
        {     0,   0x0C, "N ",                      nullptr             },  // 54 0x36  6
        {     0,   0x0C, "N ",                      nullptr             },  // 55 0x37  7
        {     8,   0x3C, "Decay Freq",              setDecayTime        },  // 56 0x38  8
        {     0,   0x0C, "N ",                      nullptr             },  // 57 0x39  9
        {     0,   0x0C, "N ",                      nullptr             },  // 58 0x3A  10
        {     9,   0x2E, "Decay Q",                 setDecayTime        },  // 59 0x3B  11
        {     0,   0x0C, "N ",                      nullptr             },  // 60 0x3C  12
        {     0,   0x0C, "N ",                      nullptr             },  // 61 0x3D  13
        {     0,   0x0C, "N ",                      nullptr             },  // 62 0x3E  14
        {     0,   0x0C, "N ",                      nullptr             },  // 63 0x3F  15
        {     8,   0x3C, "Release Freq",            setReleaseTime      },  // 64 0x40  16
        {     0,   0x0C, "N ",                      nullptr             },  // 65 0x41  17
        {     0,   0x0C, "N ",                      nullptr             },  // 66 0x42  18
        {     9,   0x2E, "Release Q",               setReleaseTime      },  // 67 0x43  19
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
        {     0,   0x3C, "Freq ctrl mode advance",  freqCtrlModeAdv     },  // 78 0x4E  30
        {     1,   0x3C, "Q ctrl mode advance",     freqCtrlModeAdv     },  // 79 0x4D  31
        {     0,   0x3C, "Select Bypass",           selectFilter        },  // 80 0x50  32
        {     1,   0x3C, "Select LP",               selectFilter        },  // 81 0x51  33
        {     2,   0x3C, "Select LBP",              selectFilter        },  // 82 0x52  34
        {     3,   0x3C, "Select UBP",              selectFilter        },  // 83 0x53  35
        {     4,   0x3C, "Select HP",               selectFilter        },  // 84 0x54  36
        {     0,   0x0C, "N ",                      nullptr             },  // 85 0x55  37
        {     0,   0x0C, "N ",                      nullptr             },  // 86 0x56  38
        {     0,   0x0C, "N ",                      nullptr             },  // 87 0x57  39  last button
        {     0,   0x3F, "Map mode select",         mappingSelect       },  // 88 0x58  40
        {     1,   0x3F, "Save Configuration",      saveConfig          },  // 89 0x59  41
        {     2,   0x3F, "Load Configuration",      loadConfig          },  // 90 0x5A  42
        {     3,   0x3F, "Page Advance",            pageAdvance         },  // 91 0x5B  43
        {     0,   0x0C, "N ",                      nullptr             },  // 92 0x5C  44
        {     1,   0x0C, "N ",                      nullptr             },  // 93 0x5D  45
        {     0,   0x0C, "N ",                      nullptr             },  // 94 0x5E  46
        {     1,   0x0C, "N ",                      nullptr             },  // 95 0x5F  47
        {     8,      0, "FLT FREQ START",          fltStart            },  // 96 0x60  48
        {     8,      0, "FLT FREQ END",            fltEnd              },  // 97 0x61  49
        {     8,      0, "FLT FREQ SUSTAIN",        fltSustain          },  // 98 0x62  50
        {     9,      0, "FLT Q START",             fltStart            },  // 99 0x63  51
        {     9,      0, "FLT Q END",               fltEnd              },  //100 0x64  52
        {     9,      0, "FLT Q SUSTAIN",           fltSustain          },  //101 0x65  53
        {     0,      0, "N ",                      nullptr             },  //102 0x66  54
        {     0,      0, "N ",                      nullptr             },  //103 0x67  55
      },
// XL_MIDI_MAP_LFO
      { {     0,   0x2C, "Hard LFO 1 course freq",  freqLFO             },  // 48 0x30  0
        {     1,   0x3C, "Hard LFO 1 fine freq",    freqLFO             },  // 49 0x31  1
        {     2,   0x1C, "Hard LFO 1 pulse width",  freqLFO             },  // 50 0x32  2
        {     0,      0, "N ",                      nullptr             },  // 51 0x33  3
        {     3,   0x0E, "Hard LFO 2 course freq",  freqLFO             },  // 52 0x34  4
        {     4,   0x0F, "Hard LFO 2 fine freq",    freqLFO             },  // 53 0x35  5
        {     5,   0x0D, "Hard LFO 2 pulse width",  freqLFO             },  // 54 0x36  6
        {     0,      0, "N ",                      nullptr             },  // 55 0x37  7
        {     0,      0, "N ",                      nullptr             },  // 56 0x38  8
        {     0,      0, "N ",                      nullptr             },  // 57 0x39  9
        {     0,      0, "N ",                      nullptr             },  // 58 0x3A  10
        {     0,      0, "N ",                      nullptr             },  // 59 0x3B  11
        {     0,      0, "N ",                      nullptr             },  // 60 0x3C  12
        {     0,      0, "N ",                      nullptr             },  // 61 0x3D  13
        {     0,      0, "N ",                      nullptr             },  // 62 0x3E  14
        {     0,      0, "N ",                      nullptr             },  // 63 0x3F  15
        {     6,   0x2C, "Soft LFO course freq",    freqLFO             },  // 64 0x40  16
        {     7,   0x3C, "Soft LFO fine freq",      freqLFO             },  // 65 0x41  17
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

//########################################################
//########################################################
G49_FADER_MIDI_MAP FaderMidiMapArray[] =
    {   {  0, "Level Sine",             faderG49     },  // 01  07  xx
        {  1, "Level Triangle",         faderG49     },  // 02  07  xx
        {  2, "Level Ramp",             faderG49     },  // 03  07  xx
        {  3, "Level Pulse",            faderG49     },  // 04  07  xx
        {  4, "Level Noise",            faderG49     },  // 05  07  xx
        {  5, "N ",                     nullptr      },  // 06  07  xx
        {  6, "Filter Frequency",       faderG49     },  // 07  07  xx
        {  7, "Filter Q",               faderG49     },  // 08  07  xx
        {  8, "Master Volume",          faderG49     },  // 09  07  xx
        {  9, "N ",                     nullptr      },  // 0A  07  xx
        { 10, "N ",                     nullptr      },  // 0B  07  xx
        { 11, "N ",                     nullptr      },  // 0C  07  xx
        { 12, "N ",                     nullptr      },  // 0D  07  xx
        { 13, "N ",                     nullptr      },  // 0E  07  xx
        { 14, "N ",                     nullptr      },  // 0F  07  xx
        { 15, "N ",                     nullptr      },  // 10  07  xx
    };

//########################################################
G49_ENCODER_MIDI_MAP KnobMidiMapArray[] =
    {   {  0, "N ",                     nullptr, 1  },  //  01  0A  xx
        {  1, "N ",                     nullptr, 1  },  //  02  0A  xx
        {  2, "N ",                     nullptr, 1  },  //  03  0A  xx
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
        {  0,   false,  "Tune -",       tuneUpDown  },  //  17  1
        {  1,   false,  "Tune +",       tuneUpDown  },  //  17  2
        { 22,   false,  "Tune/Reset",   tuneReset   },  //  17  3
        { 23,   false,  "Tune -/+",     tuneBump    },  //  17  4
        { 24,   false,  "Tuning save",  tunningSave },  //  17  5
     };















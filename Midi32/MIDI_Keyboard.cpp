//#######################################################################
// Module:     FrontEnd.cpp
// Descrption: Controller change on Graphite 49
// Creator:    markeby
// Date:       10/9/2024
//#######################################################################
#include <Arduino.h>

#include "Config.h"
#include "FrontEnd.h"
#include "I2Cmessages.h"
#include "SerialMonitor.h"
#include "Debug.h"
#include "MidiConf.h"

#ifdef DEBUG_SYNTH
static const char* Label  = "TOP";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#endif

using namespace MIDI_NAMESPACE;
using namespace DISP_MESSAGE_N;

//-------------------------------------------------------------------
static void Cb_ControlChange_Keyboard (uint8_t mchan, uint8_t type, uint8_t value)
    {
    DBG ("Control change Keyboard: 0x%2.2X  value 0x%2.2X", type, value);
    ControlChangeKeyboard (mchan, type, value);
    }

//-------------------------------------------------------------------
static void Cb_KeyDown_Keyboard (uint8_t mchan, uint8_t key, uint8_t velocity)
    {
    if ( KeyboardKapture () )
        {
        if ( CurrentMidi() > 0 )
            mchan = CurrentMidi ();
        }
    DBG ("Key down: %d  velocity: %d", key, velocity);
    KeyDown(mchan, key, velocity);
    }

//-------------------------------------------------------------------
static void  Cb_KeyUp_Keyboard (uint8_t mchan, uint8_t key, uint8_t velocity)
    {
    if ( KeyboardKapture () )
        {
        if ( CurrentMidi() > 0 )
            mchan = CurrentMidi ();
        }
    DBG ("Key up: %d  velocity: %d", key, velocity);
    KeyUp (mchan, key, velocity);
    }

//-------------------------------------------------------------------
static void Cb_PitchBend_Keyboard (uint8_t mchan, int value)
    {
    DBG ("Keyboard pitch bend %d", value);
    value = (value + 8192) >> 2;
    if ( KeyboardKapture () )
        {
        if ( CurrentMidi() > 0 )
            mchan = CurrentMidi ();
        }
    PitchBender (mchan, value);
    }

//-------------------------------------------------------------------
static void Cb_Message_Keyboard (const MidiMessage& msg)
    {
    printf ("*** Keyboard MESSAGE:    type = 0x%02X   channel = %2d   data1 = 0x%02X   data2 = 0x%02X   length = 0x%02X\n",
            msg.type, msg.channel, msg.data1, msg.data2, msg.length);
    }

//-------------------------------------------------------------------
static void Cb_SystemEx_Keyboard (byte * array, unsigned size)
    {
    printf ("\n\n*** Keyboard SYSEX");
    SystemExDebug (array, size);
    }

//-------------------------------------------------------------------
static void Cb_SystemReset_Keyboard (void)
    {
    printf ("\n\n*** Keyboard RESET\n");
    }

//-------------------------------------------------------------------
static void Cb_Error_Keyboard (int8_t err)
    {
    printf ("\n\n*** Keyboard ERROR %d\n", err);
    }

//########################################################
//########################################################
static void tuneReset (int index, bool state)
    {
    if ( state )
        StartCalibration ();
    else
        Monitor.Reset ();
    }
//########################################################
static void tuneUpDown (int index, bool state)
    {
    TuningAdjust (index);
    }

//########################################################
static void tuneBump (int index, bool state)
    {
    if ( IsInTuning () )
        TuningBump (state);
    else
        KeyboardKapture (state);
    }

//########################################################
static void tunningSave (int index, bool state)
    {
    SaveTuning ();
    }

//########################################################
static void faderG49 (int index, short data)
    {
    if ( index == 8 )
        MasterVolume (data);
    else if ( IsInTuning () )
        {
        switch ( index )
            {
            case 0 ... 4:
                SetTuningLevel (index, data);
                break;
            case 6 ... 7:
                SetTuningFilter (index - 6, data);
                break;
            default:
                break;
            }
        }
    }
//########################################################
//########################################################
//    Midi control mapping
typedef struct
    {
    short       Index;
    const char* Desc;
    void        (*CallBack)(int index, short data);
    int         Value;
    }  G49_VARIABLE_MAP;

typedef struct
    {
    short       Index;
    bool        State;
    const char* Desc;
    void       (*CallBack)(int index, bool state);
    }  G49_BUTTON_MAP;

//########################################################
//########################################################
G49_VARIABLE_MAP faderMap[] =
    {   {  0, "Level Sine",             faderG49, 0 },  // 01  07  xx
        {  1, "Level Triangle",         faderG49, 0 },  // 02  07  xx
        {  2, "Level Ramp",             faderG49, 0 },  // 03  07  xx
        {  3, "Level Pulse",            faderG49, 0 },  // 04  07  xx
        {  4, "Level Noise",            faderG49, 0 },  // 05  07  xx
        {  5, "N ",                     nullptr,  0 },  // 06  07  xx
        {  6, "Filter Frequency",       faderG49, 0 },  // 07  07  xx
        {  7, "Filter Q",               faderG49, 0 },  // 08  07  xx
        {  8, "Master Volume",          faderG49, 0 },  // 09  07  xx
    };

//########################################################
G49_VARIABLE_MAP encoderMap[] =
    {   {  0, "N ",                     nullptr, 1  },  //  01  0A  xx
        {  1, "N ",                     nullptr, 1  },  //  02  0A  xx
        {  2, "N ",                     nullptr, 1  },  //  03  0A  xx
        {  3, "N ",                     nullptr, 1  },  //  04  0A  xx
        {  4, "N ",                     nullptr, 1  },  //  05  0A  xx
        {  5, "N ",                     nullptr, 1  },  //  06  0A  xx
        {  6, "N ",                     nullptr, 1  },  //  07  0A  xx
        {  7, "N ",                     nullptr, 1  },  //  08  0A  xx
    };

//########################################################
G49_BUTTON_MAP switchMap[] =
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


//#######################################################################
//#######################################################################
void InitMidiKeyboard ()
    {
    Midi_1.setHandleNoteOn               (Cb_KeyDown_Keyboard);
    Midi_1.setHandleNoteOff              (Cb_KeyUp_Keyboard);
    Midi_1.setHandleControlChange        (Cb_ControlChange_Keyboard);
    Midi_1.setHandlePitchBend            (Cb_PitchBend_Keyboard);
    Midi_1.setHandleError                (Cb_Error_Keyboard);
//    Midi_1.setHandleAfterTouchPoly       (AfterTouchPolyCallback fptr);
//    Midi_1.setHandleProgramChange        (ProgramChangeCallback fptr);
//    Midi_1.setHandleAfterTouchChannel    (AfterTouchChannelCallback fptr);
//    Midi_1.setHandleTimeCodeQuarterFrame (TimeCodeQuarterFrameCallback fptr);
//    Midi_1.setHandleSongPosition         (SongPositionCallback fptr);
//    Midi_1.setHandleSongSelect           (SongSelectCallback fptr);
//    Midi_1.setHandleTuneRequest          (TuneRequestCallback fptr);
//    Midi_1.setHandleClock                (ClockCallback fptr);
//    Midi_1.setHandleStart                (StartCallback fptr);
//    Midi_1.setHandleTick                 (TickCallback fptr);
//    Midi_1.setHandleContinue             (ContinueCallback fptr);
//    Midi_1.setHandleStop                 (StopCallback fptr);
//    Midi_1.setHandleActiveSensing        (ActiveSensingCallback fptr);
    Midi_1.setHandleSystemExclusive      (Cb_SystemEx_Keyboard);
    Midi_1.setHandleSystemReset          (Cb_SystemReset_Keyboard);
#ifdef DEBUG_MIDI_MSG           // Enable all messages to print on debug terminal
    Midi_1.setHandleMessage              (Cb_Message_Keyboard);
#endif
    }

//#######################################################################
void ControlChangeKeyboard (short mchan, byte type, byte value)
    {
    type -= 16;                 // all controls start at 16
    switch ( mchan )
        {
        case 1:
            switch ( type )
                {
                case 1:
                    // mod wheel
                    SetLevelLFO (0, mchan, value);
                    SetLevelLFO (1, mchan, value);
                    SoftLFO.Multiplier (mchan, (float)value * PRS_SCALER * 0.5);
                    DBG ("MIDI channel = %d   modulation = %d    ", mchan, value);
                    break;

                case 0x40:      // Damper pedal (sustain)
                    {
                    bool zb;

                    if ( value < 64 )       zb = false;
                    else                    zb = true;
                    DBG ("Damper pdeal: %s", (( zb ) ? "ON" : "OFF"));
                    }
                    break;

                default:
                    break;
                }
            break;

        case 2:
            {
            G49_VARIABLE_MAP& m = faderMap[type];
            DBG ("%s > %d", m.Desc, value);
            if ( m.CallBack != nullptr )
                m.CallBack (m.Index, value);
            }
            break;

        case 3:
            {
            G49_VARIABLE_MAP& m = encoderMap[type];
            int z = m.Value + (value & 0x1F) * ((value & 0x40) ? -1 : 1);
            if ( z > 4095 )   z = 4095;
            if ( z < 1 )      z = 1;
            DBG ("%s > %d (%d)", m.Desc, z, value);
            if ( m.CallBack != nullptr )
                m.CallBack (m.Index, z);
            }
            break;

        case 4:
            {
            if ( SetTuning )
                {
                switch ( type )
                    {
                    case 0 ... 7:
                        if ( (RunTime - TuningSelectionTime) < 5000 )
                            {
                            pVoice[type]->TuningState (true);
                            DisplayMessage.TuningSelectSecond (type);
                            TuningSelectionTime = 0;
                            }
                        else
                            {
                            for ( int z = 0;  z < VOICE_COUNT;  z++ )
                                pVoice[z]->TuningState (false);
                            pVoice[type]->TuningState (true);
                            DisplayMessage.TuningSelect (type);
                            TuningSelectionTime = RunTime;
                            }
                        break;

                    case 8 ... 12:
                        TuningOutputBitFlip (type - 8);
                        break;

                    default:
                        break;
                    }
                }
            else
                {
                G49_BUTTON_MAP& m = switchMap[type];
                m.State = !m.State;
                DBG ("%s %d", m.Desc, m.State);
                if ( m.CallBack != nullptr )
                    m.CallBack (m.Index, m.State);
                }
            }
            break;

        case 5:
            {
            type += 16;
            G49_BUTTON_MAP& m = switchMap[type];
            m.State = !m.State;
            DBG ("%s %d", m.Desc, m.State);
            if ( m.CallBack != nullptr )
                m.CallBack (m.Index, m.State);
            }
            break;
        default:
            break;
        }
    }



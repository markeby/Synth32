//#######################################################################
// Module:     FrontEndSeq.cpp
// Descrption: Synthesizer sequencing input
// Creator:    markeby
// Date:       10/11/2025
//#######################################################################
#include <Arduino.h>

#include "Config.h"
#include "FrontEnd.h"
#include "Debug.h"
#include "MidiConf.h"

#ifdef DEBUG_SYNTH
static const char* Label  = "SEQ";
#define DBG(args...) {if(DebugSeq){DebugMsg(Label,mchan,args);}}
#else
#define DBG(args...)
#endif

//###################################################################
using namespace MIDI_NAMESPACE;

//-------------------------------------------------------------------
static void Cb_ControlChange_Seq (uint8_t mchan, uint8_t type, uint8_t value)
    {
    DBG ("Control change: 0x%2.2X  value 0x%2.2X", type, value);
    ControllerSequence (mchan, type, value);
    }

//-------------------------------------------------------------------
static void Cb_KeyDown_Seq (uint8_t mchan, uint8_t key, uint8_t velocity)
    {
    DBG ("Key down: %d  velocity: %d", key, velocity);
    KeyDown (mchan, key, velocity);
    }

//-------------------------------------------------------------------
static void Cb_KeyUp_Seq (uint8_t mchan, uint8_t key, uint8_t velocity)
    {
    DBG ("Key up: %d  velocity: %d", key, velocity);
    KeyUp (mchan, key, velocity);
    }

//-------------------------------------------------------------------
static void Cb_PitchBend_Seq (uint8_t mchan, int value)
    {
    DBG ("Sequencer pitch bend value %d", value);
    value = (value + 8192) >> 2;
    PitchBender (mchan, value);
    }

//-------------------------------------------------------------------
static void Cb_Message_Seq (const MidiMessage& msg)
    {
    printf ("*** Sequencer MESSAGE:    type = 0x%02X   channel = %2d   data1 = 0x%02X   data2 = 0x%02X   length = 0x%02X\n",
            msg.type, msg.channel, msg.data1, msg.data2, msg.length);
    }

//-------------------------------------------------------------------
static void Cb_SystemEx_Seq (byte * array, unsigned size)
    {
    printf ("\n\n*** Sequencer SYSEX");
    SystemExDebug (array, size);
    }

//-------------------------------------------------------------------
static void Cb_SystemReset_Seq (void)
    {
    printf ("\n\n*** Sequencer RESET\n");
    }

//-------------------------------------------------------------------
static void Cb_Error_Seq (int8_t err)
    {
    printf ("\n\n*** Sequencer ERROR %d\n", err);
    }

//#######################################################################
//#######################################################################
void InitMidiSequence ()
    {
    Midi_2.setHandleNoteOn               (Cb_KeyDown_Seq);
    Midi_2.setHandleNoteOff              (Cb_KeyUp_Seq);
    Midi_2.setHandleControlChange        (Cb_ControlChange_Seq);
    Midi_2.setHandlePitchBend            (Cb_PitchBend_Seq);
    Midi_2.setHandleError                (Cb_Error_Seq);
//    Midi_2.setHandleAfterTouchPoly       (AfterTouchPolyCallback fptr);
//    Midi_2.setHandleProgramChange        (ProgramChangeCallback fptr);
//    Midi_2.setHandleAfterTouchChannel    (AfterTouchChannelCallback fptr);
//    Midi_2.setHandleTimeCodeQuarterFrame (TimeCodeQuarterFrameCallback fptr);
//    Midi_2.setHandleSongPosition         (SongPositionCallback fptr);
//    Midi_2.setHandleSongSelect           (SongSelectCallback fptr);
//    Midi_2.setHandleTuneRequest          (TuneRequestCallback fptr);
//    Midi_2.setHandleClock                (ClockCallback fptr);
//    Midi_2.setHandleStart                (StartCallback fptr);
//    Midi_2.setHandleTick                 (TickCallback fptr);
//    Midi_2.setHandleContinue             (ContinueCallback fptr);
//    Midi_2.setHandleStop                 (StopCallback fptr);
//    Midi_2.setHandleActiveSensing        (ActiveSensingCallback fptr);
    Midi_2.setHandleSystemExclusive      (Cb_SystemEx_Seq);
    Midi_2.setHandleSystemReset          (Cb_SystemReset_Seq);
#ifdef DEBUG_MIDI_MSG           // Enable all messages to print on debug terminal
    Midi_2.setHandleMessage              (Cb_Message_Seq);
#endif
    }

//#######################################################################
void ControllerSequence (short mchan, byte type, byte value)
    {
    int  z;
    bool zl;

    switch ( type )
        {
        case 0x78:      // [Channel Mode Message] All Sound Off
            DBG ("[Channel Mode Message] All Sound Off");
            ClearSynth ();
            break;

        case 0x40:      // Damper pedal (sustain)
            if ( value < 64 )       zl = false;
            else                    zl = true;
            DBG ("Damper pdeal: %s", (( zl ) ? "ON" : "OFF"));
            break;

        case 0x0B:
            DBG ("Expression Controller: %d", value);
            break;

        case 0x5D:
            DBG ("Effects 3 Depth: %d", value);
            break;
        case 0x5B:
            DBG ("Effects 1 Depth: %d", value);
            break;

        case 0x07:      // Channel volume
            DBG ("Channel volume: %d", value);
            break;

        case 0x01:      // Modulation wheel
            DBG ("Modulation whee:l %d", value);
            break;
        case 0x21:      // LSB Modulation wheel
            DBG ("Modulation wheel: LSB %d", value);
            break;

        case 0x00:
            DBG ("Bank select: %d", value);
            break;
        case 0x20:
            DBG ("Bank select LSB: %d", value);
            break;

        case 0x64:
            DBG ("RPN LSB: %d", value);
            break;
        case 0x65:
            DBG ("RPN MSB: %d", value);
            break;
        case 0x06:
            DBG ("Data Entry MSB: %d", value);
            break;

        default:
            DBG ("Uknown controller command: %d   value: %d", type, value);
            break;
        }

    }


//#######################################################################
// Module:     FrontEnd.cpp
// Descrption: Synthesizer front end controller
// Creator:    markeby
// Date:       12/9/2024
//#######################################################################
#include <UHS2-MIDI.h>
#include <MIDI.h>

#include "../Common/SynthCommon.h"
#include "../Common/DispMessages.h"
#include "I2Cmessages.h"
#include "Osc.h"
#include "Settings.h"
#include "FrontEnd.h"
#include "Debug.h"

#ifdef DEBUG_SYNTH
static const char* Label  = "TOP";
static const char* LabelM = "M";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#define DBGM(args...) {if(DebugMidi){DebugMsg(LabelM,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#define DBGM(args...)
#endif

//###################################################################
static      USB Usb;
static      UHS2MIDI_CREATE_INSTANCE(&Usb, MIDI_PORT, Midi_0);
static      MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, Midi_1);
static      MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, Midi_2);

#define SENDnote0(k,c) {Midi_0.sendNoteOn(k,c,1);DBGM("Note 0x%X  Color = 0x%X  [%s]", k, c,__PRETTY_FUNCTION__);}
#define SENDcc0(k,c) {Midi_0.send(midi::MidiType::ControlChange,k,c,1);DBGM("code 0x%X Color = 0x%X  [%s]", k, c, __PRETTY_FUNCTION__);}
#define SENDcc1(k,c) {Midi_1.send(midi::MidiType::ControlChange,k,c,1);DBGM("CC 0x%X Value = 0x%X  [%s]", k, c, __PRETTY_FUNCTION__);}

using namespace MIDI_NAMESPACE;
typedef Message<MIDI_NAMESPACE::DefaultSettings::SysExMaxSize> MidiMessage;

//###################################################################
//void FuncMessage (const MidiInterface::MidiMessage& msg)
void FuncMessage (const MidiMessage& msg)
    {
    DBGM ("valid = %X   type = %X   channel = %X   data1 = %X   data2 = %X   length = %X",
          msg.valid, msg.type, msg.channel, msg.data1, msg.data2, msg.length);
    }

//###################################################################
static void  FuncKeyDown (uint8_t mchan, uint8_t key, uint8_t velocity)
    {
    DBGM ("Key down  channel: %d  key: %d  velocity: %d", mchan, key, velocity);
    SynthFront.KeyDown (mchan, key, velocity);
    }

//###################################################################
static void  FuncKeyUp (uint8_t mchan, uint8_t key, uint8_t velocity)
    {
    DBGM ("Key up  channel: %d  key: %d  velocity: %d", mchan, key, velocity);
    SynthFront.KeyUp (mchan, key, velocity);
    }

//###################################################################
static void FuncController (uint8_t mchan, uint8_t type, uint8_t value)
    {
    SynthFront.Controller (mchan, type, value);
    DBGM ("Controller  Chan %2.2X  type %2.2X  value %2.2X", mchan, type, value);
    }

//###################################################################
static void FuncPitchBend (uint8_t mchan, int value)
    {
    DBGM ("Pitch Bend  Chan %2.2X  value %d", mchan, value);
    value = (value + 8192) >> 2;
    SynthFront.PitchBend (mchan, value);
    }

//#######################################################################
//#######################################################################
void SYNTH_FRONT_C::ResetXL ()
    {
    static const byte midi_msg[] = { 0xF0, 0x00, 0x20, 0x29, 0x02, 0x11, 0x7B, 0x00, 0x00 ,0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0xF7 };
    delay (250);
    SENDcc0 (0, 0);
    delay (100);
    for ( int z = 0;  z < SIZE_CL_MAP;  z++ )
        {
        if ( XlMap[z].Color != 0 )
            {
            if ( z < SIZE_S_LED )
                {
                SENDnote0 (XlMap[z].Index, XlMap[z].Color);
                }
            else
                {
                SENDcc0 (XlMap[z].Index, XlMap[z].Color);
                }
            delay (20);
            }
        }
    Midi_0.sendSysEx (sizeof (midi_msg), midi_msg, true);
    Midi_0.sendNoteOn (PanDevice[0], 0, 1);
    Midi_0.sendNoteOn (PanDevice[1], 0, 1);
    Midi_0.sendNoteOn (PanDevice[2], 0, 1);
    }

//#######################################################################
void SYNTH_FRONT_C::ResetUSB ()
    {
    while ( Usb.Init () == -1 )
        {
        delay (200);
        printf ("Usb midi init retry!\n");
        }
    this->ResolveMapAllocation ();
    }

//#######################################################################
static const char* SelLabel[] = { "Sine",  "Triangle", "Ramp", "Pulse", "Square" };
String SYNTH_FRONT_C::Selected ()
    {
    String str = "";

    for ( int z = 0;  z < ENVELOPE_COUNT;  z++ )
        {
        if ( this->SynthConfig.Voice[this->CurrentMapSelected].SelectedEnvelope[z] )
            {
            str += SelLabel[z];
            str += "  ";
            }
        }
    return str;
    }

//#####################################################################
SYNTH_FRONT_C::SYNTH_FRONT_C (MIDI_MAP* fader_map, MIDI_ENCODER_MAP* knob_map, MIDI_BUTTON_MAP* button_map, MIDI_XL_MAP* xl_map)
    {
    this->FaderMap            = fader_map;
    this->KnobMap             = knob_map;
    this->ButtonMap           = button_map;
    this->XlMap               = xl_map;
    this->Down.Key            = 0;
    this->Down.Trigger        = 0;
    this->Down.Velocity       = 0;
    this->Up.Key              = 0;
    this->Up.Trigger          = 0;
    this->Up.Velocity         = 0;
    this->SetTuning           = false;
    this->TuningChange        = false;
    this->CurrentMapSelected  = 0;
    this->CurrentMidiSelected = 1;
    this->CalibrationPhase    = 0;
    this->LoadSaveSelection   = 1;
    }

//#######################################################################
void SYNTH_FRONT_C::Begin (short osc_d_a, short mux_digital, short noise_digital, short lfo_analog, short lfo_digital, short mod_mux_digital, short start_a_d)
    {
//  Midi_0.setHandleMessage              (FuncMessage);
    Midi_0.setHandleNoteOn               (FuncKeyDown);
    Midi_0.setHandleNoteOff              (FuncKeyUp);
    Midi_0.setHandleControlChange        (FuncController);
    Midi_0.setHandlePitchBend            (FuncPitchBend);
//  Midi_0.setHandleError                (ErrorCallback fptr);
//  Midi_0.setHandleAfterTouchPoly       (AfterTouchPolyCallback fptr);
//  Midi_0.setHandleProgramChange        (ProgramChangeCallback fptr);
//  Midi_0.setHandleAfterTouchChannel    (AfterTouchChannelCallback fptr);
//  Midi_0.setHandleSystemExclusive      (SystemExclusiveCallback fptr);
//  Midi_0.setHandleTimeCodeQuarterFrame (TimeCodeQuarterFrameCallback fptr);
//  Midi_0.setHandleSongPosition         (SongPositionCallback fptr);
//  Midi_0.setHandleSongSelect           (SongSelectCallback fptr);
//  Midi_0.setHandleTuneRequest          (TuneRequestCallback fptr);
//  Midi_0.setHandleClock                (ClockCallback fptr);
//  Midi_0.setHandleStart                (StartCallback fptr);
//  Midi_0.setHandleTick                 (TickCallback fptr);
//  Midi_0.setHandleContinue             (ContinueCallback fptr);
//  Midi_0.setHandleStop                 (StopCallback fptr);
//  Midi_0.setHandleActiveSensing        (ActiveSensingCallback fptr);
//  Midi_0.setHandleSystemReset          (SystemResetCallback fptr);

//  Midi_1.setHandleMessage              (FuncMessage);
    Midi_1.setHandleNoteOn               (FuncKeyDown);
    Midi_1.setHandleNoteOff              (FuncKeyUp);
    Midi_1.setHandleControlChange        (FuncController);
    Midi_1.setHandlePitchBend            (FuncPitchBend);
//  Midi_1.setHandleError                (ErrorCallback fptr);
//  Midi_1.setHandleAfterTouchPoly       (AfterTouchPolyCallback fptr);
//  Midi_1.setHandleProgramChange        (ProgramChangeCallback fptr);
//  Midi_1.setHandleAfterTouchChannel    (AfterTouchChannelCallback fptr);
//  Midi_1.setHandleSystemExclusive      (SystemExclusiveCallback fptr);
//  Midi_1.setHandleTimeCodeQuarterFrame (TimeCodeQuarterFrameCallback fptr);
//  Midi_1.setHandleSongPosition         (SongPositionCallback fptr);
//  Midi_1.setHandleSongSelect           (SongSelectCallback fptr);
//  Midi_1.setHandleTuneRequest          (TuneRequestCallback fptr);
//  Midi_1.setHandleClock                (ClockCallback fptr);
//  Midi_1.setHandleStart                (StartCallback fptr);
//  Midi_1.setHandleTick                 (TickCallback fptr);
//  Midi_1.setHandleContinue             (ContinueCallback fptr);
//  Midi_1.setHandleStop                 (StopCallback fptr);
//  Midi_1.setHandleActiveSensing        (ActiveSensingCallback fptr);
//  Midi_1.setHandleSystemReset          (SystemResetCallback fptr);

//  Midi_2.setHandleMessage              (FuncMessage);
    Midi_2.setHandleNoteOn               (FuncKeyDown);
    Midi_2.setHandleNoteOff              (FuncKeyUp);
//  Midi_2.setHandleControlChange        (FuncController);
//  Midi_2.setHandlePitchBend            (FuncPitchBend);
//  Midi_2.setHandleError                (ErrorCallback fptr);
//  Midi_2.setHandleAfterTouchPoly       (AfterTouchPolyCallback fptr);
//  Midi_2.setHandleProgramChange        (ProgramChangeCallback fptr);
//  Midi_2.setHandleAfterTouchChannel    (AfterTouchChannelCallback fptr);
//  Midi_2.setHandleSystemExclusive      (SystemExclusiveCallback fptr);
//  Midi_2.setHandleTimeCodeQuarterFrame (TimeCodeQuarterFrameCallback fptr);
//  Midi_2.setHandleSongPosition         (SongPositionCallback fptr);
//  Midi_2.setHandleSongSelect           (SongSelectCallback fptr);
//  Midi_2.setHandleTuneRequest          (TuneRequestCallback fptr);
//  Midi_2.setHandleClock                (ClockCallback fptr);
//  Midi_2.setHandleStart                (StartCallback fptr);
//  Midi_2.setHandleTick                 (TickCallback fptr);
//  Midi_2.setHandleContinue             (ContinueCallback fptr);
//  Midi_2.setHandleStop                 (StopCallback fptr);
//  Midi_2.setHandleActiveSensing        (ActiveSensingCallback fptr);
//  Midi_2.setHandleSystemReset          (SystemResetCallback fptr);

    printf ("\t>>> Midi interfaces startup\n");
    while ( Usb.Init () == -1 )
        {
        delay (200);
        printf ("Usb midi init retry!\n");
        }
    printf ("\t>>> Usb midi ready\n");

    Serial1.begin (31250, SERIAL_8N1, RXD1, TXD1, false);
    Midi_1.begin (MIDI_CHANNEL_OMNI);
    printf ("\t>>> Serial1 midi ready\n");

    Serial2.begin (31250, SERIAL_8N1, RXD2, TXD2, false);
    Midi_2.begin (MIDI_CHANNEL_OMNI);
    printf ("\t>>> Serial2 midi ready\n");

    // Setup ports for calibration
    CalibrationBaseDigital = mod_mux_digital;

    printf ("\t>>> Starting synth channels\n");
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        this->pVoice[z] = new VOICE_C(z, osc_d_a, mux_digital, mod_mux_digital, noise_digital, EnvADSL);
        osc_d_a     += 8;
        mux_digital += 1;
        if ( z & 1 )
            {
            mod_mux_digital += 1;
            noise_digital   += 4;
            }
        }

    this->Lfo[0].Begin (0, lfo_analog, lfo_digital);
    lfo_analog  += 6;
    lfo_digital += 3;
    this->Lfo[1].Begin (1, lfo_analog, lfo_digital);
    this->CalibrationAtoD = start_a_d;
    this->ResolutionMode = true;
    this->SynthConfig.Load (0);         // load the default configuration
    }

//#######################################################################
void SYNTH_FRONT_C::Clear ()
    {
    for ( int z = 0;  z < VOICE_COUNT; z++ )
        this->pVoice[z]->Clear ();
    }

//#######################################################################
void SYNTH_FRONT_C::PageAdvance ()
    {
    byte  m    = this->CurrentMidiSelected;
    short next = this->CurrentMapSelected + 1;

    if ( this->CurrentMapSelected < MAP_COUNT )
        {
        this->ResetVoiceComponentSetSelected ();
        this->ResetXL ();
        }
    while ( next < MAP_COUNT )
        {
        if ( m == this->SynthConfig.Voice[next].GetVoiceMidi () )
            {
            next++;
            continue;
            }
        this->CurrentMidiSelected  = this->SynthConfig.Voice[next].GetVoiceMidi ();
        this->CurrentMapSelected   = next;
        this->CurrentVoiceSelected = next >> 1;
        DisplayMessage.SelectVoicePage (next);
        return;
        }
    if ( next < (short)DISP_MESSAGE_N::PAGE_C::PAGE_MIDI_MAP )
        {
        this->CurrentMapSelected   = -1;
        DisplayMessage.Page (next);
        this->CurrentMidiSelected  = 0;
        this->CurrentVoiceSelected = -1;
        }
    else
        {
        this->CurrentMapSelected   = 0;
        this->CurrentVoiceSelected = 0;
        this->CurrentMidiSelected  = this->SynthConfig.Voice[this->CurrentMapSelected].GetVoiceMidi ();
        DisplayMessage.SelectVoicePage (this->CurrentMapSelected);
        }
    }

//#######################################################################
void SYNTH_FRONT_C::Controller (short mchan, byte type, byte value)
    {
    int   z;
    short chan = mchan - 1;;

    switch ( type )
        {
        case 0x01:
            // mod wheel
            this->SetLevelLFO (0, mchan, value);
            this->SetLevelLFO (1, mchan, value);
            SoftLFO.Multiplier (mchan, (float)value * PRS_SCALER * 0.5);
            DBG ("MIDI channel = %d   modulation = %d    ", mchan, value);
            break;
        case 0x07:          // Faders controls
            {
            MIDI_MAP& m = this->FaderMap[chan];
            if ( m.CallBack != nullptr )
                {
                DBG ("%s > %d    ", m.Desc, value);
                m.CallBack (m.Index, value);
                }
            }
            break;
        case 0x0a:          // Rotatational encoder controls
            {
            MIDI_ENCODER_MAP& m = this->KnobMap[chan];
            if ( m.CallBack != nullptr )
                {
                z = m.Value + (value & 0x1F) * ((value & 0x40) ? -1 : 1);
                if ( z > 4095 )   z = 4095;
                if ( z < 1 )      z = 1;
                DBG("%s %s > %d (%d)", Selected().c_str(), m.Desc, z, value);
                KnobMap[chan].CallBack(m.Index, z);
                }
            }
            break;
        case 0x10 ... 0x1F:
            {
            chan = type & 0x0F;
            if ( this->SetTuning && (chan < 8) )
                {
                bool zb = !this->TuningOn[chan];
                this->TuningOn[chan] = zb;
                DBG ("Tuning channel  %d %s", chan, (( zb ) ? "ON" : "Off"));
                DisplayMessage.TuningSelect (chan, zb);
                this->TuningChange = true;
                return;
                }
            MIDI_BUTTON_MAP& m = this->ButtonMap[chan];
            if ( m.CallBack != nullptr )
                {
                m.State = !m.State;
                DBG ("%s %d", m.Desc, m.State);
                m.CallBack (m.Index, m.State);
                }
            }
            break;
        case 0x72:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
            {
            chan = type - (0x72 - 16);
            MIDI_BUTTON_MAP& m = this->ButtonMap[chan];
            if ( m.CallBack != nullptr )
                {
                m.State = !m.State;
                DBG ("%s %d", m.Desc, m.State);
                m.CallBack (m.Index, m.State);
                }
            }
            break;
        case 0x30 ... 0x4F:
            {
            chan = type - 0x30;
            MIDI_XL_MAP& m = this->XlMap[chan];
            if ( m.CallBack != nullptr )
                {
                DBG ("%s %s > %d    ", Selected ().c_str (), m.Desc, value);
                m.CallBack (m.Index, value);
                }
            }
            break;
        case 0x50 ... 0x67:
            {
            chan           = type - 0x30;       // offset to start of control map
            bool tgl       = value > 0x1F;      // use color white to show selectable and green to show selected.
            MIDI_XL_MAP& m = this->XlMap[chan]; // current control map entry

            z = m.Index - 0x50;             // buttons need index tweaked
            if ( z >= 0x10 )                // if buttons on side of unit
                z -= 0x10;                  // then recalculate offset index
            if ( m.CallBack != nullptr )
                {
                DBG ("%s %s", m.Desc, (( tgl ) ? "ON" : "Off"));
                m.CallBack (z, (short)tgl);
                }
            }
            break;

        case 120 ... 127:           // all notes stop
            this->Clear ();
            break;

        default:
            break;
        }
    }

//#######################################################################
void SYNTH_FRONT_C::Loop ()
    {
    int oldest = -1;
    int doit   = -1;

    this->Lfo[0].Loop ();
    this->Lfo[1].Loop ();

    Usb.Task    ();
    Midi_0.read ();
    Midi_1.read ();
    Midi_2.read ();

    if ( !SetTuning )
        {
        if ( this->Up.Trigger )
            {
            DBG ("Key up > %d", this->Up.Key);
            for ( int z = 0;  z < VOICE_COUNT;  z++ )
                this->pVoice[z]->NoteClear (this->Up.Trigger, this->Up.Key);
            this->Up.Trigger = 0;
            }

        if ( this->Down.Trigger )                          // Key went down so look for a channel to use
            {
            for ( int z = 0;  z <  VOICE_COUNT;  z++ )
                {
                VOICE_C& ch = *(pVoice[z]);
                if ( this->Down.Trigger == ch.GetMidi () )
                    {
                    if ( !ch.IsActive () )              // grab the first channel not in use
                        {
                        doit = z;
                        break;
                        }
                    else
                        {
                        if ( oldest < 0 )               // channel is inu use so this is the first one to check for oldest
                            oldest = z;
                        else
                            {
                            if ( ch.IsActive () > pVoice[oldest]->IsActive () )      // check if current channel older than the oldest so far
                                oldest = z;
                            }
                        }
                    }
                }
            if ( doit < 0 )                                 // no unused channel so we will capture the one used the longest
                doit = oldest;
            if ( doit >= 0 )                                // only process valid channels
                {
                this->Lfo[0].HardReset (this->Down.Trigger);
                this->Lfo[1].HardReset (this->Down.Trigger);
                this->pVoice[doit]->NoteSet(this->Down.Trigger, this->Down.Key, this->Down.Velocity);   // set the channel
                }
            DBG ("Key down > %d   Velocity > %d  Port > %d", this->Down.Key, this->Down.Velocity, doit);
            this->Down.Trigger = 0;                         // release the trigger
            }

        EnvADSL.Loop ();                                    // process all envelope generators
        for ( int z = 0;  z < VOICE_COUNT;  z++ )           // Check all channels for done
            this->pVoice[z]->Loop ();
        I2cDevices.UpdateDigital ();
        I2cDevices.UpdateAnalog  ();     // Update D/A ports
        }
    else
        this->Tuning ();
    }

//#####################################################################
void SYNTH_FRONT_C::ShowVoiceXL (int val)
    {
    Midi_0.sendNoteOn (PanDevice[0], val, 1);
    delay (20);
    Midi_0.sendNoteOn (PanDevice[1], val, 1);
    delay (20);
    Midi_0.sendNoteOn (PanDevice[2], val, 1);
    }

//#####################################################################
void SYNTH_FRONT_C::NonOscPageSelect (DISP_MESSAGE_N::PAGE_C page)
    {
    DisplayMessage.Page (page);
    this->CurrentMidiSelected   =  0;
    this->CurrentMapSelected    = -1;
    this->CurrentVoiceSelected  = -1;
    }




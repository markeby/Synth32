//#######################################################################
// Module:     SyntFront.ino
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
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

#ifdef DEBUG_ON
static const char* Label  = "TOP";
static const char* LabelM = "M";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#define DBGM(args...) {if(DebugMidi){DebugMsg(LabelM,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#define DBGM(args...)
#endif

//###################################################################
//#define TOGGLE          // use if channel select switches are to be alternate action

static      USB Usb;
static      UHS2MIDI_CREATE_INSTANCE(&Usb, MIDI_PORT, Midi_0);
static      MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, Midi_1);
//static      MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, Midi_2);

#define SENDnote(k,c) {Midi_0.sendNoteOn(k,c,1);DBGM("Note 0x%X  Color = 0x%X  [%s]", k, c,__PRETTY_FUNCTION__);}
#define SENDcc(k,c) {Midi_0.send(midi::MidiType::ControlChange,k,c,1);DBGM("code 0x%X Color = 0x%X  [%s]", k, c, __PRETTY_FUNCTION__);}

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
static void  FuncKeyDown (uint8_t chan, uint8_t key, uint8_t velocity)
    {
    DBGM ("Key down  channel: %d  key: %d  velocity: %d", chan, key, velocity);
    SynthFront.KeyDown (chan, key, velocity);
    }

//###################################################################
static void  FuncKeyUp (uint8_t chan, uint8_t key, uint8_t velocity)
    {
    DBGM ("Key up  channel: %d  key: %d  velocity: %d", chan, key, velocity);
    SynthFront.KeyUp (chan, key, velocity);
    }

//###################################################################
static void FuncController (uint8_t chan, uint8_t type, uint8_t value)
    {
    SynthFront.Controller (chan, type, value);
    DBGM ("Controller  Chan %2.2X  type %2.2X  value %2.2X", chan, type, value);
    }

//###################################################################
static void FuncPitchBend (uint8_t chan, int value)
    {
    SynthFront.PitchBend(chan, value);
    DBGM ("Pitch Bend  Chan %2.2X  value %d", chan, value);
    }

//#######################################################################
//#######################################################################
void SYNTH_FRONT_C::ResetXL ()
    {
    SENDcc (0, 0);
    delay (50);
    for ( int z = 0;  z < SIZE_CL_MAP;  z++ )
        {
        if ( XlMap[z].Color != 0 )
            {
            if ( z < SIZE_S_LED )
                {
                SENDnote (XlMap[z].Index, XlMap[z].Color);
                }
            else
                SENDcc (XlMap[z].Index, XlMap[z].Color);
            delay (20);
            }
        }
    }

//#######################################################################
MULTIPLEX_C* SYNTH_FRONT_C::Multiplex ()
    {
    return (this->Multiplexer);
    }

//#######################################################################
String SYNTH_FRONT_C::Selected ()
    {
    String str = "";

    for ( int z = 0;  z < ENVELOPE_COUNT;  z++ )
        {
        if ( SelectedEnvelope[z] )
            {
            str += SwitchMapArray[z].Desc;
            str += "  ";
            }
        }
    return str;
    }

//#####################################################################
SYNTH_FRONT_C::SYNTH_FRONT_C (MIDI_MAP* fader_map, MIDI_MAP* knob_map, MIDI_MAP* switch_map, MIDI_XL_MAP* xl_map)
    {
    FaderMap        = fader_map;
    KnobMap         = knob_map;
    SwitchMap       = switch_map;
    XlMap           = xl_map;
    DownKey         = 0;
    DownTrigger     = false;
    DownVelocity    = 0;
    UpKey           = 0;
    UpTrigger       = false;
    SetTuning       = false;
    TuningChange    = false;
    ClearEntryRed   = 0;
    ClearEntryRedL  = 0;
    }

//#######################################################################
void SYNTH_FRONT_C::Begin (int osc_d_a, int mult_digital)
    {

    Multiplexer = new MULTIPLEX_C (mult_digital);

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
//    Midi_2.setHandleNoteOn               (FuncKeyDown);
//    Midi_2.setHandleNoteOff              (FuncKeyUp);
//    Midi_2.setHandleControlChange        (FuncController);
//    Midi_2.setHandlePitchBend            (FuncPitchBend);
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

//    Serial2.begin (31250, SERIAL_8N1, RXD2, TXD2, false);
//    Midi_2.begin (MIDI_CHANNEL_OMNI);
//    printf ("\t>>> Serial2 midi ready\n");

    printf ("\t>>> Starting synth channels\n");
    for ( int z = 0;  z < CHAN_COUNT;  z++ )
        {
        pChan[z] = new SYNTH_CHANNEL_C (z, osc_d_a, EnvADSL);
        osc_d_a   += 8;
        }

    Lfo.Begin (0, osc_d_a);

    this->SawtoothDirection (false);
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        SelectedEnvelope[z] = false;
    }

//#######################################################################
void SYNTH_FRONT_C::Clear ()
    {
    for ( int z = 0;  z < CHAN_COUNT; z++ )
        pChan[z]->Clear ();
    }

//#######################################################################
void SYNTH_FRONT_C::Controller (uint8_t chan, uint8_t type, uint8_t value)
    {
    chan--;

    switch ( type )
        {
        case 0x01:
            // mod wheel
            Lfo.Level (value);
            SoftLFO.Multiplier ((float)value * PRS_SCALER * 0.4);
            DBG ("modulation = %d    ", value);
            break;
        case 0x07:          // Faders controls
            if ( FaderMap[chan].CallBack != nullptr )
                {
                DBG ("%s > %d    ", FaderMap[chan].Desc, value);
                FaderMap[chan].CallBack (FaderMap[chan].Index, value);
                }
            break;
        case 0x0a:          // Rotatation controls
            if ( KnobMap[chan].CallBack != nullptr )
                {
                DBG ("%s %s > %d    ", Selected ().c_str (), KnobMap[chan].Desc, value);
                KnobMap[chan].CallBack (KnobMap[chan].Index, value);
                }
            break;
        case 0x10 ... 0x1F:
            chan = type & 0x0F;
            if ( this->SetTuning && (chan < 8) )
                {
                DBG ("Tuning channel %s", (( value ) ? "ON" : "Off"));
                if ( value )
                    this->TuningOn[chan] = true;
                else
                    this->TuningOn[chan] = false;
                this->TuningChange = true;
                return;
                }
            if ( SwitchMap[chan].CallBack != nullptr )
                {
                DBG ("%s %s", SwitchMap[chan].Desc, (( value ) ? "ON" : "Off"));
                SwitchMap[chan].CallBack (SwitchMap[chan].Index, value);
                }
            break;
        case 0x72:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
            chan = type - (0x72 - 16);
            if ( SwitchMap[chan].CallBack != nullptr )
                {
                DBG ("%s %s", SwitchMap[chan].Desc, (( value ) ? "ON" : "Off"));
                SwitchMap[chan].CallBack (SwitchMap[chan].Index, value);
                }
            break;
        case 0x30 ... 0x4F:
            chan = type - 0x30;
            if ( XlMap[chan].CallBack != nullptr )
                {
                DBG ("%s %s > %d    ", Selected ().c_str (), XlMap[chan].Desc, value);
                XlMap[chan].CallBack (XlMap[chan].Index, value);
                }
            break;
        case 0x50 ... 0x67:
            chan = type - 0x30;
            if ( XlMap[chan].CallBack != nullptr )
                {
                DBG ("%s %s", XlMap[chan].Desc, (( value ) ? "ON" : "Off"));
                XlMap[chan].CallBack (XlMap[chan].Index, value);
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

    if ( this->ClearEntryRed )
        {
        SENDcc (this->ClearEntryRed, 0x3F);
        this->ClearEntryRed = 0;
        }
    if ( this->ClearEntryRedL )
        {
        SENDcc (this->ClearEntryRedL, 0x0D);
        this->ClearEntryRedL = 0;
        }

    Usb.Task    ();
    Midi_0.read ();
    Midi_1.read ();
//    Midi_2.read ();

    if ( !SetTuning )
        {
        if ( this->UpTrigger )
            {
            UpTrigger = false;

            DBG ("Key up > %d", UpKey);
            for ( int z = 0;  z < CHAN_COUNT;  z++ )
                if ( this->pChan[z]->NoteClear (UpKey) )
                    {
                    if ( DebugSynth )
                        printf ("  Osc > %d", z);
                    }
            }

        if ( this->DownTrigger )                          // Key went down so look for a channel to use
            {
            for ( int z = 0;  z < CHAN_COUNT;  z++ )
                {
                SYNTH_CHANNEL_C& ch = *(pChan[z]);

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
                        if ( ch.IsActive () > pChan[oldest]->IsActive () )      // check if current channel older than the oldest so far
                            oldest = z;
                        }
                    }
                }
            if ( doit < 0 )                                 // no unused channel so we will capture the one used the longest
                doit = oldest;
            this->DownTrigger = false;                            // release the trigger
            this->pChan[doit]->NoteSet (DownKey, DownVelocity);   // set the channel
            DBG ("Key down > %d   Velocity > %d  Channel > %d", DownKey, DownVelocity, doit);
            }

        EnvADSL.Loop ();                                    // process all envelope generators
        for ( int z = 0;  z < CHAN_COUNT;  z++ )            // Check all channels for done
            this->pChan[z]->Loop ();
        I2cDevices.UpdateDigital ();
        I2cDevices.UpdateAnalog  ();     // Update D/A ports
        }
    else
        this->Tuning ();
    }

//#######################################################################
void SYNTH_FRONT_C::Tuning ()
    {
    for ( int zc = 0;  zc < CHAN_COUNT;  zc++ )
        {
        if ( this->DownTrigger )
            {
            this->pChan[zc]->pOsc()->SetTuningNote(DownKey);
            DisplayMessage.TuningNote (DownKey);
            }
        if ( this->TuningChange )
            {
            if ( this->TuningOn[zc] )
                {
                for ( int z = 0;  z < ENVELOPE_COUNT;  z++ )
                    {
                    if ( z < OSC_MIXER_COUNT )
                        {
                        this->pChan[zc]->pOsc()->SetTuningVolume (z, TuningLevel[z]);
                        DisplayMessage.TuningLevel (z, TuningLevel[z] * MIDI_INV_MULTIPLIER);
                        }
                    }
                }
            else
                {
                for ( int z = 0;  z < ENVELOPE_COUNT;  z++ )
                    {
                    if ( z < OSC_MIXER_COUNT )
                        this->pChan[zc]->pOsc()->SetTuningVolume(z, 0);
                    }
                }
            }
        }
//    SetNoiseFilterMin (TuningLevel[ENVELOPE_COUNT]);
    this->TuningChange = false;
    this->DownTrigger = false;
    I2cDevices.UpdateDigital();
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_FRONT_C::StartTuning ()
    {
    if ( SetTuning == false )
        {
        DisplayMessage.PageTuning ();
        for ( int z = 0;  z < ENVELOPE_COUNT;  z++)
            {
            TuningLevel[z] = (uint16_t)(pChan[0]->pOsc()->GetMaxLevel (z) * MAX_DA);
            DisplayMessage.TuningLevel (z, TuningLevel[z] * MIDI_INV_MULTIPLIER);
            }
        for ( int zc = 0;  zc < CHAN_COUNT;  zc++ )
            {
            TuningOn[zc] = false;
            TuningChange = true;
            }
        }
    SetTuning = true;
    }

//#######################################################################
void SYNTH_FRONT_C::TuningAdjust (bool up)
    {
    for ( int zc = 0;  zc < CHAN_COUNT;  zc++ )
        {
        if ( TuningOn[zc] )
            pChan[zc]->pOsc()->TuningAdjust (up);
        }
    }

//#######################################################################
void SYNTH_FRONT_C::ChannelSetSelect (uint8_t chan, bool state)
    {
#ifdef TOGGLE
    SelectedEnvelope[chan] = !SelectedEnvelope[chan];
#else
    SelectedEnvelope[chan] = state;
#endif
    DisplayMessage.OscSelected (chan, SelectedEnvelope[chan]);
    DBG ("%s %s ", SwitchMap[chan].Desc,  ( SelectedEnvelope[chan] ) ? "ON" : "off");

    byte val = 0x0C;
    for ( int z = 0;  z < OSC_MIXER_COUNT; z++ )
        {
        if ( SelectedEnvelope[z] )
            val = 0x3C;
        }
    Midi_0.sendNoteOn (PanDevice[0], val, 1);
    delay (20);
    Midi_0.sendNoteOn (PanDevice[1], val, 1);
    delay (20);
    Midi_0.sendNoteOn (PanDevice[2], val, 1);
    delay (20);
    Midi_0.sendNoteOn (PanDevice[3], val, 1);
    }

//#####################################################################
void SYNTH_FRONT_C::SetMBaselevel (uint8_t ch, uint8_t data)
    {
    MidiAdsr[ch].BaseLevel = data;
    }

//#####################################################################
void SYNTH_FRONT_C::SetMaxLevel (uint8_t ch, uint8_t data)
    {
    if ( this->SetTuning )
        {
        this->TuningLevel[ch] = data * MIDI_MULTIPLIER;
        DisplayMessage.TuningLevel (ch, data);
        this->TuningChange = true;
        return;
        }

    float val = (float)data * PRS_SCALER;
    for (int zc = 0;  zc < CHAN_COUNT;  zc++)
        {
        if ( ch < OSC_MIXER_COUNT )
            this->pChan[zc]->pOsc()->SetMaxLevel (ch, val);
        }
    this->MidiAdsr[ch].MaxLevel = data;
    DisplayMessage.OscMaxLevel (ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetAttackTime (uint8_t data)
    {
    float dtime = data * TIME_MULT;
    for ( int zs = 0;  zs < ENVELOPE_COUNT;  zs++ )
        {
        if ( this->SelectedEnvelope[zs] )
            {
            this->MidiAdsr[zs].AttackTime = data;
            for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
                {
                if ( zs < OSC_MIXER_COUNT )
                    this->pChan[zc]->pOsc()->SetAttackTime (zs, dtime);
                }
            DisplayMessage.OscAttackTime (zs, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetDecayTime (uint8_t data)
    {
    float dtime = data * TIME_MULT;
    for ( int zs = 0;  zs < ENVELOPE_COUNT;  zs++ )
        {
        if ( this->SelectedEnvelope[zs] )
            {
            this->MidiAdsr[zs].DecayTime = data;
            for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
                {
                if ( zs < OSC_MIXER_COUNT )
                    this->pChan[zc]->pOsc()->SetDecayTime (zs, dtime);
                }
            DisplayMessage.OscDecayTime (zs, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetSustainLevel (uint8_t ch, uint8_t data)
    {
    float val = (float)data * PRS_SCALER;
    for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
        {
        if ( ch < OSC_MIXER_COUNT )
            this->pChan[zc]->pOsc()->SetSustainLevel (ch, val);
        }
    this->MidiAdsr[ch].SustainLevel = data;
    DisplayMessage.OscSustainLevel (ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetSustainTime (uint8_t data)
    {
    float dtime;

    if ( data == 0 )
        dtime = -1;
    else
        dtime = data * TIME_MULT;

    for ( int zs = 0;  zs < ENVELOPE_COUNT;  zs++ )
        {
        if ( this->SelectedEnvelope[zs] )
            {
            this->MidiAdsr[zs].SustainTime = data;
            for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
                {
                if ( zs < OSC_MIXER_COUNT )
                    this->pChan[zc]->pOsc()->SetSustainTime (zs, dtime);
                }
            DisplayMessage.OscSustainTime (zs, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetReleaseTime (uint8_t data)
    {
    float dtime = data * TIME_MULT;
    for ( int zs = 0;  zs < ENVELOPE_COUNT;  zs++ )
        {
        if ( this->SelectedEnvelope[zs] )
            {
            this->MidiAdsr[zs].ReleaseTime = data;
            for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
                {
                if ( zs < OSC_MIXER_COUNT )
                    this->pChan[zc]->pOsc()->SetReleaseTime (zs, dtime);
                }
            DisplayMessage.OscReleaseTime (zs, data);
            }
        }
    }

//#######################################################################
void SYNTH_FRONT_C::SawtoothDirection (bool data)
    {
    for ( int z = 0;  z < CHAN_COUNT;  z++)
        this->pChan[z]->pOsc()->SawtoothDirection (data);
    this->SawToothDirection = data;
    DisplayMessage.OscSawtoothDirection (data);
    if ( !data )
        this->ClearEntryRedL = XlMap[37].Index;
    }

//#######################################################################
void SYNTH_FRONT_C::SetPulseWidth (byte data)
    {
    float percent = data * PRS_SCALER;
    for ( int z = 0;  z < CHAN_COUNT;  z++)
        pChan[z]->pOsc()->PulseWidth (percent);
    this->PulseWidth = data;
    DisplayMessage.OscPulseWidth (data);
    }

//#####################################################################
void SYNTH_FRONT_C::DisplayUpdate ()
    {
    uint8_t zd;

    for ( uint8_t z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        DisplayMessage.OscSelected (z, this->SelectedEnvelope[z]);
        DisplayMessage.OscMaxLevel (z, this->MidiAdsr[z].MaxLevel);
        DisplayMessage.OscAttackTime (z, this->MidiAdsr[z].AttackTime);
        DisplayMessage.OscDecayTime (z, this->MidiAdsr[z].DecayTime);
        DisplayMessage.OscSustainTime (z, this->MidiAdsr[z].SustainTime);
        DisplayMessage.OscReleaseTime (z, this->MidiAdsr[z].ReleaseTime);
        DisplayMessage.OscSustainLevel (z, this->MidiAdsr[z].SustainLevel);
        }
    DisplayMessage.OscSawtoothDirection (this->SawToothDirection);
    DisplayMessage.OscPulseWidth (this->PulseWidth);
    }

//#######################################################################
void SYNTH_FRONT_C::SaveAllSettings ()
    {
    for ( int z = 0;  z < CHAN_COUNT;  z++ )
        Settings.PutOscBank (z, this->pChan[z]->pOsc ()->GetBankAddr ());
    }


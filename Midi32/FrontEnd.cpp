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
//    DBGM ("valid = %X   type = %X   channel = %X   data1 = %X   data2 = %X   length = %X",
//          msg.valid, msg.type, msg.channel, msg.data1, msg.data2, msg.length);
    }

//###################################################################
static void  FuncKeyDown (byte chan, byte key, byte velocity)
    {
    DBGM ("Key down  channel: %d  key: %d  velocity: %d", chan, key, velocity);
    SynthFront.KeyDown (chan, key, velocity);
    }

//###################################################################
static void  FuncKeyUp (byte chan, byte key, byte velocity)
    {
    DBGM ("Key up  channel: %d  key: %d  velocity: %d", chan, key, velocity);
    SynthFront.KeyUp (chan, key, velocity);
    }

//###################################################################
static void FuncController (byte chan, byte type, byte value)
    {
    SynthFront.Controller (chan, type, value);
    DBGM ("Controller Chan %2.2X  type %2.2X  value %2.2X", chan, type, value);
    }

//###################################################################
static void FuncPitchBend (byte chan, int value)
    {
    SynthFront.PitchBend (chan, value);
    DBGM ("Pitch Bend  Chan %2.2X  value %d", chan, value);
    }

//###################################################################
static void FuncError (signed char err)
    {
    DBGM ("*** MIDI Error %d", err);
    }

//###################################################################
static void FuncProgramChange (byte channel, byte program)
    {
    DBGM ("Program change : channel %d, program %d", channel, program);
    }

//###################################################################
static void FuncAfterTouchPoly (byte channel, byte note, byte pressure)
    {
    DBGM ("Polyphonic after touch : channel %d, note %d, pressure %d", channel, note, pressure);
    }

//###################################################################
static void FuncAfterTouchChannel (byte channel, byte pressure)
    {
    DBGM ("After touch : channel %d, pressure %d", channel, pressure);
    }

//###################################################################
static void FuncSystemExclusive (byte* data, unsigned size)
    {
#ifdef DEBUG_ON
    if ( DebugMidi )
        {
        printf ("[M] System Exclusive {");
        for ( unsigned z = 0;  z < size;  z++ )
            printf(" %2X", data[z]);
        printf(" }\n");
        }
#endif
    }

//###################################################################
static void FuncTimeCodeQuarterFrame (byte data)
    {
    DBGM ("TimeCode QuarterFrame : %2X", data);
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
    return (this->pMultiplexer);
    }

//#######################################################################
NOISE_C* SYNTH_FRONT_C::Noise ()
    {
    return (this->pNoise);
    }

//#######################################################################
String SYNTH_FRONT_C::Selected ()
    {
    String str = "";

    for ( int z = 0;  z < ENVELOPE_COUNT;  z++ )
        {
        if ( this->pZone[this->CurrentZone]->SelectedEnvelope[z] )
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
void SYNTH_FRONT_C::Begin (int osc_d_a, int mult_digital, int noise_digital)
    {
    pMultiplexer = new MULTIPLEX_C (mult_digital);
    pNoise       = new NOISE_C     (noise_digital);

//    Midi_0.setHandleMessage              (FuncMessage);
    Midi_0.setHandleNoteOn               (FuncKeyDown);
    Midi_0.setHandleNoteOff              (FuncKeyUp);
    Midi_0.setHandleControlChange        (FuncController);
    Midi_0.setHandlePitchBend            (FuncPitchBend);
    Midi_0.setHandleError                (FuncError);
    Midi_0.setHandleAfterTouchPoly       (FuncAfterTouchPoly);
    Midi_0.setHandleProgramChange        (FuncProgramChange);
    Midi_0.setHandleAfterTouchChannel    (FuncAfterTouchChannel);
    Midi_0.setHandleSystemExclusive      (FuncSystemExclusive);
    Midi_0.setHandleTimeCodeQuarterFrame (FuncTimeCodeQuarterFrame);
//  Midi_0.setHandleSongPosition         (SongPositionCallback);
//  Midi_0.setHandleSongSelect           (SongSelectCallback);
//  Midi_0.setHandleTuneRequest          (TuneRequestCallback);
//  Midi_0.setHandleClock                (ClockCallback);
//  Midi_0.setHandleStart                (StartCallback);
//  Midi_0.setHandleTick                 (TickCallback);
//  Midi_0.setHandleContinue             (ContinueCallback);
//  Midi_0.setHandleStop                 (StopCallback);
//  Midi_0.setHandleActiveSensing        (ActiveSensingCallback);
//  Midi_0.setHandleSystemReset          (SystemResetCallback);

//    Midi_1.setHandleMessage              (FuncMessage);
    Midi_1.setHandleNoteOn               (FuncKeyDown);
    Midi_1.setHandleNoteOff              (FuncKeyUp);
    Midi_1.setHandleControlChange        (FuncController);
    Midi_1.setHandlePitchBend            (FuncPitchBend);
    Midi_1.setHandleError                (FuncError);
    Midi_1.setHandleAfterTouchPoly       (FuncAfterTouchPoly);
    Midi_1.setHandleProgramChange        (FuncProgramChange);
    Midi_1.setHandleAfterTouchChannel    (FuncAfterTouchChannel);
    Midi_1.setHandleSystemExclusive      (FuncSystemExclusive);
    Midi_1.setHandleTimeCodeQuarterFrame (FuncTimeCodeQuarterFrame);
//  Midi_1.setHandleSongPosition         (SongPositionCallback);
//  Midi_1.setHandleSongSelect           (SongSelectCallback);
//  Midi_1.setHandleTuneRequest          (TuneRequestCallback);
//  Midi_1.setHandleClock                (ClockCallback);
//  Midi_1.setHandleStart                (StartCallback);
//  Midi_1.setHandleTick                 (TickCallback);
//  Midi_1.setHandleContinue             (ContinueCallback);
//  Midi_1.setHandleStop                 (StopCallback);
//  Midi_1.setHandleActiveSensing        (ActiveSensingCallback);
//  Midi_1.setHandleSystemReset          (SystemResetCallback);

//  Midi_2.setHandleMessage              (FuncMessage);
//    Midi_2.setHandleNoteOn               (FuncKeyDown);
//    Midi_2.setHandleNoteOff              (FuncKeyUp);
//    Midi_2.setHandleControlChange        (FuncController);
//    Midi_2.setHandlePitchBend            (FuncPitchBend);
//  Midi_2.setHandleError                (FuncError);
//  Midi_2.setHandleAfterTouchPoly       (FuncAfterTouchPoly);
//  Midi_2.setHandleProgramChange        (FuncProgramChange);
//  Midi_2.setHandleAfterTouchChannel    (FuncAfterTouchChannel);
//  Midi_2.setHandleSystemExclusive      (FuncSystemExclusive);
//  Midi_2.setHandleTimeCodeQuarterFrame (FuncTimeCodeQuarterFrame);
//  Midi_2.setHandleSongPosition         (SongPositionCallback);
//  Midi_2.setHandleSongSelect           (SongSelectCallback);
//  Midi_2.setHandleTuneRequest          (TuneRequestCallback);
//  Midi_2.setHandleClock                (ClockCallback);
//  Midi_2.setHandleStart                (StartCallback);
//  Midi_2.setHandleTick                 (TickCallback);
//  Midi_2.setHandleContinue             (ContinueCallback);
//  Midi_2.setHandleStop                 (StopCallback);
//  Midi_2.setHandleActiveSensing        (ActiveSensingCallback);
//  Midi_2.setHandleSystemReset          (SystemResetCallback);

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
        pChan[z] = new CHANNEL_C (z, osc_d_a, EnvADSL);
        osc_d_a   += 8;
        }
    pZone[0]     = pChan[0];
    pZone[1]     = pChan[0];
    pZone[2]     = pChan[4];
    CurrentZone  = ZONE0;
    ZoneCount    = CHAN_COUNT;
    Lfo.Begin (0, osc_d_a);

    this->SawtoothDirection (false);
    for ( int zc = 0;  zc < CHAN_COUNT;  zc++ )
        {
        for (int z = 0;  z < OSC_MIXER_COUNT;  z++)
            pChan[zc]->SelectedEnvelope[z] = false;
        }
    }

//#######################################################################
void SYNTH_FRONT_C::Clear ()
    {
    for ( int z = 0;  z < CHAN_COUNT; z++ )
        pChan[z]->Clear ();
    }

//#######################################################################
void SYNTH_FRONT_C::Controller (byte chan, byte type, byte value)
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
                CHANNEL_C& ch = *(pChan[z]);

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
void SYNTH_FRONT_C::ChannelSetSelect (byte chan, bool state)
    {
#ifdef TOGGLE
    pZone[CurrentZone]->SelectedEnvelope[chan] = !pZone[CurrentZone]->SelectedEnvelope[chan];
#else
    pZone[CurrentZone]->SelectedEnvelope[chan] = state;
#endif
    DisplayMessage.OscSelected (CurrentZone, chan, pZone[CurrentZone]->SelectedEnvelope[chan]);
    DBG ("%s %s ", SwitchMap[chan].Desc,  ( pZone[CurrentZone]->SelectedEnvelope[chan] ) ? "ON" : "off");

    byte val = 0x0C;
    for ( int z = 0;  z < OSC_MIXER_COUNT; z++ )
        {
        if ( pZone[CurrentZone]->SelectedEnvelope[z] )
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
void SYNTH_FRONT_C::SetMaxLevel (byte ch, byte data)
    {
    if ( this->SetTuning )
        {
        this->TuningLevel[ch] = data * MIDI_MULTIPLIER;
        DisplayMessage.TuningLevel (ch, data);
        this->TuningChange = true;
        return;
        }

    float val = (float)data * PRS_SCALER;

    for ( int z = 0;  z < ZoneCount;  z++ )
        this->pChan[CurrentZone + z]->SetMaxLevel (ch, val);

    DisplayMessage.OscMaxLevel (CurrentZone, ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetAttackTime (byte data)
    {
    float dtime = data * TIME_MULT;

    for ( int ch = 0;  ch < OSC_MIXER_COUNT;  ch++ )
        {
        if ( pZone[CurrentZone]->SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < ZoneCount;  z++ )
                this->pChan[CurrentZone + z]->pOsc()->SetAttackTime (ch, dtime);
            DisplayMessage.OscAttackTime (CurrentZone, ch, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetDecayTime (byte data)
    {
    float dtime = data * TIME_MULT;

    for ( int ch = 0;  ch < OSC_MIXER_COUNT;  ch++ )
        {
        if ( this->pZone[CurrentZone]->SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < ZoneCount;  z++)
                this->pChan[CurrentZone + z]->pOsc()->SetDecayTime (ch, dtime);
            DisplayMessage.OscDecayTime (CurrentZone, ch, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetSustainLevel (byte ch, byte data)
    {
    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < ZoneCount;  z++ )
        this->pChan[CurrentZone + z]->pOsc()->SetSustainLevel (ch, val);
    DisplayMessage.OscSustainLevel (CurrentZone, ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetSustainTime (byte data)
    {
    float dtime;

    if ( data == 0 )
        dtime = -1;
    else
        dtime = data * TIME_MULT;

    for ( int ch = 0;  ch < OSC_MIXER_COUNT;  ch++ )
        {
        if ( this->pZone[CurrentZone]->SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < ZoneCount;  z++)
                this->pChan[CurrentZone + z]->pOsc()->SetSustainTime (ch, dtime);
            DisplayMessage.OscSustainTime (CurrentZone, ch, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetReleaseTime (byte data)
    {
    float dtime = data * TIME_MULT;

    for ( int ch = 0;  ch < OSC_MIXER_COUNT;  ch++ )
        {
        if ( this->pZone[CurrentZone]->SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < ZoneCount;  z++)
                this->pChan[CurrentZone + z]->pOsc()->SetReleaseTime (ch, dtime);
            DisplayMessage.OscReleaseTime (CurrentZone, ch, data);
            }
        }
    }

//#######################################################################
void SYNTH_FRONT_C::SawtoothDirection (bool data)
    {
    for ( int z = 0;  z < CHAN_COUNT;  z++)
        this->pChan[z]->pOsc()->SawtoothDirection (data);
    DisplayMessage.OscSawtoothDirection (CurrentZone, data);
    if ( !data )
        this->ClearEntryRedL = XlMap[37].Index;
    }

//#######################################################################
void SYNTH_FRONT_C::SetPulseWidth (byte data)
    {
    float percent = data * PRS_SCALER;
    for ( int z = 0;  z < ZoneCount;  z++)
        this->pChan[CurrentZone + z]->pOsc()->PulseWidth (percent);
    DisplayMessage.OscPulseWidth (CurrentZone, data);
    }

//#######################################################################
void SYNTH_FRONT_C::SetNoise (byte ch, bool state)
    {
    int z;

    switch (  CurrentZone )
        {
        case ZONE0:
            for ( z = 0;  z < DUCT_NUM;  z++ )
                this->pNoise->Select (z, ch, state);
            DisplayMessage.OscNoise (ZONE0, ch, state);
        default:
            if ( ch & 0x80 )
                {
                ch &= 0x0F;
                for ( z = 0;  z < (DUCT_NUM / NUM_ZONES);  z++ )
                    this->pNoise->Select (z + 2, ch, state);
                DisplayMessage.OscNoise (ZONE2, ch, state);
                }
            else
                {
                for ( int z = 0;  z < (DUCT_NUM / NUM_ZONES);  z ++)
                    this->pNoise->Select (z, ch, state);
                DisplayMessage.OscNoise (ZONE1, ch, state);
                }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::DisplayUpdate (int zone)
    {
    int zcount;
    CHANNEL_C& ch = *(this->pZone[zone]);
    OSC_C& osc    = *(ch.pOsc ());

    switch ( zone )
        {
        case ZONE0:
            zcount = 8;
            break;
        case ZONE1:
            zcount = 4;
            break;
        case ZONE2:
            zcount = 4;
            break;
        default:
            return;
        }

    for ( byte z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        OSC_C& osc = *(this->pZone[zone]->pOsc ());

        DisplayMessage.OscSelected     (zone, z, ch.SelectedEnvelope[z]);
        DisplayMessage.OscMaxLevel     (zone, z, osc.GetMaxLevel (z));
        DisplayMessage.OscAttackTime   (zone, z, osc.GetAttackTime (z));
        DisplayMessage.OscDecayTime    (zone, z,   osc.GetDecayTime (z));
        DisplayMessage.OscSustainTime  (zone, z, osc.GetSustainTime (z));
        DisplayMessage.OscReleaseTime  (zone, z, osc.GetReleaseTime (z));
        DisplayMessage.OscSustainLevel (zone, z,osc.GetSustainLevel (z));
        }
    DisplayMessage.OscSawtoothDirection (zone, this->pZone[zone]->GetSawToothDirection ());
    DisplayMessage.OscPulseWidth        (zone, this->pZone[zone]->GetPulseWidth ());
    }

//#######################################################################
void SYNTH_FRONT_C::SaveAllSettings ()
    {
    for ( int z = 0;  z < CHAN_COUNT;  z++ )
        Settings.PutOscBank (z, this->pChan[z]->pOsc ()->GetBankAddr ());
    }


//#######################################################################
// Module:     SyntFront.ino
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <UHS2-MIDI.h>

#include "../Common/SynthCommon.h"
#include "../Common/DispMessages.h"
#include "Osc.h"
#include "LFOosc.h"
#include "Noise.h"
#include "Settings.h"
#include "FrontEnd.h"
#include "SynthChannel.h"
#include "Debug.h"

static const char* Label = "TOP";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,0,args);}}

//#define TOGGLE          // use if channel select switches are to be alternate action

static      USB Usb;
static      UHS2MIDI_CREATE_INSTANCE(&Usb, MIDI_PORT, Midi);
static      MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, Midi2);

#define     SENDnote(k,c) Midi.sendNoteOn (k, c, 1); printf("Note 0x%X  Color = 0x%X\n", k, c);
#define     SENDcc(k, c)  Midi.send (midi::MidiType::ControlChange, k, c, 1); printf("code 0x%X Color = 0x%X from <%s>\n", k, c, __FUNCTION__);

static      SYNTH_CHANNEL_C*    pChan[CHAN_COUNT];
static      LFO_N::SYNTH_LFO_C  Lfo;

//###################################################################
static void  FuncKeyDown (uint8_t chan, uint8_t key, uint8_t velocity)
    {
    SynthFront.KeyDown (chan, key, velocity);
    }

//###################################################################
static void  FuncKeyUp (uint8_t chan, uint8_t key, uint8_t velocity)
    {
    SynthFront.KeyUp (chan, key, velocity);
    }

//###################################################################
static void FuncController (uint8_t chan, uint8_t type, uint8_t value)
    {
    SynthFront.Controller (chan, type, value);
    if ( DebugMidi )
        printf ("Chan %2.2X  type %2.2X  value %2.2X\n", chan, type, value);
    }

//###################################################################
static void FuncPitchBend (uint8_t chan, int value)
    {
    SynthFront.PitchBend (chan, value);
    }

//#######################################################################
//#######################################################################
void SendTest1 (void)
    {
    static byte devtest = 0x50;
    printf("Sending 0x01, %X, 0x3F\n", devtest);
    SENDcc (devtest, 0x3C);
    devtest += 1;
    if ( devtest > 0x67 )
        devtest = 0x50;
    }

void SendTest2 (void)
    {
    static byte val = 0xFF;
    printf("Sending note data %d\n", val);
    SENDcc (val, 0x3B);
    val += 1;
    }

void SendTest3 (void)
    {
    static byte devtest = 0x50;
    printf("Sending 0x01, %X, 0x3F\n", devtest);
    SENDcc (devtest, 0x2E);
    devtest += 1;
    if ( devtest > 0x67 )
        devtest = 0x50;
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
            printf("code %d[0x%X]  Color = 0x%X\n", XlMap[z].Index, XlMap[z].Index, XlMap[z].Color);
            if ( z < SIZE_S_LED )
                {
                SENDnote (XlMap[z].Index, XlMap[z].Color);
                }
            else
                {
                SENDcc (XlMap[z].Index, XlMap[z].Color);
                }
            delay (20);
            }
        }
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
    ClearEntryRed   = 0;
    ClearEntryRedL  = 0;
    }

//#######################################################################
void SYNTH_FRONT_C::Begin (int osc_d_a, int noise_d_a, int noise_dig)
    {
//  Midi.setHandleMessage              (void (*fptr)(const MidiMessage&));
//  Midi.setHandleError                (ErrorCallback fptr);
    Midi.setHandleNoteOn               (FuncKeyDown);
    Midi.setHandleNoteOff              (FuncKeyUp);
//  Midi.setHandleAfterTouchPoly       (AfterTouchPolyCallback fptr);
    Midi.setHandleControlChange        (FuncController);
//  Midi.setHandleProgramChange        (ProgramChangeCallback fptr);
//  Midi.setHandleAfterTouchChannel    (AfterTouchChannelCallback fptr);
    Midi.setHandlePitchBend            (FuncPitchBend);
//  Midi.setHandleSystemExclusive      (SystemExclusiveCallback fptr);
//  Midi.setHandleTimeCodeQuarterFrame (TimeCodeQuarterFrameCallback fptr);
//  Midi.setHandleSongPosition         (SongPositionCallback fptr);
//  Midi.setHandleSongSelect           (SongSelectCallback fptr);
//  Midi.setHandleTuneRequest          (TuneRequestCallback fptr);
//  Midi.setHandleClock                (ClockCallback fptr);
//  Midi.setHandleStart                (StartCallback fptr);
//  Midi.setHandleTick                 (TickCallback fptr);
//  Midi.setHandleContinue             (ContinueCallback fptr);
//  Midi.setHandleStop                 (StopCallback fptr);
//  Midi.setHandleActiveSensing        (ActiveSensingCallback fptr);
//  Midi.setHandleSystemReset          (SystemResetCallback fptr);

//  Midi2.setHandleMessage              (void (*fptr)(const MidiMessage&));
//  Midi2.setHandleError                (ErrorCallback fptr);
    Midi2.setHandleNoteOn               (FuncKeyDown);
    Midi2.setHandleNoteOff              (FuncKeyUp);
//  Midi2.setHandleAfterTouchPoly       (AfterTouchPolyCallback fptr);
    Midi2.setHandleControlChange        (FuncController);
//  Midi2.setHandleProgramChange        (ProgramChangeCallback fptr);
//  Midi2.setHandleAfterTouchChannel    (AfterTouchChannelCallback fptr);
    Midi2.setHandlePitchBend            (FuncPitchBend);
//  Midi2.setHandleSystemExclusive      (SystemExclusiveCallback fptr);
//  Midi2.setHandleTimeCodeQuarterFrame (TimeCodeQuarterFrameCallback fptr);
//  Midi2.setHandleSongPosition         (SongPositionCallback fptr);
//  Midi2.setHandleSongSelect           (SongSelectCallback fptr);
//  Midi2.setHandleTuneRequest          (TuneRequestCallback fptr);
//  Midi2.setHandleClock                (ClockCallback fptr);
//  Midi2.setHandleStart                (StartCallback fptr);
//  Midi2.setHandleTick                 (TickCallback fptr);
//  Midi2.setHandleContinue             (ContinueCallback fptr);
//  Midi2.setHandleStop                 (StopCallback fptr);
//  Midi2.setHandleActiveSensing        (ActiveSensingCallback fptr);
//  Midi2.setHandleSystemReset          (SystemResetCallback fptr);

    printf ("\t>>> Midi interfaces startup\n");
    while ( Usb.Init () == -1 )
        {
        delay (200);
        printf ("Usb midi init retry!\n");
        }
    printf ("\t>>> Usb midi init done!\n");
    Serial2.begin (31250, SERIAL_8N1, RXD2, TXD2, false);

    Midi2.begin (MIDI_CHANNEL_OMNI);
    delay (200);
    printf ("\t>>> Serial2 midi init done!\n");

    printf ("\t>>> Starting synth channels\n");
    for ( int z = 0;  z < CHAN_COUNT;  z++ )
        {
        pChan[z] = new SYNTH_CHANNEL_C (z, osc_d_a, noise_d_a, noise_dig, EnvADSL);
        osc_d_a   += 8;
        noise_d_a += 2;
        if ( !(z & 1) )
            noise_dig -= 2;
        }
    NoiseColorDev = noise_dig;
    Lfo.Begin (0, osc_d_a);

    this->SawtoothDirection (false);
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        SelectedEnvelope[z] = false;
    }

//#######################################################################
void SYNTH_FRONT_C::Controller (uint8_t chan, uint8_t type, uint8_t value)
    {
    chan--;

    switch ( type )
        {
        case 0x01:
            // mod wheel
//            Lfo.Level (value);
            SineWave.Multiplier ((float)value * PRS_SCALER * 0.4);
            DBG ("modulation = %f    ", (float)value * (float)PRS_SCALER);
            break;
        case 0x07:          // Faders controls
        case 0x0a:          // Rotatation controls
            if ( KnobMap[chan].CallBack != nullptr )
                {
                DBG ("%s %s > %d    ", Selected ().c_str (), KnobMap[chan].Desc, value);
                KnobMap[chan].CallBack (chan, value);
                }
            break;
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F:
            chan = type & 0x0F;
            if ( SetTuning && (chan < 6) )
                {
                if ( value )
                    TuningOn[chan] = true;
                else
                    TuningOn[chan] = false;
                return;
                }
            if ( SwitchMap[chan].CallBack != nullptr )
                {
                DBG ("%s %s", SwitchMap[chan].Desc, (( value ) ? "ON" : "Off"));
                SwitchMap[chan].CallBack (chan, value);
                }
            break;
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
            chan = type & 0x1F;
            if ( SwitchMap[chan].CallBack != nullptr )
                {
                DBG ("%s %s", SwitchMap[chan].Desc, (( value ) ? "ON" : "Off"));
                SwitchMap[chan].CallBack (chan, value);
                }
            break;
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x36:
        case 0x37:
        case 0x38:
        case 0x39:
        case 0x3A:
        case 0x3B:
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F:
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4E:
        case 0x4F:
            chan = type - 0x30;
            if ( XlMap[chan].CallBack != nullptr )
                {
                DBG ("%s %s > %d    ", Selected ().c_str (), XlMap[chan].Desc, value);
                XlMap[chan].CallBack (XlMap[chan].Index, value);
                }
            break;
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
        case 0x58:
        case 0x59:
        case 0x5A:
        case 0x5B:
        case 0x5C:
        case 0x5D:
        case 0x5E:
        case 0x5F:
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
            chan = type - 0x30;
            if ( XlMap[chan].CallBack != nullptr )
                {
                DBG ("%s %s", XlMap[chan].Desc, (( value ) ? "ON" : "Off"));
                XlMap[chan].CallBack (XlMap[chan].Index, value);
                }
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

    if ( ClearEntryRed )
        {
        SENDcc (ClearEntryRed, 0x3F);
        ClearEntryRed = 0;
        }
    if ( ClearEntryRedL )
        {
        SENDcc (ClearEntryRedL, 0x0D);
        ClearEntryRedL = 0;
        }

    Usb.Task ();
    Midi.read ();
    Midi2.read ();

    if ( !SetTuning )
        {
        if ( UpTrigger )
            {
            UpTrigger = false;

            DBG ("Key up > %d", UpKey);
            for ( int z = 0;  z < CHAN_COUNT;  z++ )
                if ( pChan[z]->NoteClear (UpKey) )
                    {
                    if ( DebugSynth )
                        printf ("  Osc > %d", z);
                    }
            }

        if ( DownTrigger )                          // Key went down so look for a channel to use
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
            DownTrigger = false;                            // release the trigger
            pChan[doit]->NoteSet (DownKey, DownVelocity);   // set the channel
            DBG ("Key down > %d   Velocity > %d  Channel > %d", DownKey, DownVelocity, doit);
            }

        EnvADSL.Loop ();                                    // process all envelope generators
        for ( int z = 0;  z < CHAN_COUNT;  z++ )            // Check all channels for done
            pChan[z]->Loop ();
        I2cDevices.UpdateDigital ();
        I2cDevices.UpdateAnalog  ();     // Update D/A ports
        }
    else
        Tuning ();
    }

//#######################################################################
void SYNTH_FRONT_C::Tuning ()
    {
    for ( int zc = 0;  zc < CHAN_COUNT;  zc++ )
        {
        if ( DownTrigger )
            pChan[zc]->pOsc()->SetTuningNote(DownKey);
        if ( TuningOn[zc] )
            {
            for ( int z = 0;  z < ENVELOPE_COUNT;  z++ )
                {
                if ( z < OSC_MIXER_COUNT )
                    pChan[zc]->pOsc()->SetTuningVolume(z, TuningLevel[z]);
                else
                    pChan[zc]->pNoise()->SetTuningVolume(z - OSC_MIXER_COUNT, TuningLevel[z]);
                }
            }
        else
            {
            for ( int z = 0;  z < ENVELOPE_COUNT;  z++ )
                {
                if ( z < OSC_MIXER_COUNT )
                    pChan[zc]->pOsc()->SetTuningVolume(z, 0);
                else
                    pChan[zc]->pNoise()->SetTuningVolume(z - OSC_MIXER_COUNT, 0);
                }
            }
        }
//    SetNoiseFilterMin (TuningLevel[ENVELOPE_COUNT]);
    DownTrigger = false;
    I2cDevices.UpdateDigital();
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_FRONT_C::StartTuning ()
    {
    if ( SetTuning == false )
        {
        DisplayMessage.Page (DISP_MESSAGE_N::PAGE_C::PAGE_TUNING);
        for ( int z = 0;  z < ENVELOPE_COUNT;  z++)
            TuningLevel[z] = 0;
        for ( int zc = 0;  zc < CHAN_COUNT;  zc++ )
            TuningOn[zc] = false;
        }
    SetTuning = true;
    }

//#######################################################################
void SYNTH_FRONT_C::PitchBend (uint8_t ch, int value)
    {
    float scaler = (value + 16384) * BEND_SCALER;
    Lfo.PitchBend (scaler);
    DBG ("Pitch bend > %f", scaler);
    }

//#######################################################################
void SYNTH_FRONT_C::ChannelSetSelect (uint8_t chan, bool state)
    {
#ifdef TOGGLE
    SelectedEnvelope[chan] = !SelectedEnvelope[chan];
#else
    SelectedEnvelope[chan] = state;
#endif
    DisplayMessage.Selected (chan, SelectedEnvelope[chan]);
    DBG ("%s %s", SwitchMap[chan].Desc,  ( SelectedEnvelope[chan] ) ? " ON" : " off");

    byte val = 0x0C;
    for ( int z = 0;  z < OSC_MIXER_COUNT; z++ )
        {
        if ( SelectedEnvelope[z] )
            val = 0x3C;
        }
    Midi.sendNoteOn (PanDevice[0], val, 1);
    delay (20);
    Midi.sendNoteOn (PanDevice[1], val, 1);
    delay (20);
    Midi.sendNoteOn (PanDevice[2], val, 1);
    delay (20);
    Midi.sendNoteOn (PanDevice[3], val, 1);
    }

//#####################################################################
void SYNTH_FRONT_C::SetMBaselevel (uint8_t ch, uint8_t data)
    {
    float val = (float)data * PRS_SCALER;
    for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
        {
        if ( ch >= OSC_MIXER_COUNT )
            pChan[zc]->pNoise()->SetMaxLevel (ch - OSC_MIXER_COUNT, val);
        }
    MidiAdsr[ch].BaseLevel = data;
    }

//#####################################################################
void SYNTH_FRONT_C::SetMaxLevel (uint8_t ch, uint8_t data)
    {
    if ( SetTuning )
        {
        TuningLevel[ch] = data * MIDI_MULTIPLIER;
        return;
        }

    float val = (float)data * PRS_SCALER;
    for (int zc = 0;  zc < CHAN_COUNT;  zc++)
        {
        if ( ch < OSC_MIXER_COUNT )
            pChan[zc]->pOsc()->SetMaxLevel (ch, val);
        else
            pChan[zc]->pNoise()->SetMaxLevel (ch - OSC_MIXER_COUNT, val);
        }
    MidiAdsr[ch].MaxLevel = data;
    DisplayMessage.MaxLevel (ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetAttackTime (uint8_t data)
    {
    float dtime = data * TIME_MULT;
    for ( int zs = 0;  zs < ENVELOPE_COUNT;  zs++ )
        {
        if ( SelectedEnvelope[zs] )
            {
            MidiAdsr[zs].AttackTime = data;
            for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
                {
                if ( zs < OSC_MIXER_COUNT )
                    pChan[zc]->pOsc()->SetAttackTime (zs, dtime);
                else
                    pChan[zc]->pNoise()->SetAttackTime (zs - OSC_MIXER_COUNT, dtime);
                }
            DisplayMessage.AttackTime (zs, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetDecayTime (uint8_t data)
    {
    float dtime = data * TIME_MULT;
    for ( int zs = 0;  zs < ENVELOPE_COUNT;  zs++ )
        {
        if ( SelectedEnvelope[zs] )
            {
            MidiAdsr[zs].DecayTime = data;
            for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
                {
                if ( zs < OSC_MIXER_COUNT )
                    pChan[zc]->pOsc()->SetDecayTime (zs, dtime);
                else
                    pChan[zc]->pNoise()->SetDecayTime (zs - OSC_MIXER_COUNT, dtime);
                }
            DisplayMessage.DecayTime (zs, data);
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
            pChan[zc]->pOsc()->SetSustainLevel (ch, val);
        else
            pChan[zc]->pNoise()->SetSustainLevel (ch - OSC_MIXER_COUNT, val);
        }
    MidiAdsr[ch].SustainLevel = data;
    DisplayMessage.SustainLevel (ch, data);
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
        if ( SelectedEnvelope[zs] )
            {
            MidiAdsr[zs].SustainTime = data;
            for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
                {
                if ( zs < OSC_MIXER_COUNT )
                    pChan[zc]->pOsc()->SetSustainTime (zs, dtime);
                else
                    pChan[zc]->pNoise()->SetSustainTime (zs - OSC_MIXER_COUNT, dtime);
                }
            DisplayMessage.SustainTime (zs, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetReleaseTime (uint8_t data)
    {
    float dtime = data * TIME_MULT;
    for ( int zs = 0;  zs < ENVELOPE_COUNT;  zs++ )
        {
        if ( SelectedEnvelope[zs] )
            {
            MidiAdsr[zs].ReleaseTime = data;
            for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
                {
                if ( zs < OSC_MIXER_COUNT )
                    pChan[zc]->pOsc()->SetReleaseTime (zs, dtime);
                else
                    pChan[zc]->pNoise()->SetReleaseTime (zs - OSC_MIXER_COUNT, dtime);
                }
            DisplayMessage.ReleaseTime (zs, data);
            }
        }
    }

//#######################################################################
void SYNTH_FRONT_C::SawtoothDirection (bool data)
    {
    for ( int z = 0;  z < CHAN_COUNT;  z++)
        pChan[z]->pOsc()->SawtoothDirection (data);
    this->SawToothDirection = data;
    DisplayMessage.SawtoothDirection (data);
    if ( !data )
        ClearEntryRedL = XlMap[37].Index;
    }

//#######################################################################
void SYNTH_FRONT_C::SetPulseWidth (byte data)
    {
    float percent = data * PRS_SCALER;
    for ( int z = 0;  z < CHAN_COUNT;  z++)
        pChan[z]->pOsc()->PulseWidth (percent);
    this->PulseWidth = data;
    DisplayMessage.PulseWidth (data);
    }

//#####################################################################
void SYNTH_FRONT_C::SelectWaveVCA (uint8_t ch, uint8_t state)
    {
    for ( int z = 0;  z < CHAN_COUNT;  z++)
        pChan[z]->pOsc()->SoftLFO (ch, state);
    }

//#####################################################################
void SYNTH_FRONT_C::SelectWaveVCF (uint8_t ch, uint8_t state)
    {
    Lfo.Select (ch, state);
    }

//#####################################################################
void SYNTH_FRONT_C::FreqSelectLFO (uint8_t ch, uint8_t data)
    {
    Lfo.SetFreq (data);
    }

//#######################################################################
void SYNTH_FRONT_C::LFOrange (bool up, bool button)
    {
    if ( SetTuning )
        {
        for ( int zc = 0;  zc < CHAN_COUNT;  zc++ )
            {
            if ( TuningOn[zc] )
                pChan[zc]->pOsc()->TuningAdjust (up);
            }
        }
    if ( button )
        Lfo.Range(up);
    else
        {
        if ( up )
            ClearEntryRed = XlMap[48].Index;
        else
            ClearEntryRed = XlMap[49].Index;
        }
    }

//#######################################################################
void SYNTH_FRONT_C::SetLevelLFO (uint8_t data)
    {
    Lfo.Level (data * PERS_SCALER);
    }

//#######################################################################
void  SYNTH_FRONT_C::NoiseFilter (uint8_t bit, bool state)
    {
    uint8_t bits = 0;

    if ( state )
        NoiseFilterBits[bit] = true;
    else
        NoiseFilterBits[bit] = false;

    if ( NoiseFilterBits[0] )
        bits |= 1;
    if ( NoiseFilterBits[1] )
        bits |= 2;
    for ( int z = 0;  z < (CHAN_COUNT / 2);  z++ )
        pChan[z * 2]->pNoise ()->FilterSelect (bits);
    I2cDevices.UpdateDigital ();
    }

//#######################################################################
void SYNTH_FRONT_C::NoiseColor (bool data)
    {
    I2cDevices.DigitalOut (NoiseColorDev, data);
    I2cDevices.UpdateDigital ();
    if ( !data )
        ClearEntryRedL = XlMap[38].Index;
    }

//#######################################################################
void SYNTH_FRONT_C::SetNoiseFilterMin (uint8_t data)
    {
    if ( SetTuning )
        TuningLevel[ENVELOPE_COUNT - 1] = data * MIDI_MULTIPLIER;

    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < CHAN_COUNT;  z++ )
        pChan[z]->pNoise ()->SetBaseLevel (NOISE_N::SELECT::VCF, val);
    }

//#######################################################################
void SYNTH_FRONT_C::SetNoiseFilterMax (uint8_t data)
    {
    }

//#####################################################################
void SYNTH_FRONT_C::DisplayUpdate ()
    {
    uint8_t zd;

    if ( SetTuning )
        DisplayMessage.Page(DISP_MESSAGE_N::PAGE_C::PAGE_TUNING);
    else
        DisplayMessage.Page(DISP_MESSAGE_N::PAGE_C::PAGE_OSC);
    delay (200);
    for ( uint8_t z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        DisplayMessage.Selected (z, SelectedEnvelope[z]);
        DisplayMessage.MaxLevel (z, MidiAdsr[z].MaxLevel);
        DisplayMessage.AttackTime (z, MidiAdsr[z].AttackTime);
        DisplayMessage.DecayTime (z, MidiAdsr[z].DecayTime);
        DisplayMessage.SustainTime (z, MidiAdsr[z].SustainTime);
        DisplayMessage.ReleaseTime (z, MidiAdsr[z].ReleaseTime);
        DisplayMessage.SustainLevel (z, MidiAdsr[z].SustainLevel);
        }
    DisplayMessage.SawtoothDirection (this->SawToothDirection);
    DisplayMessage.PulseWidth (this->PulseWidth);
    }

//#######################################################################
void SYNTH_FRONT_C::SaveAllSettings ()
    {
    for ( int z = 0;  z < CHAN_COUNT;  z++ )
        Settings.PutOscBank (z, pChan[z]->pOsc ()->GetBankAddr ());
    }



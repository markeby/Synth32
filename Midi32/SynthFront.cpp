//#######################################################################
// Module:     SyntFront.ino
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include <UHS2-MIDI.h>

#include <SynthCommon.h>
#include <DispMessages.h>
#include "Osc.h"
#include "LFOosc.h"
#include "Noise.h"
#include "SynthChannel.h"
#include "SynthFront.h"
#include "Debug.h"
static const char* Label = "TOP";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,0,args);}}

//#define TOGGLE          // use if channel select switches are to be alternate action

using namespace SYNTH_FRONT;

namespace ___StuffForThisModuleOnly___
    {
    static USB Usb;
    UHS2MIDI_CREATE_INSTANCE(&Usb, MIDI_PORT, Midi);

    static SYNTH_CHANNEL_C*    pChan[CHAN_COUNT];
    static LFO_N::SYNTH_LFO_C  Lfo;

    //###################################################################
    inline void SendToDisp32 (DISP_MESSAGE_N::CMD_C status, DISP_MESSAGE_N::EFFECT_C effect, byte value)
        {
        byte snd[4];

        snd[0] = (byte)status;
        snd[1] = (byte)effect;
        snd[2] = value;
        Serial1.write (snd, 3);
        }
    //###################################################################
    inline void SendToDisp32 (DISP_MESSAGE_N::CMD_C status, byte index, DISP_MESSAGE_N::EFFECT_C effect, byte value)
        {
        SendToDisp32 ((DISP_MESSAGE_N::CMD_C)((byte)status | index), effect, value);
        }

    //###################################################################
    static void  FuncKeyDown (byte chan, byte key, byte velocity)
        {
        SynthFront.KeyDown (chan, key, velocity);
        }

    //###################################################################
    static void  FuncKeyUp (byte chan, byte key, byte velocity)
        {
        SynthFront.KeyUp (chan, key, velocity);
        }

    //###################################################################
    static void FuncController (byte chan, byte type, byte value)
        {
        SynthFront.Controller (chan, type, value);
        if ( DebugMidi )
            printf ("Chan %2.2X  type %2.2X  value %2.2X\n", chan, type, value);
        }

    //###################################################################
    static void FuncPitchBend (byte chan, int value)
        {
        SynthFront.PitchBend (chan, value);
        }
}// end namespace USB_MIDI_NOT_OUTSIDE_THIS_MODULE

//#######################################################################
//#######################################################################
using namespace ___StuffForThisModuleOnly___;

//#######################################################################
//#######################################################################

String SYNTH_FRONT_C::Selected ()
    {
    String str = "";

    for ( int z = 0;  z < ENVELOPE_COUNT;  z++ )
        {
        if ( SelectedEnvelope[z] )
            {
            str += SwitchMapArray[z].desc;
            str += "  ";
            }
        }
    return str;
    }

//#####################################################################
SYNTH_FRONT_C::SYNTH_FRONT_C (MIDI_VALUE_MAP* fader_map, MIDI_VALUE_MAP* knob_map, MIDI_SWITCH_MAP* switch_map)
    {
    FaderMap         = fader_map;
    KnobMap          = knob_map;
    SwitchMap        = switch_map;
    DownKey          = 0;
    DownTrigger      = false;
    DownVelocity     = 0;
    UpKey            = 0;
    UpTrigger        = false;
    LastOp           = 0;
    SetTuning        = 0;
    InDispReset      = false;
    }

//#######################################################################
void SYNTH_FRONT_C::Begin (int osc_d_a, int noise_d_a, int noise_dig)
    {
    printf ("\t>>> Usb Midi startup\n");

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

    while ( Usb.Init () == -1 )
        {
        delay (200);
        printf ("Usb init retry!\n");
        }
    delay (200);
    printf ("\t>>> Usb init done!\n");

    printf ("\t>>> Starting echo midi: Port = 1  TX = %d  RX= %d\n", TXD1, RXD1);
    Serial1.begin (115200, SERIAL_8N1, RXD1, TXD1);

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

    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        SelectedEnvelope[z] = false;
    }

//###################################################################
void  SYNTH_FRONT_C::DispMessageHandler (byte cmd)
    {
    switch ( (DISP_MESSAGE_N::CMD_C)cmd )
        {
        case DISP_MESSAGE_N::CMD_C::RESET:
            DispMessageTimer = RunTime;
            InDispReset = true;
            break;
        default:
            break;
        }
    }

//#######################################################################
void SYNTH_FRONT_C::Controller (byte chan, byte type, byte value)
    {
    if ( DebugSynth )
        {
        uint16_t op = (type << sizeof (byte)) | chan;
        if ( op != LastOp )       // issue a newline only if the op has changed
            {
            LastOp = op;
            }
        }
    chan--;
    switch ( type )
        {
        case 0x01:
            // mod wheel
            Lfo.Level (value);
            DBG ("modulation = %f    ", (float)value * (float)PRS_SCALER);
            break;
        case 0x07:          // Faders controls
            if ( FaderMap[chan].CallBack != nullptr )
                {
                FaderMap[chan].CallBack (chan, value);
                DBG ("%s > %f    ", FaderMap[chan].desc, (float)value * (float)PRS_SCALER);
                }
            break;
        case 0x0a:          // Rotatation controls
            if ( KnobMap[chan].CallBack != nullptr )
                {
                KnobMap[chan].CallBack (chan, value);
                DBG ("%s %s > %f    ", Selected ().c_str (), KnobMap[chan].desc, (float)value * (float)TIME_MULT);
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
            if ( DebugMidi )
                DBG (SwitchMap[chan].desc);
            if ( SwitchMap[chan].CallBack != nullptr )
                SwitchMap[chan].CallBack (chan, value);
            break;
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
            chan = type & 0x1F;
            if ( DebugMidi )
                DBG (SwitchMap[chan].desc);
            if ( SwitchMap[chan].CallBack != nullptr )
                SwitchMap[chan].CallBack (chan, value);
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

    Usb.Task ();
    Midi.read ();

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

    if ( Serial1.available () )
        {
        int  z;
        byte buf[8];

        for (z = 0;  Serial1.available() && (z < 3);  z++)
            buf[z] = Serial1.read ();
        if ( z >= 3 )
            {
            DispMessageHandler (buf[2]);
            }
        }
    if ( InDispReset )
        {
        if ( (RunTime - DispMessageTimer) > MILLI_TO_MICRO(1000) )
            {
            InDispReset = false;
            this->DISP32UpdateAll ();
            }
        }
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
        for ( int z = 0;  z < ENVELOPE_COUNT;  z++)
            TuningLevel[z] = 0;
        for ( int zc = 0;  zc < CHAN_COUNT;  zc++ )
            TuningOn[zc] = false;
        }
    SetTuning = true;
    }

//#######################################################################
void SYNTH_FRONT_C::PitchBend (byte ch, int value)
    {
    float scaler = (value + 16384) * BEND_SCALER;
    Lfo.PitchBend (scaler);
    DBG ("Pitch bend > %f", scaler);
    }

//#######################################################################
void SYNTH_FRONT_C::ChannelSetSelect (byte chan, bool state)
    {
#ifdef TOGGLE
    SelectedEnvelope[chan] = !SelectedEnvelope[chan];
#else
    SelectedEnvelope[chan] = state;
#endif
    DBG ("%s %s", SwitchMap[chan].desc,  (SelectedEnvelope[chan]) ? " ON" : " off");
    }

//#####################################################################
void SYNTH_FRONT_C::SetMBaselevel (byte ch, byte data)
    {
    float val = (float)data * PRS_SCALER;
    for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
        {
        if ( ch >= OSC_MIXER_COUNT )
            pChan[zc]->pNoise()->SetMaxLevel (ch - OSC_MIXER_COUNT, val);
        }
    MidiAdsr[ch].BaseLevel = data;
    SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, ch, DISP_MESSAGE_N::EFFECT_C::BASE_VOL, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetMaxLevel (byte ch, byte data)
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
    SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, ch, DISP_MESSAGE_N::EFFECT_C::LIMIT_VOL, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetAttackTime (byte data)
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
            SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, zs, DISP_MESSAGE_N::EFFECT_C::ATTACK_TIME, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetDecayTime (byte data)
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
            SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, zs, DISP_MESSAGE_N::EFFECT_C::DECAY_TIME, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetSustainLevel (byte ch, byte data)
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
    SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, ch, DISP_MESSAGE_N::EFFECT_C::SUSTAIN_VOL, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetSustainTime (byte data)
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

            SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, zs, DISP_MESSAGE_N::EFFECT_C::SUSTAIN_TIME, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetReleaseTime (byte data)
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
            SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, zs, DISP_MESSAGE_N::EFFECT_C::RELEASE_TIME,  data);
            }
        }
    }

//#######################################################################
void SYNTH_FRONT_C::SetReverse (bool data)
    {
    for ( int z = 0;  z < CHAN_COUNT;  z++)
        pChan[z]->pOsc()->SetReverse (data);
    SendToDisp32(DISP_MESSAGE_N::CMD_C::CONTROL, (byte)DISP_MESSAGE_N::SHAPE_C::SAWTOOTH, DISP_MESSAGE_N::EFFECT_C::SAWTOOTH_REVERSE, data);
    }

//#####################################################################
void SYNTH_FRONT_C::DISP32UpdateAll ()
    {
    byte zd;

    SendToDisp32 (DISP_MESSAGE_N::CMD_C::RENDER, DISP_MESSAGE_N::EFFECT_C::INIT, (byte)(DISP_MESSAGE_N::SHAPE_C::ALL));
    for ( byte z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, z, DISP_MESSAGE_N::EFFECT_C::SELECTED, SelectedEnvelope[z]);
        SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, z, DISP_MESSAGE_N::EFFECT_C::LIMIT_VOL, MidiAdsr[z].MaxLevel);
        SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, z, DISP_MESSAGE_N::EFFECT_C::ATTACK_TIME, MidiAdsr[z].AttackTime);
        SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, z, DISP_MESSAGE_N::EFFECT_C::DECAY_TIME, MidiAdsr[z].DecayTime);
        SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, z, DISP_MESSAGE_N::EFFECT_C::SUSTAIN_TIME, MidiAdsr[z].SustainTime);
        SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, z, DISP_MESSAGE_N::EFFECT_C::RELEASE_TIME, MidiAdsr[z].ReleaseTime);
        SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, z, DISP_MESSAGE_N::EFFECT_C::SUSTAIN_VOL, MidiAdsr[z].SustainLevel);
        }
    SendToDisp32 (DISP_MESSAGE_N::CMD_C::RENDER, DISP_MESSAGE_N::EFFECT_C::RENDER_ADSR, (byte)(DISP_MESSAGE_N::SHAPE_C::ALL));
    }


//#####################################################################
void SYNTH_FRONT_C::SelectWaveLFO (byte ch, byte state)
    {
    Lfo.Select(ch, state);
    }

//#####################################################################
void SYNTH_FRONT_C::FreqSelectLFO (byte ch, byte data)
    {
    Lfo.SetFreq (data);
    }

//#######################################################################
void SYNTH_FRONT_C::LFOrange (bool up)
    {
    Lfo.Range (up);
    }

//#######################################################################
void SYNTH_FRONT_C::SetLevelLFO (byte data)
    {
    Lfo.Level (data * PERS_SCALER);
    }

//#######################################################################
void  SYNTH_FRONT_C::NoiseFilter (byte bit, bool state)
    {
    byte bits = 0;

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
void SYNTH_FRONT_C::NoiseColor (byte val)
    {
    I2cDevices.DigitalOut (NoiseColorDev, val);
    I2cDevices.UpdateDigital ();
    }


//#######################################################################
void SYNTH_FRONT_C::SetNoiseFilterMin (byte data)
    {
    if ( SetTuning )
        TuningLevel[ENVELOPE_COUNT - 1] = data * MIDI_MULTIPLIER;

    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < CHAN_COUNT;  z++ )
        pChan[z]->pNoise ()->SetBaseLevel (NOISE_N::SELECT::VCF, val);
    }

//#######################################################################
void SYNTH_FRONT_C::SetNoiseFilterMax (byte data)
    {
    }










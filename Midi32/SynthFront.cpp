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
#include "SynthChannel.h"
#include "SynthFront.h"

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

    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        if ( SelectWaveShapeVCO[z] )
            {
            str += SwitchMapArray[z].desc;
            str += "  ";
            }
        }
    return str;
    }

//#####################################################################
SYNTH_FRONT_C::SYNTH_FRONT_C (int first_device, MIDI_VALUE_MAP* fader_map, MIDI_VALUE_MAP* knob_map, MIDI_SWITCH_MAP* switch_map)
    {
    SetDeviceIndex   = first_device;
    FaderMap         = fader_map;
    KnobMap          = knob_map;
    SwitchMap        = switch_map;
    DownKey          = 0;
    DownTrigger      = false;
    DownVelocity     = 0;
    UpKey            = 0;
    UpTrigger        = false;
    LastOp           = 0;
    InTuning         = 0;
    SetTuning        = 0;
    InDispReset      = false;
    }

//#######################################################################
void SYNTH_FRONT_C::Begin ()
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
        pChan[z] = new SYNTH_CHANNEL_C (z, SetDeviceIndex);
        SetDeviceIndex += 8;
        }
    Lfo.Begin (0, SetDeviceIndex);
    SetDeviceIndex += 8;

    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        SelectWaveShapeVCO[z] = false;
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
void SYNTH_FRONT_C::OscChannelSelect (byte chan, bool state)
    {
    SelectWaveShapeVCO[chan] = !SelectWaveShapeVCO[chan];
    if ( DebugSynth )
        Serial << endl << SwitchMap[chan].desc << ((SelectWaveShapeVCO[chan]) ? " ON" : " off");
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

            if ( DebugSynth )
                printf ("\n[s]  Key up > %d", UpKey);
            for ( int z = 0;  z < CHAN_COUNT;  z++ )
                if ( pChan[z]->NoteClear (UpKey) )
                    {
                    if ( DebugSynth )
                        printf ("  Osc > %d", z);
                    }
            if ( DebugSynth )
                printf("\n");
            }

        if ( DownTrigger )
            {
            for ( int z = 0;  z < CHAN_COUNT;  z++ )
                {
                SYNTH_CHANNEL_C& ch = *(pChan[z]);
                if ( ch.Active () < 1 )
                    {
                    doit = z;
                    break;
                    }
                else
                    {
                    if ( oldest < 0 )
                        oldest = z;
                    else
                        {
                        if ( ch.Active () > pChan[oldest]->Active () )
                            oldest = z;
                        }
                    }
                }
            if ( doit < 0 )
                doit = oldest;
            DownTrigger = false;
            pChan[doit]->NoteSet (DownKey, DownVelocity);
            if ( DebugSynth )
                printf ("\n[s]  Key down > %d   Velocity > %d  Osc > %d", DownKey, DownVelocity, doit);
            }

        for ( int z = 0;  z < CHAN_COUNT;  z++ )             // Process all audio channels
            pChan[z]->Loop ();
        }
    else
        {
        if ( SetTuning != InTuning )
            {
            InTuning = SetTuning;
            for (int z = 0;  z < CHAN_COUNT;  z++)
                pChan[z]->Clear ();
            DownTrigger = true;
            }
        if ( DownTrigger )
            {
            pChan[SetTuning - 1]->NoteSet (DownKey, DownVelocity);
            DownTrigger = false;
            }
        pChan[SetTuning - 1]->SetTuning ();
        }

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
void SYNTH_FRONT_C::Controller (byte chan, byte type, byte value)
    {
    float   scaler;

    if ( DebugSynth )
        {
        uint16_t op = (type << sizeof (byte)) | chan;
        if ( op != LastOp )       // issue a newline only if the op has changed
            {
            printf ("\n");
            LastOp = op;
            }
        }
    chan--;
    switch ( type )
        {
        case 0x01:
            // mod wheel
            Lfo.Level (value);
            if ( DebugSynth )
                {
                printf ("\r[s] modulation = %f    ", scaler);
                fflush(stdout);
                }
            break;
        case 0x07:          // Faders controls
            if ( FaderMap[chan].CallBack != nullptr )
                {
                FaderMap[chan].CallBack (chan, value);
                if ( DebugSynth )
                    {
                    printf ("\r[s] %s > %f    ", FaderMap[chan].desc, scaler);
                    fflush(stdout);
                    }
                }
            break;
        case 0x0a:          // Rotatation controls
            if ( KnobMap[chan].CallBack != nullptr )
                {
                KnobMap[chan].CallBack (chan, value);
                if ( DebugSynth )
                    {
                    printf ("\r[s] %s %s > %f    ", Selected ().c_str (), KnobMap[chan].desc, scaler);
                    fflush(stdout);
                    }
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
            if ( SwitchMap[chan].CallBack != nullptr )
                SwitchMap[chan].CallBack (chan, value);
            break;
        default:
            break;
        }

    }

//#######################################################################
void SYNTH_FRONT_C::PitchBend (byte ch, int value)
    {
    float scaler = (value + 16384) * BEND_SCALER;
    Lfo.PitchBend (scaler);
    if ( DebugSynth )
        Serial << "\r[s] Pitch bend > " << scaler << "    ";
    }

//#####################################################################
void SYNTH_FRONT_C::SetOscMaxLevel (byte ch, byte data)
    {
    float val = (float)data * PRS_SCALER;
    for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
        pChan[zc]->pOsc()->SetMaxLevel (ch, val);
    MidiAdsr[ch].MaxLevel = data;
    SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, ch, DISP_MESSAGE_N::EFFECT_C::LIMIT_VOL, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetOscAttackTime (byte data)
    {
    float dtime = data * TIME_MULT;
    for ( int zs = 0;  zs < OSC_MIXER_COUNT;  zs++ )
        {
        if ( SelectWaveShapeVCO[zs] )
            {
            MidiAdsr[zs].AttackTime = data;
            for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
                pChan[zc]->pOsc()->SetAttackTime (zs, dtime);
            SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, zs, DISP_MESSAGE_N::EFFECT_C::ATTACK_TIME, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetOscDecayTime (byte data)
    {
    float dtime = data * TIME_MULT;
    for ( int zs = 0;  zs < OSC_MIXER_COUNT;  zs++ )
        {
        if ( SelectWaveShapeVCO[zs] )
            {
            MidiAdsr[zs].DecayTime = data;
            for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
                pChan[zc]->pOsc()->SetDecayTime (zs, dtime);
            SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, zs, DISP_MESSAGE_N::EFFECT_C::DECAY_TIME, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetOscSustainLevel (byte ch, byte data)
    {
    float val = (float)data * PRS_SCALER;
    for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
        pChan[zc]->pOsc()->SetSustainLevel (ch, val);
    MidiAdsr[ch].SustainLevel = data;
    SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, ch, DISP_MESSAGE_N::EFFECT_C::SUSTAIN_VOL, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetOscSustainTime (byte data)
    {
    float dtime;

    if ( data == 0 )
        dtime = -1;
    else
        float dtime = data * TIME_MULT;

    for ( int zs = 0;  zs < OSC_MIXER_COUNT;  zs++ )
        {
        if ( SelectWaveShapeVCO[zs] )
            {
            MidiAdsr[zs].SustainTime = data;
            for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
                pChan[zc]->pOsc()->SetSustainTime (zs, dtime);
            SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, zs, DISP_MESSAGE_N::EFFECT_C::SUSTAIN_TIME, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetOscReleaseTime (byte data)
    {
    float dtime = data * TIME_MULT;
    for ( int zs = 0;  zs < OSC_MIXER_COUNT;  zs++ )
        {
        if ( SelectWaveShapeVCO[zs] )
            {
            MidiAdsr[zs].ReleaseTime = data;
            for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
                pChan[zc]->pOsc()->SetReleaseTime (zs, dtime);
            SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, zs, DISP_MESSAGE_N::EFFECT_C::RELEASE_TIME,  data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::DISP32UpdateAll ()
    {
    byte zd;

    SendToDisp32 (DISP_MESSAGE_N::CMD_C::RENDER, DISP_MESSAGE_N::EFFECT_C::INIT, (byte)(DISP_MESSAGE_N::SHAPE_C::ALL));
    for ( byte z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        SendToDisp32 (DISP_MESSAGE_N::CMD_C::CONTROL, z, DISP_MESSAGE_N::EFFECT_C::SELECTED, SelectWaveShapeVCO[z]);
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


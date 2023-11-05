//#######################################################################
// Module:     SyntFront.ino
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include <UHS2-MIDI.h>

#include "Osc.h"
#include "LFOosc.h"
#include "SynthChannel.h"
#include "SynthFront.h"

namespace ___StuffForThisModuleOnly___
    {
    static USB Usb;
    UHS2MIDI_CREATE_INSTANCE(&Usb, MIDI_PORT, Midi);

    static SYNTH_CHANNEL_C*    pChan[CHAN_COUNT];
    static LFO_N::SYNTH_LFO_C  Lfo;

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
        SynthFront.Controller (type, chan, value);
        }

    //###################################################################
    static void FuncPitchBend (byte chan, int value)
        {
        SynthFront.PitchBend (chan, value);
        }

}// end namespace USB_MIDI_NOT_OUTSIDE_THIS_MODULE

//#######################################################################
//#######################################################################
using namespace SYNTH_FRONT;
using namespace ___StuffForThisModuleOnly___;

//#######################################################################
//#######################################################################

void SYNTH_FRONT_C::SendControl ()
    {
    byte z;

    z = Midi.getInputChannel ();
//    z = (z + 1) % 5;
    printf ("\n\nMIDI Test.... %d   /n/n/n", z);
    Midi.setInputChannel (z);
    }






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
SYNTH_FRONT_C::SYNTH_FRONT_C (int first_device, MIDI_VALUE_MAP* fader_map, MIDI_VALUE_MAP* knob_map, MIDI_VALUE_MAP* pitch_map, MIDI_SWITCH_MAP* switch_map)
    {
    SetDeviceIndex = first_device;
    FaderMap       = fader_map;
    KnobMap        = knob_map;
    SwitchMap      = switch_map;
    PitchMap       = pitch_map;

    DownKey        = 0;
    DownTrigger    = false;
    DownVelocity   = 0;
    UpKey          = 0;
    UpTrigger      = false;
    LastOp         = 0;
    InTuning       = 0;
    SetTuning      = 0;
    }

//#######################################################################
void SYNTH_FRONT_C::Begin ()
    {
    printf ("\t>>> Usb Midi startup\n");

//  MIDI.setHandleMessage              (void (*fptr)(const MidiMessage&));
//  MIDI.setHandleError                (ErrorCallback fptr);
    Midi.setHandleNoteOn               (FuncKeyDown);
    Midi.setHandleNoteOff              (FuncKeyUp);
//  MIDI.setHandleAfterTouchPoly       (AfterTouchPolyCallback fptr);
    Midi.setHandleControlChange        (FuncController);
//  MIDI.setHandleProgramChange        (ProgramChangeCallback fptr);
//  MIDI.setHandleAfterTouchChannel    (AfterTouchChannelCallback fptr);
    Midi.setHandlePitchBend            (FuncPitchBend);
//  MIDI.setHandleSystemExclusive      (SystemExclusiveCallback fptr);
//  MIDI.setHandleTimeCodeQuarterFrame (TimeCodeQuarterFrameCallback fptr);
//  MIDI.setHandleSongPosition         (SongPositionCallback fptr);
//  MIDI.setHandleSongSelect           (SongSelectCallback fptr);
//  MIDI.setHandleTuneRequest          (TuneRequestCallback fptr);
//  MIDI.setHandleClock                (ClockCallback fptr);
//  MIDI.setHandleStart                (StartCallback fptr);
//  MIDI.setHandleTick                 (TickCallback fptr);
//  MIDI.setHandleContinue             (ContinueCallback fptr);
//  MIDI.setHandleStop                 (StopCallback fptr);
//  MIDI.setHandleActiveSensing        (ActiveSensingCallback fptr);
//  MIDI.setHandleSystemReset          (SystemResetCallback fptr);

    while ( Usb.Init () == -1 )
        {
        delay (200);
        printf ("Usb init retry!\n");
        }
    delay (200);
    printf ("\t>>> Usb init done!\n");

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

//#######################################################################
void SYNTH_FRONT_C::ChannelSelect (uint8_t chan, bool state)
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
    }

//#######################################################################
void SYNTH_FRONT_C::Controller (byte type, byte chan, byte value)
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
            if ( PitchMap[0].CallBack != nullptr )
                {
                scaler = value * PitchMap[chan].Scaler;
                PitchMap[0].CallBack (0, scaler);
                if ( DebugSynth )
                    printf ("\r[s] %s > %f    ", PitchMap[0].desc, scaler);
                }
            break;
        case 0x07:          // Faders controls
            if ( FaderMap[chan].CallBack != nullptr )
                {
                scaler = value * FaderMap[chan].Scaler;
                FaderMap[chan].CallBack (chan, scaler);
                if ( DebugSynth )
                    printf ("\r[s] %s > %f    ", FaderMap[chan].desc, scaler);
                }
            break;
        case 0x0a:          // Rotatation controls
            if ( KnobMap[chan].CallBack != nullptr )
                {
                scaler = value * KnobMap[chan].Scaler;
                KnobMap[chan].CallBack (chan, scaler);
                if ( DebugSynth )
                    printf ("\r[s] %s %s > %f    ", Selected ().c_str (), KnobMap[chan].desc, scaler);
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
void SYNTH_FRONT_C::PitchBend (byte chan, int value)
    {
    if ( PitchMap[1].CallBack != nullptr )
        {
        float scaler = (value + 16384) * PitchMap[1].Scaler;
//                scaler = ( scaler < 0.8 ) ? 0.0 : scaler;
        PitchMap[1].CallBack (1, scaler);
        if ( DebugSynth )
            Serial << "\r[s] Pitch bend > " << scaler << "    ";
        }
    }

//#######################################################################
void SYNTH_FRONT_C::ShortMsg (uint8_t *data)
    {
    float   scaler;
    uint8_t chan = data[0] & 0x0F;

    if ( data[0] == 99 )                // Pad pressure

        return;

    switch ( data[0] & 0xF0 )
        {
        case 0xe0:      /* pitchbend */
            if ( PitchMap[1].CallBack != nullptr )
                {
                scaler = ((((data[2] - 64) << 8) | data[1]) + 16384) * PitchMap[1].Scaler;
//                scaler = ( scaler < 0.8 ) ? 0.0 : scaler;
                PitchMap[1].CallBack (1, scaler);
                if ( DebugSynth )
                    Serial << "\r[s] Pitch bend > " << scaler << "    ";
                }
            break;
        default:
            break;
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetMaxLevel (uint8_t wave, float data)
    {
    for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
        pChan[zc]->SetMaxLevel (wave, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetAttack ( float data)
    {
    for ( int zs = 0;  zs < OSC_MIXER_COUNT;  zs++ )
        if ( SelectWaveShapeVCO[zs] )
            for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
                pChan[zc]->SetAttack (zs, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetDecay (float data)
    {
    for ( int zs = 0;  zs < OSC_MIXER_COUNT;  zs++ )
        if ( SelectWaveShapeVCO[zs] )
            for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
                pChan[zc]->SetDecay (zs, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetSustainLevel (uint8_t ch, float data)
    {
    for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
        pChan[zc]->SetSustainLevel (ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetSustainTime (float data)
    {
    for ( int zs = 0;  zs < OSC_MIXER_COUNT;  zs++ )
        if ( SelectWaveShapeVCO[zs] )
            for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
                pChan[zc]->SetSustainTime (zs, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetRelease (float data)
    {
    for ( int zs = 0;  zs < OSC_MIXER_COUNT;  zs++ )
        if ( SelectWaveShapeVCO[zs] )
            for ( int zc = 0;  zc < CHAN_COUNT;  zc++)
                pChan[zc]->SetRelease (zs, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SelectWaveLFO (uint8_t ch, uint8_t state)
    {
    Lfo.Select(ch, state);
    }

//#####################################################################
void SYNTH_FRONT_C::FreqSelectLFO (uint8_t ch, float val)
    {
    Lfo.SetFreq (val);
    }

//#####################################################################
void SYNTH_FRONT_C::PitchBend (float val)
    {
    Lfo.PitchBend (val);
    }

//#######################################################################
void SYNTH_FRONT_C::LFOrange (bool up)
    {
    Lfo.Range (up);
    }

//#######################################################################
void SYNTH_FRONT_C::SetLevelLFO (float data)
    {
    Lfo.Level (data);
    }


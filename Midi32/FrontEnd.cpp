//#######################################################################
// Module:     SyntFront.ino
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include <UHS2-MIDI.h>

#include "../Common/SynthCommon.h"
#include "../Common/DispMessages.h"
#include "Osc.h"
#include "LFOosc.h"
#include "Noise.h"
#include "FrontEnd.h"
#include "SynthChannel.h"
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
    FaderMap            = fader_map;
    KnobMap             = knob_map;
    SwitchMap           = switch_map;
    DownKey             = 0;
    DownTrigger         = false;
    DownVelocity        = 0;
    UpKey               = 0;
    UpTrigger           = false;
    LastOp              = 0;
    SetTuning           = false;
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
    if ( DebugSynth )
        {
        uint16_t op = (type << sizeof (uint8_t)) | chan;
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
//            Lfo.Level (value);
            SineWave.Multiplier ((float)value * PRS_SCALER * 0.4);
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
    DBG ("%s %s", SwitchMap[chan].desc,  (SelectedEnvelope[chan]) ? " ON" : " off");
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
void SYNTH_FRONT_C::SelectWaveLFO (uint8_t ch, uint8_t state)
    {
//    Lfo.Select(ch, state);
    for ( int z = 0;  z < CHAN_COUNT;  z++)
        pChan[z]->pOsc()->SoftLFO (ch, state);
    }

//#####################################################################
void SYNTH_FRONT_C::FreqSelectLFO (uint8_t ch, uint8_t data)
    {
    Lfo.SetFreq (data);
    }

//#######################################################################
void SYNTH_FRONT_C::LFOrange (bool up)
    {
    if ( SetTuning )
        {
        for ( int zc = 0;  zc < CHAN_COUNT;  zc++ )
            {
            if ( TuningOn[zc] )
                pChan[zc]->pOsc()->TuningAdjust (up);
            }
        }
    Lfo.Range(up);
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
void SYNTH_FRONT_C::NoiseColor (uint8_t val)
    {
    I2cDevices.DigitalOut (NoiseColorDev, val);
    I2cDevices.UpdateDigital ();
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










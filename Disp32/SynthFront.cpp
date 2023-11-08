//#######################################################################
// Module:     SyntFront.ino
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include <MIDI.h>

#include "config.h"
#include "SynthFront.h"

namespace ___StuffForThisModuleOnly___
    {
    MIDI_CREATE_INSTANCE (HardwareSerial, Serial1,  Midi);

    //###################################################################
    static void  FuncKeyDown (byte chan, byte key, byte velocity)
        {
        }

    //###################################################################
    static void  FuncKeyUp (byte chan, byte key, byte velocity)
        {
        }

    //###################################################################
    static void FuncController (byte chan, byte type, byte value)
        {
        SynthFront.Controller (chan, type, value);
        }

    //###################################################################
    static void FuncPitchBend (byte chan, int value)
        {
        }

}// end namespace USB_MIDI_NOT_OUTSIDE_THIS_MODULE

//#######################################################################
//#######################################################################
using namespace SYNTH_FRONT;
using namespace ___StuffForThisModuleOnly___;

//#######################################################################
//#######################################################################

void SYNTH_FRONT_C::SendReset ()
    {
    static const byte rst[] = {0xFF, 0xFF};

    Serial1.write (rst, 2);
    }

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
SYNTH_FRONT_C::SYNTH_FRONT_C (int first_device, MIDI_VALUE_MAP* fader_map, MIDI_VALUE_MAP* knob_map, MIDI_VALUE_MAP* pitch_map, MIDI_SWITCH_MAP* switch_map)
    {
    SetDeviceIndex = first_device;
    FaderMap       = fader_map;
    KnobMap        = knob_map;
    SwitchMap      = switch_map;
    PitchMap       = pitch_map;
    }

//#######################################################################
void SYNTH_FRONT_C::Begin ()
    {
    printf ("\t>>> Starting serial Midi32 echo port 1...  TX = %d  RX= %d\n", TXD1, RXD1);
    Serial1.begin (115200, SERIAL_8N1, RXD1, TXD1);
    }

//#######################################################################
void SYNTH_FRONT_C::ChannelSelect (uint8_t chan, bool state)
    {
    }

//#######################################################################
void SYNTH_FRONT_C::Loop ()
    {
    int  z;
    char buf[8];

    if ( Serial1.available () )
        {
        for ( z = 0;  Serial1.available () && (z < 3);  z++ )
            {
            buf[z] = Serial1.read ();
            }
        if ( z == 3 )
            {
            byte chan = buf[0] & 0x0F;
            byte status = buf[0] & 0xF0;
            byte type = buf[1];
            byte value = buf[2];

            if ( DebugMidi )
                    printf("\n%2.2X  %2.2X  %2.2X\n", buf[0], type, value);

            switch ( status )
                {
                case ControlChange:
                    this->Controller (chan, type, value);
                    break;
                default:
                    break;
                }
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
            if ( PitchMap[0].CallBack != nullptr )
                {
                scaler = value * PitchMap[chan].Scaler;
                PitchMap[0].CallBack (0, scaler);
                if ( DebugSynth )
                    {
                    printf ("\r[s] %s > %f    ", PitchMap[0].desc, scaler);
                    fflush(stdout);
                    }
                }
            break;
        case 0x07:          // Faders controls
            if ( FaderMap[chan].CallBack != nullptr )
                {
                scaler = value * FaderMap[chan].Scaler;
                FaderMap[chan].CallBack (chan, scaler);
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
                scaler = value * KnobMap[chan].Scaler;
                KnobMap[chan].CallBack (chan, scaler);
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
void SYNTH_FRONT_C::PitchBend (byte chan, int value)
    {
    }

//#####################################################################
void SYNTH_FRONT_C::SetMaxLevel (uint8_t wave, float data)
    {
    }

//#####################################################################
void SYNTH_FRONT_C::SetAttack ( float data)
    {
    }

//#####################################################################
void SYNTH_FRONT_C::SetDecay (float data)
    {
    }

//#####################################################################
void SYNTH_FRONT_C::SetSustainLevel (uint8_t ch, float data)
    {
    }

//#####################################################################
void SYNTH_FRONT_C::SetSustainTime (float data)
    {
    }

//#####################################################################
void SYNTH_FRONT_C::SetRelease (float data)
    {
    }

//#####################################################################
void SYNTH_FRONT_C::SelectWaveLFO (uint8_t ch, uint8_t state)
    {
    }

//#####################################################################
void SYNTH_FRONT_C::FreqSelectLFO (uint8_t ch, float val)
    {
    }

//#####################################################################
void SYNTH_FRONT_C::PitchBend (float val)
    {
    }

//#######################################################################
void SYNTH_FRONT_C::LFOrange (bool up)
    {
    }

//#######################################################################
void SYNTH_FRONT_C::SetLevelLFO (float data)
    {
    }


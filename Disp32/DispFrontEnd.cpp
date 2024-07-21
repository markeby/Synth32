//#######################################################################
// Module:     SyntFront.cpp
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>

#include "config.h"
#include "../Common/SynthCommon.h"
#include "DispFrontEnd.h"
#include "Graphics.h"
#include "../Common/DispMessages.h"
#include "SynthData.h"
#include <Wire.h>

using namespace DISP_MESSAGE_N;

//#######################################################################
void DISP_FRONT_END_C::SendReset ()
    {
    static const CMD_C rst[] = {CMD_C::RESET, CMD_C::RESET, CMD_C::RESET};

    Serial1.write ((byte*)rst, 3);
    }

//#####################################################################
DISP_FRONT_END_C::DISP_FRONT_END_C ()
    {
    }

//#######################################################################
void DISP_FRONT_END_C::Begin ()
    {
    printf ("\t>>> Starting serial echo port 1...  TX = %d  RX= %d\n", TXDM1, RXDM1);
//    Serial1.begin (115200, SERIAL_8N1, RXDM1, TXDM1);
//    Wire.begin(15, 16);
    }

//#######################################################################
void DISP_FRONT_END_C::ChannelSelect (uint8_t chan, bool state)
    {
    }

//#######################################################################
void DISP_FRONT_END_C::Display (byte ch)
    {
    }

//#######################################################################
void DISP_FRONT_END_C::Loop ()
    {
    int  z;
    byte buf[8];

    if ( Serial1.available () )
        {
        for ( z = 0;  Serial1.available () && (z < 3);  z++ )
            {
            buf[z] = Serial1.read ();
            }
        if ( z == 3 )
            {
            byte status = buf[0] & 0xF0;
            byte ch     = buf[0] & 0x0F;
            byte effect = buf[1];
            byte value  = buf[2];

            if ( DebugInterface )
                printf("\n[i]status = %2.2X  channel = %2.2X  effect = %2.2X  value = %2.2X  Command: ", status, ch, effect, value);

            switch ( (CMD_C)status )
                {
                case CMD_C::CONTROL:
                    this->Controller (ch, effect, value);
                    break;
                case CMD_C::RENDER:
                    switch ( (EFFECT_C)effect )
                        {
                        case EFFECT_C::INIT:
                            if ( DebugInterface )
                                printf("Initialize phase");
                            break;
                        case EFFECT_C::RENDER_ADSR:
                            if ( DebugInterface )
                                printf("Render now\n");
                            break;
                        default:
                            break;
                        }

                default:
                    break;
                }
            }
        }
    }

//#######################################################################
void DISP_FRONT_END_C::Controller (byte ch, byte effect, byte value)
    {
    switch ( (EFFECT_C)effect )
        {
        case EFFECT_C::SELECTED:
//            SynthD.SetSelected (ch, (bool)value);
            if ( DebugInterface )
                printf("Select");
            break;
        case EFFECT_C::LIMIT_VOL:
//            SynthD.SetMaxLevel (ch, value);
            if ( DebugInterface )
                printf("Max Level");
            break;
        case EFFECT_C::ATTACK_TIME:
//            SynthD.SetAttackTime (ch, value);
            if ( DebugInterface )
                printf("Attack time");
            break;
        case EFFECT_C::DECAY_TIME:
//            SynthD.SetDecayTime (ch, value);
            if ( DebugInterface )
                printf("Decay time");
            break;
        case EFFECT_C::SUSTAIN_TIME:
//            SynthD.SetSustainTime (ch, value);
            if ( DebugInterface )
                printf("Sustain time");
            break;
        case EFFECT_C::RELEASE_TIME:
 //           SynthD.SetReleaseTime (ch, value);
            if ( DebugInterface )
                printf("Release time");
            break;
        case EFFECT_C::SUSTAIN_VOL:
 //           SynthD.SetSustainLevel (ch, value);
            if ( DebugInterface )
                printf("Sustain level");
            break;
        case EFFECT_C::SAWTOOTH_REVERSE:
//            SynthD.SetSawtoothDir (value);
            if ( DebugInterface )
                printf("Sawtooth Direction");
            break;
        default:
            break;
        }
    this->Display (ch);
    }


//#######################################################################
// Module:     SyntFront.cpp
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>

#include "config.h"
#include <SynthCommon.h>
#include "DispFrontEnd.h"
#include "Graphics.h"
#include <DispMessages.h>
#include "SynthData.h"

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
    DisplayInit = true;
    }

//#######################################################################
void DISP_FRONT_END_C::Begin ()
    {
    printf ("\t>>> Starting serial echo port 1...  TX = %d  RX= %d\n", TXD1, RXD1);
    Serial1.begin (115200, SERIAL_8N1, RXD1, TXD1);
    }

//#######################################################################
void DISP_FRONT_END_C::ChannelSelect (uint8_t chan, bool state)
    {
    }

//#######################################################################
void DISP_FRONT_END_C::Display (byte ch)
    {
    if ( !DisplayInit )
        Graphics.ShowADSR (ch);
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
                            DisplayInit = true;
                            break;
                        case EFFECT_C::RENDER_ADSR:
                            DisplayInit = false;
                            if ( DebugInterface )
                                printf("Render now\n");
                            Graphics.ShowADSR (value);
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
            SynthD.SetSelected (ch, (bool)value);
            if ( DebugInterface )
                printf("Select");
            this->Display (ch);
            break;
        case EFFECT_C::LIMIT_VOL:
            SynthD.SetLimitLevel (ch, value);
            if ( DebugInterface )
                printf("Max Level");
            this->Display (ch);
            break;
        case EFFECT_C::ATTACK_TIME:
            SynthD.SetAttackTime (ch, value);
            if ( DebugInterface )
                printf("Attack time");
            this->Display (ch);
            break;
        case EFFECT_C::DECAY_TIME:
            SynthD.SetDecayTime (ch, value);
            if ( DebugInterface )
                printf("Decay time");
            this->Display (ch);
            break;
        case EFFECT_C::SUSTAIN_TIME:
            SynthD.SetSustainTime (ch, value);
            if ( DebugInterface )
                printf("Sustain time");
            this->Display (ch);
            break;
        case EFFECT_C::RELEASE_TIME:
            SynthD.SetReleaseTime (ch, value);
            if ( DebugInterface )
                printf("Release time");
            this->Display (ch);
            break;
        case EFFECT_C::SUSTAIN_VOL:
            SynthD.SetSustainLevel (ch, value);
            if ( DebugInterface )
                printf("Sustain level");
            this->Display (ch);
            break;
        default:
            break;
        }

    }


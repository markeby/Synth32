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
            byte wave   = buf[0] & 0x0F;
            byte effect = buf[1];
            byte value  = buf[2];

            if ( DebugInterface )
                printf("[i] status %2.2X  wave %2.2X  effect %2.2X  value %2.2X\n", status, wave, effect, value);

            switch ( (CMD_C)status )
                {
                case CMD_C::CONTROL:
                    this->Controller (wave, effect, value);
                    break;
                case CMD_C::RENDER:
                    switch ( (EFFECT_C)effect )
                        {
                        case EFFECT_C::RENDER_ADSR:
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
void DISP_FRONT_END_C::Controller (byte wave, byte effect, byte value)
    {
    switch ( (EFFECT_C)effect )
        {
        case EFFECT_C::SELECTED:
            SynthData.SetSelected (wave, (bool)value);
            break;
        case EFFECT_C::LIMIT_VOL:
            SynthData.SetLimitLevel (wave, value * PERS_SCALER);
            break;
        case EFFECT_C::ATTACK_TIME:
            SynthData.SetAttackTime (wave, value * TIME_MULT);
            break;
        case EFFECT_C::DECAY_TIME:
            SynthData.SetDecayTime (wave, value * TIME_MULT);
            break;
        case EFFECT_C::SUSTAIN_TIME:
            SynthData.SetSustainTime (wave, value * TIME_MULT);
            break;
        case EFFECT_C::RELEASE_TIME:
            SynthData.SetReleaseTime (wave, value * TIME_MULT);
            break;
        case EFFECT_C::SUSTAIN_VOL:
            SynthData.SetSustainLevel (wave, value * PERS_SCALER);
            break;
        default:
            break;
        }

    }


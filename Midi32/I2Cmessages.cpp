//#######################################################################
// Module:     SyntFront.ino
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "I2Cmessages.h"
#include "Debug.h"

static const char* LabelD = "D";
#define DBGM(args...) {if(DebugDisp){ DebugMsg(LabelD,DEBUG_NO_INDEX,args);}}

//#######################################################################
//#######################################################################

I2C_MESSAGE_C::I2C_MESSAGE_C () : Ready (false)
    {
    }

//#######################################################################
void I2C_MESSAGE_C::Begin (uint8_t display, uint8_t sda, uint8_t scl)
    {
    Paused = false;
    this->DisplayAddress = display;
    Wire1.begin (sda, scl, 400000UL);   // Clock at 400kHz
    this->Ready = true;
    }

//###################################################################
void I2C_MESSAGE_C::SendComplete (byte length)
    {
    Wire1.beginTransmission (DisplayAddress);
    int written = Wire1.write ((const uint8_t*)SendBuffer, length);
    Wire1.endTransmission();
    if ( written != length )
        printf("[DISP] ### ERROR ### Attempt to send %d byte message but shows %d", length, written);
    }

//###################################################################
void I2C_MESSAGE_C::SendVCA (uint8_t channel, DISP_MESSAGE_N::EFFECT_C effect, uint16_t value)
    {
    if ( this->Ready )
        {
        this->Throttle ();
        this->SendBuffer[0] = (uint8_t)DISP_MESSAGE_N::CMD_C::CTRL_VCA;
        this->SendBuffer[1] = channel;
        this->SendBuffer[2] = (uint8_t)effect;
        this->SendBuffer[3] = (uint8_t)(value >> 8);
        this->SendBuffer[4] = (uint8_t)(value & 0x00FF);
        DBGM ("{VCA-1} channel: %d   effect: %d   Value: %d", this->SendBuffer[1], this->SendBuffer[2], value);
        this->SendComplete (MESSAGE_LENGTH_VCA);
        }
    }

//#####################################################################
void I2C_MESSAGE_C::Pause (bool state)
    {
    if ( state )
        this->SendBuffer[0] = (uint8_t)DISP_MESSAGE_N::CMD_C::PAUSE;
    else
        this->SendBuffer[0] = (uint8_t)DISP_MESSAGE_N::CMD_C::UPDATE;
    this->SendBuffer[1] = 0;
    DBGM ("{CMD} command: %d\n", this->SendBuffer[0]);
    this->SendComplete (MESSAGE_LENGTH_CNT);
    }

//#####################################################################
void I2C_MESSAGE_C::Selected (uint8_t channel, bool select)
    {
    this->SendVCA (channel, DISP_MESSAGE_N::EFFECT_C::SELECTED, (( select ) ? 1 : 0));
    }

//#####################################################################
void I2C_MESSAGE_C::AttackTime (uint8_t channel, uint16_t value)
    {
    this->SendVCA (channel, DISP_MESSAGE_N::EFFECT_C::ATTACK_TIME, value);
    }

//#####################################################################
void I2C_MESSAGE_C::MaxLevel (uint8_t channel, uint16_t value)
    {
    this->SendVCA (channel, DISP_MESSAGE_N::EFFECT_C::MAX_LEVEL, value);
    }

//#####################################################################
void I2C_MESSAGE_C::DecayTime (uint8_t channel, uint16_t value)
    {
    this->SendVCA (channel, DISP_MESSAGE_N::EFFECT_C::DECAY_TIME, value);
    }

//#####################################################################
void I2C_MESSAGE_C::SustainTime (uint8_t channel, uint16_t value)
    {
    this->SendVCA (channel, DISP_MESSAGE_N::EFFECT_C::SUSTAIN_TIME, value);
    }

//#####################################################################
void I2C_MESSAGE_C::ReleaseTime (uint8_t channel, uint16_t value)
    {
    this->SendVCA (channel, DISP_MESSAGE_N::EFFECT_C::RELEASE_TIME, value);
    }

//#####################################################################
void I2C_MESSAGE_C::SustainLevel (uint8_t channel, uint16_t value)
    {
    this->SendVCA (channel, DISP_MESSAGE_N::EFFECT_C::SUSTAIN_LEVEL, value);
    }

//#####################################################################
void I2C_MESSAGE_C::SelectADSR (uint8_t channel, bool select)
    {
    this->SendVCA (channel, DISP_MESSAGE_N::EFFECT_C::SELECTED, (( select ) ? 1 : 0));
    }

//#####################################################################
void I2C_MESSAGE_C::SawtoothDirection (bool select)
    {
    this->SendVCA ((uint8_t)(DISP_MESSAGE_N::ADSR_C::SAWTOOTH), DISP_MESSAGE_N::EFFECT_C::SAWTOOTH_DIRECTION, (( select ) ? 1 : 0));
    }

I2C_MESSAGE_C DisplayMessage;


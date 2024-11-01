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

using namespace DISP_MESSAGE_N;

static const char* LabelD = "D";
#define DBGM(args...) {if(DebugDisp){ DebugMsg(LabelD,DEBUG_NO_INDEX,args);}}

static byte sendBuffer[16];

//#######################################################################
//#######################################################################

I2C_MESSAGE_C::I2C_MESSAGE_C () : Ready (false)
    {
    ResetState = false;
    Paused = false;
    }

//#######################################################################
void I2C_MESSAGE_C::Begin (uint8_t display, uint8_t sda, uint8_t scl)
    {
    pinMode (RESET_STROBE_IO, INPUT);

    memset (sendBuffer, 0, 16);
    this->DisplayAddress = display;
    Wire1.begin (sda, scl, 400000UL);   // Clock at 400kHz
    this->Ready = true;
    }

//###################################################################
void I2C_MESSAGE_C::SendComplete (byte length)
    {
    Wire1.beginTransmission (DisplayAddress);
    int written = Wire1.write ((const uint8_t*)sendBuffer, length);
    Wire1.endTransmission();
    if ( written != length )
        printf("[DISP] ### ERROR ### Attempt to send %d byte message but shows %d", length, written);
    }

//###################################################################
void I2C_MESSAGE_C::Page (PAGE_C page)
    {
    if ( this->Ready )
        {
        sendBuffer[0] = (uint8_t)CMD_C::PAGE_SHOW;
        sendBuffer[1] = 0;
        sendBuffer[2] = (uint8_t)page;
        DBGM ("Page %d", (uint8_t)page);
        SendComplete (MESSAGE_LENGTH_PAGE);
        }
    }

//###################################################################
// CMD_C chan EFFECT_C high low
void I2C_MESSAGE_C::SendUpdate (CMD_C page, uint8_t channel, EFFECT_C effect, uint16_t value)
    {
    if ( this->Ready )
        {
        sendBuffer[0] = (uint8_t)page;
        sendBuffer[1] = channel;
        sendBuffer[2] = (uint8_t)effect;
        sendBuffer[3] = (uint8_t)(value & 0x00FF);
        sendBuffer[4] = (uint8_t)(value >> 8);
        DBGM ("Update: %X  channel: %X   effect: %X   Value: %X", sendBuffer[0], sendBuffer[1], sendBuffer[2], value);
        this->SendComplete (MESSAGE_LENGTH_UPDATE);
        }
    }

//#####################################################################
void I2C_MESSAGE_C::Pause (bool state)
    {
    if ( state )
        sendBuffer[0] = (uint8_t)CMD_C::PAUSE;
    else
        sendBuffer[0] = (uint8_t)CMD_C::GO;
    sendBuffer[1] = 0;
    DBGM ("Command: %d\n", sendBuffer[0]);
    this->SendComplete (MESSAGE_LENGTH_CNTL);
    }

I2C_MESSAGE_C DisplayMessage;


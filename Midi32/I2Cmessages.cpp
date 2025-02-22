//#######################################################################
// Module:     SyntFront.ino
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include <Wire.h>
#define ALLOCATE_DISP_MESSAGES
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
    CurrentVoicePage = 0;
    }

//#######################################################################
void I2C_MESSAGE_C::Begin (byte display, byte sda, byte scl)
    {
    pinMode (RESET_STROBE_IO, INPUT);

    memset (sendBuffer, 0, 16);
    this->DisplayAddress = display;
    Wire1.begin (sda, scl, 400000UL);   // Clock at 400kHz
    this->SetVoicePage (0, 1);
    this->SetVoicePage (1, 1);
    this->SetVoicePage (2, 1);
    this->SetVoicePage (3, 1);
    this->Page (PAGE_C::PAGE_OSC0);
    this->Ready = true;
    }

//###################################################################
void I2C_MESSAGE_C::SendComplete (byte length)
    {
    Wire1.beginTransmission (DisplayAddress);
    int written = Wire1.write ((const byte*)sendBuffer, length);
    Wire1.endTransmission();
    if ( written != length )
        printf("[DISP] ### ERROR ### Attempt to send %d byte message but shows %d", length, written);
    }

//###################################################################
void I2C_MESSAGE_C::Page (PAGE_C page)
    {
    if ( !this->Lock )
        {
        if ( this->Ready )
            {
            sendBuffer[0] = (byte)CMD_C::PAGE_SHOW;
            sendBuffer[1] = 0;
            sendBuffer[2] = (byte)page;
            DBGM ("Page %d", (byte)page);
            SendComplete (MESSAGE_LENGTH_PAGE);

            if ( (page == PAGE_C::PAGE_TUNING) || (page == PAGE_C::PAGE_MIDI_MAP) )
                this->Lock = true;
            }
        }
    }

//###################################################################
void I2C_MESSAGE_C::Reset ()
    {
    if ( this->Ready )
        {
        sendBuffer[0] = (byte)CMD_C::RESET;
        sendBuffer[1] = 0;
        sendBuffer[2] = 0;
        SendComplete (MESSAGE_LENGTH_PAGE);
        }
    }
//###################################################################
// Send command to attach midi channel to voice page indexed.
void I2C_MESSAGE_C::SetVoicePage (byte page, byte midi)
    {
    if ( this->Ready )
        {
        sendBuffer[0] = (byte)CMD_C::SET_PAGE_VOICE;
        sendBuffer[1] = page;
        sendBuffer[2] = midi;
        DBGM ("Set voice %d to page %d", (byte)page, (byte)midi);
        SendComplete (MESSAGE_LENGTH_PAGE);
        }
    }
//###################################################################
// CMD_C chan EFFECT_C high low
void I2C_MESSAGE_C::SendUpdate (CMD_C cmd, byte mapi, byte channel, EFFECT_C effect, short value)
    {
    if ( this->Ready )
        {
        sendBuffer[0] = (byte)cmd;
        sendBuffer[1] = mapi;
        sendBuffer[2] = channel;
        sendBuffer[3] = (byte)effect;
        sendBuffer[4] = (byte)(value & 0x00FF);
        sendBuffer[5] = (byte)(value >> 8);
        DBGM ("cmd: %X  mapi: %d\tchannel: %X\tValue: %X\teffect: %s", sendBuffer[0], sendBuffer[1], sendBuffer[2], value, EffectText[sendBuffer[3]]);
        this->SendComplete (MESSAGE_LENGTH_UPDATE);
        }
    }

I2C_MESSAGE_C DisplayMessage;


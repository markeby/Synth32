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

static const char* LabelD = "DSP";
#define ERROR(args...) {ErrorMsg (LabelD, __FUNCTION__, args);}
#define DBGM(args...) {if(DebugDisp){ DebugMsg(LabelD,DEBUG_NO_INDEX,args);}}

//#######################################################################
//#######################################################################

I2C_MESSAGE_C::I2C_MESSAGE_C () : Ready (false)
    {
    this->ResetState = false;
    this->Paused = false;
    this->CurrentPage = 0;
    memset (this->SendBuffer, 0, sizeof (this->SendBuffer));
    }

//#######################################################################
void I2C_MESSAGE_C::Begin (byte display, byte sda, byte scl)
    {
    pinMode (RESET_STROBE_IO, INPUT);

    this->DisplayAddress = display;
    Wire1.begin (sda, scl, 400000UL);   // Clock at 400kHz
    this->Ready = true;
    }

//###################################################################
void I2C_MESSAGE_C::SendComplete (byte length)
    {
    Wire1.beginTransmission (this->DisplayAddress);
    int written = Wire1.write ((const byte*)this->SendBuffer, length);
    this->LastEndT = Wire1.endTransmission ();
    if ( this->LastEndT )
        ERROR ("Send message command 0x%X of %d length, with error: %s", *this->SendBuffer, length, ErrorStringI2C (this->LastEndT));
    if ( written != length )
        ERROR ("Attempt to send %d byte message but shows %d", length, written);
    }

//###################################################################
void I2C_MESSAGE_C::Page (byte page)
    {
    if ( !this->Lock )
        {
        if ( this->Ready )
            {
            this->SendBuffer[0] = (byte)CMD_C::PAGE_SHOW;
            this->SendBuffer[1] = 0;
            this->SendBuffer[2] = page;
            DBGM ("Page %d", page);
            SendComplete (MESSAGE_LENGTH_PAGE);
            this->CurrentPage = page;
            if ( (page == (byte)PAGE_C::PAGE_TUNING) || (page == (byte)PAGE_C::PAGE_MIDI_MAP) )
                this->Lock = true;
            }
        }
    }

//###################################################################
void I2C_MESSAGE_C::Reset ()
    {
    if ( this->Ready )
        {
        this->SendBuffer[0] = (byte)CMD_C::RESET;
        this->SendBuffer[1] = 0;
        this->SendBuffer[2] = 0;
        SendComplete (MESSAGE_LENGTH_PAGE);
        }
    }
//###################################################################
// Send command to attach midi channel to page.
void I2C_MESSAGE_C::SetPage (byte page, byte midi)
    {
    if ( this->Ready )
        {
        this->SendBuffer[0] = (byte)CMD_C::SET_PAGE;
        this->SendBuffer[1] = page;
        this->SendBuffer[2] = midi;
        DBGM ("Set voice %d to page %d", (byte)page, (byte)midi);
        SendComplete (MESSAGE_LENGTH_PAGE);
        this->CurrentPage = page;
        }
    }
//###################################################################
// CMD_C chan EFFECT_C high low
void I2C_MESSAGE_C::SendUpdate (CMD_C cmd, byte mapi, byte channel, EFFECT_C effect, short value)
    {
    if ( this->Ready )
        {
        this->SendBuffer[0] = (byte)cmd;
        this->SendBuffer[1] = mapi;
        this->SendBuffer[2] = channel;
        this->SendBuffer[3] = (byte)effect;
        this->SendBuffer[4] = (byte)(value & 0x00FF);
        this->SendBuffer[5] = (byte)(value >> 8);
        DBGM ("cmd: %s  mapi: %d\tchannel: %X\tValue: %X\teffect: %s",
              CommandText[this->SendBuffer[0]],
              this->SendBuffer[1],
              this->SendBuffer[2],
              value,
              EffectText[this->SendBuffer[3]]);
        this->SendComplete (MESSAGE_LENGTH_UPDATE);
        }
    }

I2C_MESSAGE_C DisplayMessage;


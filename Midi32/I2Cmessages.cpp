//#######################################################################
// Module:     SyntFront.ino
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include <Wire.h>
#include <Debug.h>
#define ALLOCATE_DISP_MESSAGES
#include "I2Cmessages.h"

using namespace DISP_MESSAGE_N;

static const char* LabelD = "DSP";
#define ERROR(args...) {ErrorMsg (LabelD, __FUNCTION__, args);}
#define DBGM(args...) {if(DebugDisp){ DebugMsg(LabelD,DEBUG_NO_INDEX,args);}}

//#######################################################################
//#######################################################################

I2C_MESSAGE_C::I2C_MESSAGE_C () : _Ready (false)
    {
    _ResetState = false;
    _Paused = false;
    _CurrentPage = 0;
    memset (_SendBuffer, 0, sizeof (_SendBuffer));
    }

//#######################################################################
void I2C_MESSAGE_C::Begin (byte display, byte sda, byte scl)
    {
    pinMode (RESET_STROBE_IO, INPUT);

    _DisplayAddress = display;
    Wire1.begin (sda, scl, 400000UL);   // Clock at 400kHz
    _Ready = true;
    }

//###################################################################
void I2C_MESSAGE_C::SendComplete (byte length)
    {
    Wire1.beginTransmission (_DisplayAddress);
    int written = Wire1.write ((const byte*)_SendBuffer, length);
    _LastEndT = Wire1.endTransmission ();
    if ( _LastEndT )
        ERROR ("Send message command 0x%X of %d length, with error: %s", *_SendBuffer, length, ErrorStringI2C (_LastEndT));
    if ( written != length )
        ERROR ("Attempt to send %d byte message but shows %d", length, written);
    }

//###################################################################
void I2C_MESSAGE_C::Page (byte page)
    {
    if ( !_Lock )
        {
        if ( _Ready )
            {
            _SendBuffer[0] = (byte)CMD_C::PAGE_SHOW;
            _SendBuffer[1] = 0;
            _SendBuffer[2] = page;
            DBGM ("Page %d", page);
            SendComplete (MESSAGE_LENGTH_PAGE);
            _CurrentPage = page;
            if ( (page == (byte)PAGE_C::PAGE_TUNING) || (page == (byte)PAGE_C::PAGE_MIDI_MAP) )
                _Lock = true;

            }
        }
    }

//###################################################################
void I2C_MESSAGE_C::Reset ()
    {
    if ( _Ready )
        {
        _SendBuffer[0] = (byte)CMD_C::RESET;
        _SendBuffer[1] = 0;
        _SendBuffer[2] = 0;
        SendComplete (MESSAGE_LENGTH_PAGE);
        }
    }
//###################################################################
// Send command to attach midi channel to page.
void I2C_MESSAGE_C::SetPage (byte page, byte midi)
    {
    if ( _Ready )
        {
        _SendBuffer[0] = (byte)CMD_C::SET_PAGE;
        _SendBuffer[1] = page;
        _SendBuffer[2] = midi;
        DBGM ("Set voice %d to page %d", (byte)page, (byte)midi);
        SendComplete (MESSAGE_LENGTH_PAGE);
        if ( _CurrentPage != page )
            delay (360);
        _CurrentPage = page;
        }
    }
//###################################################################
// CMD_C chan EFFECT_C high low
void I2C_MESSAGE_C::SendUpdate (CMD_C cmd, byte mapi, byte channel, EFFECT_C effect, short value)
    {
    if ( _Ready )
        {
        _SendBuffer[0] = (byte)cmd;
        _SendBuffer[1] = mapi;
        _SendBuffer[2] = channel;
        _SendBuffer[3] = (byte)effect;
        _SendBuffer[4] = (byte)(value & 0x00FF);
        _SendBuffer[5] = (byte)(value >> 8);
        DBGM ("cmd: %s  mapi: %d\tchannel: %X\tValue: %X\teffect: %s",
              CommandText[_SendBuffer[0]],
              _SendBuffer[1],
              _SendBuffer[2],
              value,
              EffectText[_SendBuffer[3]]);
        SendComplete (MESSAGE_LENGTH_UPDATE);
        }
    }

I2C_MESSAGE_C DisplayMessage;


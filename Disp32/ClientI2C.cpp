//#######################################################################
// Module:     ClientI2C.cpp
// Descrption: Display command receiver
// Creator:    markeby
// Date:       6/23/2024
//#######################################################################
#include <Wire.h>

#include "config.h"
#include "Graphics.h"
#include "ClientI2C.h"
#include "Debug.h"
#include "../Common/DispMessages.h"

static const char* Label = "I";
#define DBG(args...) {if(DebugInterface){ DebugMsg(Label,DEBUG_NO_INDEX,args);}}

using namespace DISP_MESSAGE_N;

//################ Data to client #######################################
static void DataOut ()
    {
    printf("We aren't doing I2C output\n");
    Wire.print ("Nothing");
    }

//################ Data from client #####################################
static void DataIn (int len)
    {
    byte* bptr = Client.GetNextBuffer ();
    int   z    = 0;

    while ( Wire.available () )
        bptr[z++] = Wire.read ();
    if ( z == len )
        Client.NextBufferGood ();
    }

//#######################################################################
//#######################################################################
    MESSAGE_CLIENT_C::MESSAGE_CLIENT_C   ()
    {
    this->NextBufferIndex   = 0;
    this->CuurentBufferTop  = 0;
    this->CountBuffersInUse = 0;
    memset (this->Buffers, 0, BUFFER_COUNT * sizeof (BUFFER_T));
    };

//#######################################################################
void MESSAGE_CLIENT_C::Begin (uint8_t device_addr)
    {
    // Set listening addresss and start listening
    Wire.onReceive (DataIn);
    Wire.onRequest (DataOut);
    Wire.setClock(400000);
    Wire.begin (device_addr);
    }

//#######################################################################
// Trigger Midi side to send Current settings
void MESSAGE_CLIENT_C::TriggerInitialMsgs  ()
    {
    digitalWrite (MIDI_TRIGGER_PORT, LOW);
    delay (RESET_TRIGGER_TIME / 1000);
    digitalWrite (MIDI_TRIGGER_PORT, HIGH);
    delay (50);
    digitalWrite (MIDI_TRIGGER_PORT, LOW);
    }

//#######################################################################
void MESSAGE_CLIENT_C::Process ()
    {
    if ( this->CountBuffersInUse )
        {
        BUFFER_T& ptop = this->Buffers[this->CuurentBufferTop];
        uint16_t value = ptop.Value0 | (ptop.Value1 << 8);
        DBG ("Update: %X  channel: %X   effect: %X   Value: %X", (uint8_t)ptop.Command, (uint8_t)ptop.Channel, (uint8_t)ptop.Effect, value);
        switch ( ptop.Command )
            {
            case CMD_C::UPDATE_PAGE_OSC:
                Graphics.UpdatePageOsc ((uint8_t)ptop.Channel, ptop.Effect, value);
                Graphics.PageSelect (PAGE_C::PAGE_OSC);
                break;
            case CMD_C::UPDATE_PAGE_MOD:
                Graphics.UpdatePageMod ((uint8_t)ptop.Channel, ptop.Effect, value);
                Graphics.PageSelect (PAGE_C::PAGE_MOD);
                break;
            case CMD_C::UPDATE_PAGE_FILTER:
                Graphics.PageSelect (PAGE_C::PAGE_FILTER);
                break;
            case CMD_C::UPDATE_PAGE_TUNING:
                Graphics.UpdatePageTuning ((uint8_t)ptop.Channel, ptop.Effect, value);
                break;
            case CMD_C::PAUSE:
                Graphics.Pause (true);
                break;
            case CMD_C::GO:
                Graphics.Pause (false);
                break;
            case CMD_C::PAGE_SHOW:
                Graphics.PageSelect ((PAGE_C)ptop.Bytes[2]);
                break;
            default:
                break;
            }
        if ( ++this->CuurentBufferTop == BUFFER_COUNT )
            this->CuurentBufferTop = 0;
        this->CountBuffersInUse--;
        }
    }

//#######################################################################
MESSAGE_CLIENT_C    Client;

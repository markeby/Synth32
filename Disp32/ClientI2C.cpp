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
        DBG ("Update: 0x%X  channel: 0x%X   effect: 0x%X   Value: 0x%X   Index: %d", (byte)ptop.Command, (byte)ptop.Channel, (byte)ptop.Effect, value, ptop.Index);
        switch ( ptop.Command )
            {
            case CMD_C::SET_PAGE:               // 3 byte message
                Graphics.SetPage (ptop.Index, (byte)ptop.Channel);
                break;
            case CMD_C::UPDATE_PAGE_VOICE:      // 5 byte message
                Graphics.UpdatePageVoice (ptop.Index, (byte)ptop.Channel, ptop.Effect, value);
                break;
            case CMD_C::UPDATE_PAGE_FILTER:      // 5 byte message
                Graphics.UpdatePageFilter (ptop.Index, (byte)ptop.Channel, ptop.Effect, value);
                break;
            case CMD_C::UPDATE_PAGE_MOD:        // 5 byte message
                Graphics.UpdatePageMod (ptop.Index, (byte)ptop.Channel, ptop.Effect, value);
                break;
            case CMD_C::UPDATE_PAGE_TUNING:     // 5 byte message
                Graphics.UpdatePageTuning ((uint8_t)ptop.Channel, ptop.Effect, value);
                break;
            case CMD_C::UPDATE_PAGE_MAP:        // 5 byte message
                Graphics.UpdatePageMap ((uint8_t)ptop.Channel, ptop.Effect, value);
                break;
            case CMD_C::UPDATE_PAGE_LOAD_SAVE:  // 5 byte meesage
                Graphics.UpdatePageLoadSave  (ptop.Effect, value);
                break;
            case CMD_C::RESET:                  // 3 byte message
                ESP.restart ();
                break;
            case CMD_C::PAGE_SHOW:              // 3 byte message
                Graphics.PageSelect ((PAGE_C)ptop.Bytes[2]);
                break;
            default:                            // any other message goes nowher
                break;
            }
        if ( ++this->CuurentBufferTop == BUFFER_COUNT )
            this->CuurentBufferTop = 0;
        this->CountBuffersInUse--;
        }
    }

//#######################################################################
MESSAGE_CLIENT_C    Client;

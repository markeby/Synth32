//#######################################################################
// Module:     ClientI2C.cpp
// Descrption: Display command receiver
// Creator:    markeby
// Date:       6/23/2024
//#######################################################################
#include <Wire.h>

#include "config.h"
#include "Graphics.h"
#include "Debug.h"
#define   ALLOCATE_DISP_TEXT 1
#include "../Common/DispMessages.h"

static const char* LabelI = "I";
#define DBGI(args...) {if(DebugInterface){ DebugMsg(LabelI,DEBUG_NO_INDEX,args);}}

static  uint8_t     msgBuff[32];

using namespace DISP_MESSAGE_N;

static void DataOut ()
    {
    printf("onRequest ??\n");
    Wire.print (" Packets.");
    }

static void DataIn (int len)
    {
    short z = 0;

    while ( Wire.available () )
        msgBuff[z++] = Wire.read ();

    switch ( len )
        {
        case MESSAGE_LENGTH_VCA:
            {
            z = msgBuff[4] | (msgBuff[3] << 8);
            switch ( msgBuff[0] )
                {
                case (int)DISP_MESSAGE_N::CMD_C::CTRL_VCA:
                    DBGI("{VCA} Channel: %s   Effect: %s   Value: %d", ClassADSR[msgBuff[1]], ClassEFFECT[msgBuff[2]], z);
                    Graphics.UpdatePageVCA (msgBuff[1], msgBuff[2],  z);
                    break;
                default:
                    break;
                }
            }
        case MESSAGE_LENGTH_CNT:
            {
            switch ( msgBuff[0] )
                {
                case (int)DISP_MESSAGE_N::CMD_C::PAUSE:
                    DBGI("{CNT} Command: %s", ClassCMD[2]);
                    Graphics.Pause (true);
                    break;
                case (int)DISP_MESSAGE_N::CMD_C::UPDATE:
                    DBGI("{CNT} Command: %s", ClassCMD[3]);
                    Graphics.Pause (false);
                    break;
                }
            }
        }
    }

//#######################################################################
//#######################################################################
void StartI2C (uint8_t device_addr)
    {
    Wire.onReceive (DataIn);
    Wire.onRequest (DataOut);
    Wire.begin (device_addr);
    }

//#######################################################################
// Module:     ClientI2C.h
// Descrption: Display command receiver
// Creator:    markeby
// Date:       6/23/2024
//#######################################################################
#pragma once
#include "../Common/DispMessages.h"

#define MIDI_TRIGGER_PORT   6
#define BUFFER_COUNT        1024

typedef union
    {
    uint8_t     Bytes[MESSAGE_LENGTH_UPDATE];
    struct
        {
        DISP_MESSAGE_N::CMD_C       Command;
        uint8_t                     Index;
        DISP_MESSAGE_N::VOICE_OPT_C Channel;
        DISP_MESSAGE_N::EFFECT_C    Effect;
        uint8_t                     Value0;
        uint8_t                     Value1;
        };
    }  BUFFER_T;

class MESSAGE_CLIENT_C
    {
private:
    BUFFER_T    Buffers[BUFFER_COUNT];
    int         NextBufferIndex;
    int         CuurentBufferTop;
    int         CountBuffersInUse;

public:
            MESSAGE_CLIENT_C   (void);
    void    Begin              (uint8_t device_addr);
    void    TriggerInitialMsgs (void);
    void    Process            (void);

    //###################################################################
    byte* GetNextBuffer (void)
        {
        if ( CountBuffersInUse ==  BUFFER_COUNT )   // if buffer overrun
            {
            CountBuffersInUse--;                    // Error message and compensate
            printf ("***ERROR*** Client buffer overrun!\n");
            }
        return ((byte*)&(Buffers[NextBufferIndex]));
        }

    //###################################################################
    void NextBufferGood (void)
        {
        if ( ++NextBufferIndex == BUFFER_COUNT )    // bump buffer and test for roll over
            NextBufferIndex = 0;
        CountBuffersInUse++;            // Add in use count for overrun checking
        }
    };

extern MESSAGE_CLIENT_C   Client;


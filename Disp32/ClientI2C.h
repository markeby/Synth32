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
    uint8_t     Bytes[5];
    struct
        {
        DISP_MESSAGE_N::CMD_C       Command;
        DISP_MESSAGE_N::VOICE_C   Channel;
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
    inline byte* GetNextBuffer (void)
        {
        if ( this->CountBuffersInUse ==  BUFFER_COUNT )   // if buffer overrun
            {
            this->CountBuffersInUse--;                    // Error message and compensate
            printf ("***ERROR*** Client buffer overrun!\n");
            }
        return ((byte*)&(this->Buffers[this->NextBufferIndex]));
        }

    //###################################################################
    inline void NextBufferGood (void)
        {
        if ( ++this->NextBufferIndex == BUFFER_COUNT )    // bump buffer and test for roll over
            this->NextBufferIndex = 0;
        this->CountBuffersInUse++;            // Add in use count for overrun checking
        }
    };

extern MESSAGE_CLIENT_C   Client;


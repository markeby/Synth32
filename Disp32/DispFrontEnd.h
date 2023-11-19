//#######################################################################
// Module:     SynthFront.h
// Descrption: Interface to the synth channels
// Creator:    markeby
// Date:       8/25/2023
//#######################################################################
#pragma once

#include "DispMessages.h"

//#################################################
//    Synthesizer Display Graphics front end class
//#################################################
class   DISP_FRONT_END_C
    {
private:
    uint16_t            LastOp;
    int                 InTuning;
    int                 SetTuning;
    int                 SetDeviceIndex;

public:
    void  SendControl       (void);
          DISP_FRONT_END_C  (void);
    void  Begin             (void);
    void  SendReset         (void);
    void  Loop              (void);
    void  Controller        (byte chan, byte type, byte value);
    void  ChannelSelect     (byte chan, bool state);
    };

//#################################################

extern DISP_FRONT_END_C DispFront;        // Synth Display front end class


//#######################################################################
// Module:     SynthFront.h
// Descrption: Interface to the synth channels
// Creator:    markeby
// Date:       8/25/2023
//#######################################################################
#pragma once

#include "../Common/DispMessages.h"

//#################################################
//    Synthesizer Display Graphics front end class
//#################################################
class   DISP_FRONT_END_C
    {
private:


public:
    void  SendControl       (void);
          DISP_FRONT_END_C  (void);
    void  Begin             (void);
    void  SendReset         (void);
    void  Loop              (void);
    void  Controller        (byte ch, byte type, byte value);
    void  ChannelSelect     (byte ch, bool state);
    void  Display           (byte ch);
    };

//#################################################
extern DISP_FRONT_END_C DispFront;        // Synth Display front end class


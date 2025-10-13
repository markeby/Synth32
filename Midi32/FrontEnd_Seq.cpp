//#######################################################################
// Module:     FrontEndSeq.cpp
// Descrption: Synthesizer sequencing input
// Creator:    markeby
// Date:       10/11/2025
//#######################################################################
#include <UHS2-MIDI.h>
#include <MIDI.h>

#include "../Common/SynthCommon.h"
#include "FrontEnd.h"
#include "Debug.h"

#ifdef DEBUG_SYNTH
static const char* Label  = "SEQ";
#define DBG(args...) {if(DebugSeq){DebugMsg(Label,mchan,args);}}
#else
#define DBG(args...)
#endif

using namespace DISP_MESSAGE_N;

//#######################################################################
void SYNTH_FRONT_C::SeqController (short mchan, byte type, byte value)
    {
    int  z;
    bool zl;

    switch ( type )
        {
        case 0x78:      // [Channel Mode Message] All Sound Off
            DBG ("[Channel Mode Message] All Sound Off");
            this->Clear ();
            break;

        case 0x40:      // Damper pedal (sustain)
            if ( value < 64 )       zl = false;
            else                    zl = true;
            DBG ("Damper pdeal: %s", (( zl ) ? "ON" : "OFF"));
            break;

        case 0x0B:
            DBG ("Expression Controller: %d", value);
            break;

        case 0x5D:
            DBG ("Effects 3 Depth: %d", value);
            break;
        case 0x5B:
            DBG ("Effects 1 Depth: %d", value);
            break;

        case 0x07:      // Channel volume
            DBG ("Channel volume: %d", value);
            break;

        case 0x01:      // Modulation wheel
            DBG ("Modulation whee:l %d", value);
            break;
        case 0x21:      // LSB Modulation wheel
            DBG ("Modulation wheel: LSB %d", value);
            break;

        case 0x00:
            DBG ("Bank select: %d", value);
            break;
        case 0x20:
            DBG ("Bank select LSB: %d", value);
            break;

        case 0x64:
            DBG ("RPN LSB: %d", value);
            break;
        case 0x65:
            DBG ("RPN MSB: %d", value);
            break;
        case 0x06:
            DBG ("Data Entry MSB: %d", value);
            break;

        default:
            DBG ("Uknown controller command: %d   value: %d", type, value);
            break;
        }

    }


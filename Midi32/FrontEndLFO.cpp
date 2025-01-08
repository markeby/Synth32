//#######################################################################
// Module:     SyntFront.ino
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <UHS2-MIDI.h>

#include "../Common/SynthCommon.h"
#include "../Common/DispMessages.h"
#include "I2Cmessages.h"
#include "Osc.h"
#include "FrontEnd.h"
#include "Debug.h"

#ifdef DEBUG_ON
static const char* Label  = "TOP";
static const char* LabelM = "M";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#define DBGM(args...) {if(DebugMidi){DebugMsg(LabelM,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#define DBGM(args...)
#endif

//#####################################################################
void SYNTH_FRONT_C::ToggleSelectModVCA (byte ch)
    {
    bool zb = !this->ModulationVCA[ch];
    this->ModulationVCA[ch] = zb;

    DisplayMessage.LfoSoftSelect (ch, zb);
    for ( int z = 0;  z < CHAN_COUNT;  z++)
        this->pChan[z]->pOsc()->SetSoftLFO (ch, zb);
    }

//#######################################################################
void SYNTH_FRONT_C::FreqLFO (short ch, short data)
    {
    switch ( ch )
        {
        case 2:
            DisplayMessage.LfoHardPulseWidth (data);
            this->Lfo[0].SetPulseWidth (data);
            break;
        case 1:
            DisplayMessage.LfoHardFreq (data);
            this->Lfo[0].SetFreq (data);
            break;
        case 0:
            DisplayMessage.LfoSoftFreq (data);
            SoftLFO.SetFrequency (data);
            break;
        }
    }



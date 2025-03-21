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

#ifdef DEBUG_SYNTH
static const char* Label  = "TOP";
static const char* LabelM = "M";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#define DBGM(args...) {if(DebugMidi){DebugMsg(LabelM,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#define DBGM(args...)
#endif

//#####################################################################
void SYNTH_FRONT_C::SelectModVCA (byte ch, bool state)
    {
    this->ModulationVCA[ch] = state;

    DisplayMessage.LfoSoftSelect (ch, state);
    for ( int z = 0;  z < VOICE_COUNT;  z++)
        this->pVoice[z]->pOsc()->SetSoftLFO (ch, state);
    }

//#######################################################################
void SYNTH_FRONT_C::FreqLFO (short ch, short data)
    {
    switch ( ch )
        {
        case 2:
            DisplayMessage.LfoHardPulseWidth (data);
            this->Lfo[0].SetPulseWidth (data);
            this->Lfo[1].SetPulseWidth (data);
            break;
        case 1:
            DisplayMessage.LfoHardFreq (data);
            this->Lfo[0].SetFreq (data);
            this->Lfo[1].SetFreq (data);
            break;
        case 0:
            DisplayMessage.LfoSoftFreq (data);
            SoftLFO.SetFrequency (data);
            break;
        }
    this->KnobMap[ch].Value = data;
    }

//#######################################################################
void SYNTH_FRONT_C::PitchBend (short value)
    {
    short z = value + this->PitchBendOffset;
    if ( z > 4090 )
        return;
    else if ( z < 126 )
        return;
    this->Lfo[0].PitchBend(z);
    this->Lfo[1].PitchBend (z);
    }


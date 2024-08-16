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

static const char* Label  = "TOP";
static const char* LabelM = "M";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#define DBGM(args...) {if(DebugMidi){DebugMsg(LabelM,DEBUG_NO_INDEX,args);}}

//#######################################################################
void SYNTH_FRONT_C::PitchBend (uint8_t ch, int value)
    {
    float scaler = (value + 16384) * BEND_SCALER;
    this->Lfo.PitchBend (scaler);
    DBG ("Pitch bend > %f", scaler);
    }

//#####################################################################
void SYNTH_FRONT_C::SelectWaveVCA (uint8_t ch, uint8_t state)
    {
    for ( int z = 0;  z < CHAN_COUNT;  z++)
        this->pChan[z]->pOsc()->SetSoftLFO (ch, state);
    }

//#####################################################################
void SYNTH_FRONT_C::SelectWaveVCF (uint8_t ch, uint8_t state)
    {
    this->Lfo.Select (ch, state);
    }

//#######################################################################
void SYNTH_FRONT_C::SetLevelLFO (uint8_t data)
    {
    this->Lfo.Level (data * PERS_SCALER);
    }

//#######################################################################
void SYNTH_FRONT_C::FreqLFO (byte ch, byte data)
    {
    float zf = (float)data * PRS_SCALER;

    if ( ch )
        {
        DisplayMessage.HardwareFreqLFO (data);
        this->Lfo.SetFreq (zf);
        }
    else
        {
        DisplayMessage.SoftwareFreqLFO (data);
        SoftLFO.SetFrequency (zf);
        }
    }


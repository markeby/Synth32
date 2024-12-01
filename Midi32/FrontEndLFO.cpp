//#######################################################################
// Module:     SyntFrontLFO.cpp
// Descrption: Synthesizer hardware LFO control
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

//#######################################################################
void SYNTH_FRONT_C::PitchBend (byte ch, int value)
    {
    float scaler = (value + 16384) * BEND_SCALER;
    this->Lfo.PitchBend (scaler);
    DBG ("Pitch bend > %f", scaler);
    }

//#####################################################################
void SYNTH_FRONT_C::SelectWaveVCA (byte ch, byte state)
    {
    for ( int z = 0;  z < CHAN_COUNT;  z++)
        this->pChan[z]->pOsc()->SetSoftLFO (ch, state);
    }

//#####################################################################
void SYNTH_FRONT_C::SelectWaveVCF (byte ch, byte state)
    {
    this->Lfo.Select (ch, state);
    }

//#######################################################################
void SYNTH_FRONT_C::SetLevelLFO (byte data)
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


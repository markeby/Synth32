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

//#######################################################################
void SYNTH_FRONT_C::PitchBend (short ch, short value)
    {
    this->Lfo[0].PitchBend (value);
    DBG ("Pitch bend > %d", value);
    }

//#####################################################################
void SYNTH_FRONT_C::ToggleSelectModVCA (short ch)
    {
    DisplayMessage.LfoSoftwareSelect (ch);
    for ( int z = 0;  z < CHAN_COUNT;  z++)
        this->pChan[z]->pOsc()->ToggleSoftLFO (ch);
    }

//#######################################################################
void SYNTH_FRONT_C::FreqLFO (short ch, short data)
    {
    switch ( ch )
        {
        case 2:
            DisplayMessage.LfoHardwarePulseWidth (data);
            this->Lfo[0].SetPulseWidth (data);
            break;
        case 1:
            DisplayMessage.LfoHardwareFreq (data);
            this->Lfo[0].SetFreq (data);
            break;
        case 0:
            DisplayMessage.LfoSoftwareFreq (data);
            SoftLFO.SetFrequency (data);
            break;
        }
    }

//#######################################################################
void SYNTH_FRONT_C::ToggleSelectWaveVCO (short ch)
    {
    DisplayMessage.LfoHardwareSelect (ch);
    this->Lfo[0].Toggle (ch);
    }

//#######################################################################
void SYNTH_FRONT_C::SetLevelLFO (short data)
    {
    DisplayMessage.LfoSoftwareFreq (data);
    this->Lfo[0].Level (data);
    }

//#######################################################################
void SYNTH_FRONT_C::ToggleRampSlope ()
    {
    uint8_t z;

    z = !this->Lfo[0].GetSawSlope ();
    DisplayMessage.LfoHardwareRampSlope (z);
    this->Lfo[0].SetSawSlope (z);
    }


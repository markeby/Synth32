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

//#######################################################################
void SYNTH_FRONT_C::FreqLFO (short ch, short data)
    {
    switch ( ch )
        {
        case 0:
            DisplayMessage.LfoSoftFreq    (data);
            SoftLFO.SetFrequency          (data);
            this->SynthConfig.SetSoftFreq (data);
            break;
        case 1:
            DisplayMessage.LfoHardFreq     (0, data);
            this->Lfo[0].SetFreq           (data);
            this->SynthConfig.SetFrequency (0, data);
            break;
        case 2:
            DisplayMessage.LfoHardPulseWidth (0, data);
            this->Lfo[0].SetPulseWidth       (data);
            this->SynthConfig.SetPulseWidth  (0, data);
            break;
        case 3:
            DisplayMessage.LfoHardFreq     (1, data);
            this->Lfo[1].SetFreq           (data);
            this->SynthConfig.SetFrequency (1, data);
            break;
        case 4:
            DisplayMessage.LfoHardPulseWidth (1, data);
            this->Lfo[1].SetPulseWidth       (data);
            this->SynthConfig.SetPulseWidth  (1, data);
            break;
        }
    this->G49MidiMapEcoder[ch].Value = data;
    if ( !this->ResolutionMode )
        this->NonOscPageSelect (DISP_MESSAGE_N::PAGE_C::PAGE_MOD);
    }

//#######################################################################
void SYNTH_FRONT_C::PitchBend (byte mchan, short value)
    {
    this->Lfo[0].PitchBend (mchan, value);
    this->Lfo[1].PitchBend (mchan, value);
    DBG ("Pitch Bend value = %d", value);
    }

//#####################################################################
void SYNTH_FRONT_C::SelectModVCA (byte ch, bool state)
    {
    if ( !this->MapSelectMode )
        {
        this->SynthConfig.SetModSoftMixer (ch, state);

        DisplayMessage.LfoSoftSelect (ch, state);
        for ( int z = 0;  z < VOICE_COUNT;  z++)
            {
            if ( this->pVoice[z]->GetMidi () == SoftLFO.GetMidi () )
                this->pVoice[z]->SetSoftLFO (ch, state);
            else
                this->pVoice[z]->SetSoftLFO (ch, false);
            }
        if ( !this->ResolutionMode )
            this->NonOscPageSelect (DISP_MESSAGE_N::PAGE_C::PAGE_MOD);
        }
    }


//#######################################################################
// Module:     SyntFront.ino
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <UHS2-MIDI.h>
#include <MIDI.h>

#include "../Common/SynthCommon.h"
#include "../Common/DispMessages.h"
#include "I2Cmessages.h"
#include "Osc.h"
#include "Settings.h"
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
void SYNTH_FRONT_C::ChannelSetSelect (byte chan, bool state)
    {
    pChan[this->ZoneBase]->SelectedEnvelope[chan] = state;
    DisplayMessage.OscSelected (CurrentZone, chan, pChan[this->ZoneBase]->SelectedEnvelope[chan]);
    DBG ("%s %s ", SwitchMap[chan].Desc, ( pChan[this->ZoneBase]->SelectedEnvelope[chan] ) ? "ON" : "off");

    byte val = 0x0C;
    for ( int z = 0;  z < OSC_MIXER_COUNT; z++ )
        {
        if ( pChan[this->ZoneBase]->SelectedEnvelope[z] )
            val = 0x3C;
        }
    this->ShowChannelXL (val);
    }

//#####################################################################
void SYNTH_FRONT_C::SetMaxLevel (byte ch, byte data)
    {
    if ( this->SetTuning )
        {
        this->TuningLevel[ch] = data * MIDI_MULTIPLIER;
        DisplayMessage.TuningLevel (ch, data);
        this->TuningChange = true;
        return;
        }

    float val = (float)data * PRS_SCALER;

    for ( int z = 0;  z < ZoneCount;  z++ )
        this->pChan[this->ZoneBase + z]->SetMaxLevel (ch, val);

    DisplayMessage.OscMaxLevel (CurrentZone, ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetAttackTime (byte data)
    {
    float dtime = data * TIME_MULT;

    for ( int ch = 0;  ch < OSC_MIXER_COUNT;  ch++ )
        {
        if ( this->pChan[this->ZoneBase]->SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < ZoneCount;  z++ )
                this->pChan[this->ZoneBase + z]->pOsc()->SetAttackTime (ch, dtime);
            DisplayMessage.OscAttackTime (CurrentZone, ch, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetDecayTime (byte data)
    {
    float dtime = data * TIME_MULT;

    for ( int ch = 0;  ch < OSC_MIXER_COUNT;  ch++ )
        {
        if ( this->pChan[this->ZoneBase]->SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < ZoneCount;  z++)
                this->pChan[this->ZoneBase + z]->pOsc()->SetDecayTime (ch, dtime);
            DisplayMessage.OscDecayTime (CurrentZone, ch, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetSustainLevel (byte ch, byte data)
    {
    float val = (float)data * PRS_SCALER;
    for ( int z = 0;  z < ZoneCount;  z++ )
        this->pChan[this->ZoneBase + z]->pOsc()->SetSustainLevel (ch, val);
    DisplayMessage.OscSustainLevel (CurrentZone, ch, data);
    }

//#####################################################################
void SYNTH_FRONT_C::SetSustainTime (byte data)
    {
    float dtime;

    if ( data == 0 )
        dtime = -1;
    else
        dtime = data * TIME_MULT;

    for ( int ch = 0;  ch < OSC_MIXER_COUNT;  ch++ )
        {
        if ( this->pChan[this->ZoneBase]->SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < ZoneCount;  z++)
                this->pChan[this->ZoneBase + z]->pOsc()->SetSustainTime (ch, dtime);
            DisplayMessage.OscSustainTime (CurrentZone, ch, data);
            }
        }
    }

//#####################################################################
void SYNTH_FRONT_C::SetReleaseTime (byte data)
    {
    float dtime = data * TIME_MULT;

    for ( int ch = 0;  ch < OSC_MIXER_COUNT;  ch++ )
        {
        if ( this->pChan[this->ZoneBase]->SelectedEnvelope[ch] )
            {
            for ( int z = 0;  z < ZoneCount;  z++)
                this->pChan[this->ZoneBase + z]->pOsc()->SetReleaseTime (ch, dtime);
            DisplayMessage.OscReleaseTime (CurrentZone, ch, data);
            }
        }
    }

//#######################################################################
void SYNTH_FRONT_C::SawtoothDirection (bool data)
    {
    for ( int z = 0;  z < ZoneCount;  z++)
        this->pChan[this->ZoneBase + z]->pOsc()->SawtoothDirection (data);
    DisplayMessage.OscSawtoothDirection (CurrentZone, data);
    if ( !data )
        this->ClearEntryRedL = XlMap[37].Index;
    }

//#######################################################################
void SYNTH_FRONT_C::SetPulseWidth (byte data)
    {
    float percent = data * PRS_SCALER;
    for ( int z = 0;  z < ZoneCount;  z++)
        this->pChan[this->ZoneBase + z]->pOsc()->PulseWidth (percent);
    DisplayMessage.OscPulseWidth (CurrentZone, data);
    }

//#######################################################################
void SYNTH_FRONT_C::SetNoise (byte ch, bool state)
    {
    int z;

    if ( this->CurrentZone == ZONE0 )
        {
        for ( z = 0;  z < DUCT_NUM;  z++ )
            this->pNoise->Select (z, ch, state);
        DisplayMessage.OscNoise (ZONE0, ch, state);
        }
    else
        {
        if ( ch & 0x80 )
            {
            ch &= 0x0F;
            for ( z = 0;  z < (DUCT_NUM / NUM_ZONES);  z++ )
                this->pNoise->Select (z + 2, ch, state);
            DisplayMessage.OscNoise (ZONE2, ch, state);
            }
        else
            {
            for ( int z = 0;  z < (DUCT_NUM / NUM_ZONES);  z ++)
                this->pNoise->Select (z, ch, state);
            DisplayMessage.OscNoise (ZONE1, ch, state);
            }
        }
    }

//#######################################################################
void SYNTH_FRONT_C::SaveAllSettings ()
    {
    for ( int z = 0;  z < CHAN_COUNT;  z++ )
        Settings.PutOscBank (z, this->pChan[z]->pOsc ()->GetBankAddr ());
    }


//#######################################################################
// Module:     FrontEnd.cpp
// Descrption: Synthesizer front end controller
// Creator:    markeby
// Date:       12/9/2024
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

//#####################################################################
void SYNTH_FRONT_C::MidiMapMode ()
    {
    if ( !this->MapSelectMode )
        {
        DisplayMessage.Page (DISP_MESSAGE_N::PAGE_C::PAGE_MIDI_MAP);
        this->MapSelectMode = true;
        DisplayMessage.SendMapVoiceMidi (this->CurrentMapSelected, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, this->MapVoiceMidi[this->CurrentMapSelected]);
        }
    else
        {
        DisplayMessage.Unlock ();
        DisplayMessage.Page (DISP_MESSAGE_N::PAGE_C::PAGE_OSC0);
        this->MapSelectMode = false;
        }
    }

//#####################################################################
void SYNTH_FRONT_C::MapModeBump (short down)
    {
    short z = this->MapVoiceMidi[this->CurrentMapSelected] + down;
    if ( z < 1 )            z = MAX_MIDI;
    if ( z > MAX_MIDI )     z = 1;
    this->MapVoiceMidi[this->CurrentMapSelected] = z;

    DisplayMessage.SendMapVoiceMidi (CurrentMapSelected, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, z);
    }

//#####################################################################
void SYNTH_FRONT_C::ChangeMapSelect (short right)
    {
    if ( this->MapSelectMode )
        {
        if ( right == 0 )   // go left
            {
            if ( --CurrentMapSelected < 0 )
                CurrentMapSelected = MAP_MAX;
            }
        else                // not left so must be right
            {
            if ( ++CurrentMapSelected > MAP_MAX )
                CurrentMapSelected = 0;
            }
        DisplayMessage.SendMapVoiceMidi (this->CurrentMapSelected, DISP_MESSAGE_N::EFFECT_C::MAP_VOICE, this->MapVoiceMidi[this->CurrentMapSelected]);
        }
    }


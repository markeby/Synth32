//#######################################################################
// Module:     Osc.cpp
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include "Settings.h"
#include "Osc.h"
#include "I2Cmessages.h"
#include "Debug.h"

#ifdef DEBUG_SYNTH
static const char* Label = "VCO";
#define DBG(args...) {if(DebugSynth){ DebugMsg(Label,Number,args);}}
#else
#define DBG(args...)
#endif

using namespace OSC_N;

#define CONST_MULT      (DA_RANGE / FULL_KEYS)
static  const char*     MixerNames[] = { "sine", "triangle", "square", "saw", "pulse" };

//#######################################################################
    OSC_C::OSC_C (short num, short first_device, byte& usecount, ENVELOPE_GENERATOR_C& envgen) : EnvGen (envgen)
    {
    this->Valid = false;
    this->Number = num;
    // D/A configuration
    this->OscPortIO                 = first_device + byte(D_A_OFF::EXPO);
    this->PwmPortIO                 = first_device + byte(D_A_OFF::WIDTH);
    this->RampDirPortIO             = first_device + byte(D_A_OFF::DIR);
    this->Mix[int(SHAPE::TRIANGLE)] = EnvGen.NewADSR (num, MixerNames[int(SHAPE::TRIANGLE)], first_device + byte(D_A_OFF::TRIANGLE), usecount);
    this->Mix[int(SHAPE::RAMP)]     = EnvGen.NewADSR (num, MixerNames[int(SHAPE::RAMP)], first_device + byte(D_A_OFF::RAMP), usecount);
    this->Mix[int(SHAPE::PULSE)]    = EnvGen.NewADSR (num, MixerNames[int(SHAPE::PULSE)], first_device + byte(D_A_OFF::PULSE), usecount);
    this->Mix[int(SHAPE::SINE)]     = EnvGen.NewADSR (num, MixerNames[int(SHAPE::SINE)], first_device + byte(D_A_OFF::SINE), usecount);
    this->Mix[int(SHAPE::NOISE)]    = EnvGen.NewADSR (num, MixerNames[int(SHAPE::NOISE)], first_device + byte(D_A_OFF::NOISE), usecount);

    for ( int z = 0;  z < (int)SHAPE::ALL;  z++ )
        {
        Mix[z]->OutputMultiplier (0.60);
        Mix[z]->SetLevel (ESTATE::SUSTAIN, 1.0);
        }

    // Configure keyboard MIDI frequencies
    memset (this->OctaveArray, 0, sizeof (this->OctaveArray));
    if ( Settings.GetOscBank (num, this->OctaveArray) )                   // If key/note array not in storage
        {
        printf ("\t  **** Tunning data failed to load.\n\t  **** Inializing default tunning.\n");
        for ( int z = 0, m = 0;  z < FULL_KEYS; z++, m++ )          // initialize at even intervals.
            {
            this->OctaveArray[z] = (uint16_t)((float)m * CONST_MULT);     // These DtoA are 9 x 1v / octave-
            if ( this->OctaveArray[z] > MAX_DA )
                this->OctaveArray[z] = (uint16_t)((float)MAX_DA * CONST_MULT);
            }
        }

    if ( I2cDevices.IsPortValid (first_device) && I2cDevices.IsPortValid (first_device + 7) )
        {
        I2cDevices.D2Analog (PwmPortIO, 900);
        this->Valid = true;
        }
    else
        printf("\t  ** VCO %d NO USABLE D/A CHANNELS FROM DEVICE %d\n", num, first_device);
    }

//#######################################################################
void OSC_C::ClearState ()
    {
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++)
        this->Mix[z]->Clear ();
    }

//#######################################################################
void OSC_C::Clear ()
    {
    this->ClearState ();
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void OSC_C::SetTuningVolume (byte select, uint16_t level)
    {
    I2cDevices.D2Analog (this->Mix[select]->GetPortIO (), level);
    }

//#######################################################################
void OSC_C::SetTuningNote (byte note, bool active)
    {
    this->CurrentNote = note;
    DBG ("DownKey = %d\n", note);
    I2cDevices.D2Analog (OscPortIO, OctaveArray[note]);
    if ( active )
        DisplayMessage.TuningDtoA (OctaveArray[note]);
    }

//#######################################################################
void OSC_C::TuningAdjust (bool up)
    {
    OctaveArray[this->CurrentNote] += (up) ? +1 : -1;
    this->SetTuningNote (this->CurrentNote, true);
    }

//#######################################################################
void OSC_C::NoteSet (byte note, byte velocity)
    {
    this->CurrentNote = note;
    DBG ("Key > %d D/A > %d\n", note, OctaveArray[note]);

    this->ClearState ();
    I2cDevices.D2Analog (this->OscPortIO, this->OctaveArray[note]);

    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        this->Mix[z]->Start ();
    }

//#######################################################################
void OSC_C::NoteClear ()
    {
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        this->Mix[z]->End ();
    }

//#######################################################################
void OSC_C::SetRampDirection (bool data)
    {
    I2cDevices.D2Analog (this->RampDirPortIO, ( data ) ? DA_MAX : 0);
    }

//#######################################################################
void OSC_C::PulseWidth (float percent)
    {
    I2cDevices.D2Analog (this->PwmPortIO, (percent * (float)DA_MAX));
    }



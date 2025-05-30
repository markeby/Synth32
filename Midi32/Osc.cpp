//#######################################################################
// Module:     Osc.cpp
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include "Settings.h"
#include "Osc.h"
#include "Debug.h"

#ifdef DEBUG_SYNTH
static const char* Label = "VCO";
#define DBG(args...) {if(DebugOsc){ DebugMsg(Label,Number,args);}}
#else
#define DBG(args...)
#endif

using namespace OSC_N;

#define CONST_MULT      (DA_RANGE / FULL_KEYS)
static  const char*     MixerNames[] = { "sine", "triangle", "square", "saw", "pulse" };

//#######################################################################
    OSC_C::OSC_C (short num, short first_device, byte& usecount, ENVELOPE_GENERATOR_C& envgen) : EnvGen (envgen)
    {
    Valid = false;
    Number = num;
    // D/A configuration
    OscPortIO                 = first_device + byte(D_A_OFF::EXPO);
    PwmPortIO                 = first_device + byte(D_A_OFF::WIDTH);
    RampDirPortIO             = first_device + byte(D_A_OFF::DIR);
    Mix[int(SHAPE::TRIANGLE)] = EnvGen.NewADSR (num, MixerNames[int(SHAPE::TRIANGLE)], first_device + byte(D_A_OFF::TRIANGLE), usecount);
    Mix[int(SHAPE::RAMP)] = EnvGen.NewADSR (num, MixerNames[int(SHAPE::RAMP)], first_device + byte(D_A_OFF::RAMP), usecount);
    Mix[int(SHAPE::PULSE)]    = EnvGen.NewADSR (num, MixerNames[int(SHAPE::PULSE)], first_device + byte(D_A_OFF::PULSE), usecount);
    Mix[int(SHAPE::SINE)]     = EnvGen.NewADSR (num, MixerNames[int(SHAPE::SINE)], first_device + byte(D_A_OFF::SINE), usecount);
    Mix[int(SHAPE::NOISE)]    = EnvGen.NewADSR (num, MixerNames[int(SHAPE::NOISE)], first_device + byte(D_A_OFF::NOISE), usecount);

    for ( int z = 0;  z < (int)SHAPE::ALL;  z++ )
        {
        Mix[z]->OutputMultiplier (0.60);
        Mix[z]->SetLevel (ESTATE::SUSTAIN, 1.0);
        }

    // Configure keyboard MIDI frequencies
    memset (OctaveArray, 0, sizeof (OctaveArray));
    if ( Settings.GetOscBank (num, OctaveArray) )                   // If key/note array not in storage
        {
        printf ("\t  **** Tunning data failed to load.\n\t  **** Inializing default tunning.\n");
        for ( int z = 0, m = 0;  z < FULL_KEYS; z++, m++ )          // initialize at even intervals.
            {
            OctaveArray[z] = (uint16_t)((float)m * CONST_MULT);     // These DtoA are 9 x 1v / octave-
            if ( OctaveArray[z] > MAX_DA )
                OctaveArray[z] = (uint16_t)((float)MAX_DA * CONST_MULT);
            }
        }

    if ( I2cDevices.IsPortValid (first_device) && I2cDevices.IsPortValid (first_device + 7) )
        {
        I2cDevices.D2Analog (PwmPortIO, 900);

        ClearState ();
        if ( DebugOsc )
            printf("\t  >> VCO %d started for device %d\n", num, first_device);
        Valid = true;
        }
    else
        printf("\t  ** VCO %d NO USABLE D/A CHANNELS FROM DEVICE %d\n", num, first_device);
    }

//#######################################################################
void OSC_C::TuningAdjust (bool up)
    {
    OctaveArray[this->CurrentNote] += ( up ) ? -1 : +1;
    SetTuningNote (this->CurrentNote);
    }

//#######################################################################
void OSC_C::ClearState ()
    {
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++)
        Mix[z]->Clear ();
    }

//#######################################################################
void OSC_C::Clear ()
    {
    ClearState ();
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void OSC_C::SetTuningVolume (byte select, uint16_t level)
    {
    I2cDevices.D2Analog (Mix[select]->GetPortIO (), level);
    }

//#######################################################################
void OSC_C::SetTuningNote (byte note)
    {
    CurrentNote = note;
    DBG ("DownKey = %d\n", note);
    I2cDevices.D2Analog (OscPortIO, OctaveArray[note]);
    }

//#######################################################################
void OSC_C::NoteSet (byte note, byte velocity)
    {
    CurrentNote = note;
    DBG ("Key > %d D/A > %d\n", note, OctaveArray[note]);

    ClearState ();
    I2cDevices.D2Analog (OscPortIO, OctaveArray[note]);

    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        Mix[z]->Start ();
    }

//#######################################################################
void OSC_C::NoteClear ()
    {
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        Mix[z]->End ();
    }

//#######################################################################
void OSC_C::SetRampDirection (bool data)
    {
    I2cDevices.D2Analog (RampDirPortIO, ( data ) ? DA_MAX : 0);
    }

//#######################################################################
void OSC_C::PulseWidth (float percent)
    {
    I2cDevices.D2Analog (PwmPortIO, (percent * (float)DA_MAX));
    }



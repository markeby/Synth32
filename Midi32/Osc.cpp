//#######################################################################
// Module:     Osc.cpp
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include <array>

#include "Config.h"
#include "Settings.h"
#include "Osc.h"
#include "I2Cmessages.h"

#ifdef DEBUG_SYNTH
static const char* Label = "VCO";
#define DBG(args...) {if(DebugSynth){ DebugMsg(Label,Number,args);}}
#else
#define DBG(args...)
#endif

using namespace OSC_N;

static  const char*     MixerNames[] = { "sine", "triangle", "square", "saw", "pulse" };

//#######################################################################
    OSC_C::OSC_C ()
    {
    Valid = false;
    }

//#######################################################################
void OSC_C::Begin (short num, short first_device, byte& usecount)
    {
    Number = num;
    // D/A configuration
    OscPortIO                 = first_device + byte(D_A_OFF::EXPO);
    PwmPortIO                 = first_device + byte(D_A_OFF::WIDTH);
    RampDirPortIO             = first_device + byte(D_A_OFF::DIR);
    Mix[int(SHAPE::TRIANGLE)] = EnvelopeGenerator.NewADSR
                                (num, MixerNames[int(SHAPE::TRIANGLE)], first_device + byte(D_A_OFF::TRIANGLE), DA_MAX, usecount);
    Mix[int(SHAPE::RAMP)]     = EnvelopeGenerator.NewADSR
                                (num, MixerNames[int(SHAPE::RAMP)], first_device + byte(D_A_OFF::RAMP), DA_MAX, usecount);
    Mix[int(SHAPE::PULSE)]    = EnvelopeGenerator.NewADSR
                                (num, MixerNames[int(SHAPE::PULSE)], first_device + byte(D_A_OFF::PULSE), DA_MAX, usecount);
    Mix[int(SHAPE::SINE)]     = EnvelopeGenerator.NewADSR
                                (num, MixerNames[int(SHAPE::SINE)], first_device + byte(D_A_OFF::SINE), DA_MAX, usecount);
    Mix[int(SHAPE::NOISE)]    = EnvelopeGenerator.NewADSR
                                (num, MixerNames[int(SHAPE::NOISE)], first_device + byte(D_A_OFF::NOISE), DA_MAX, usecount);

    for ( int z = 0;  z < (int)SHAPE::ALL;  z++ )
        Mix[z]->SetLevel (ESTATE::SUSTAIN, 1.0);

    _OctaveArray = SynthConfig.GetOctaveArray(num);           // get pointer to keyboard MIDI frequencies

    if ( I2cDevices.IsPortValid (first_device) && I2cDevices.IsPortValid (first_device + 7) )
       {
       I2cDevices.D2Analog (PwmPortIO, 900);
       Valid = true;
       }
    else
       printf("\t  ** VCO %d NO USABLE D/A CHANNELS FROM DEVICE %d\n", num, first_device);
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
    I2cDevices.Update ();     // Update D/A ports
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
    I2cDevices.D2Analog (OscPortIO, _OctaveArray[note]);
    CurrentDA = _OctaveArray[note];
    }

//#######################################################################
void OSC_C::TuningAdjust (bool up)
    {
    _OctaveArray[CurrentNote] += (up) ? +1 : -1;
    SetTuningNote (CurrentNote);
    DisplayMessage.TuningDtoA (CurrentDA);
    }

//#######################################################################
void OSC_C::NoteSet (byte note, byte velocity)
    {
    CurrentNote = note;
    DBG ("Key > %d D/A > %d\n", note, _OctaveArray[note]);

    ClearState ();
    I2cDevices.D2Analog (OscPortIO, _OctaveArray[note]);

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

//#######################################################################
void OSC_C::Mute (bool state)
    {
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        Mix[z]->Mute (state);
    }

//#######################################################################
void OSC_C::Expression (float level)
    {
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        Mix[z]->Expression (level);
    }

//#######################################################################
void OSC_C::Damper (bool state)
    {
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        Mix[z]->Damper (state);
    }


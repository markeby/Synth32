//#######################################################################
// Module:     Osc.cpp
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include "config.h"
#include "Osc.h"
#include "Debug.h"
static const char* Label = "VCO";
#define DBG(args...) {if(DebugOsc){ DebugMsg(Label,Number,args);}}

using namespace OSC_N;

#define CONST_MULT      (DA_RANGE / FULL_KEYS)
static  const char*     MixerNames[] = { "sine", "triangle", "square", "saw", "pulse" };

//#######################################################################
SYNTH_OSC_C::SYNTH_OSC_C (uint8_t num, uint8_t first_device, uint8_t& usecount, ENVELOPE_GENERATOR_C& envgen) : EnvGen (envgen)
    {
    Valid = false;
    Number = num;
    // D/A configuration
    OscChannel                = first_device + uint8_t(D_A_OFF::EXPO);
    PwmChannel                = first_device + uint8_t(D_A_OFF::WIDTH);
    SawtoothDirChannel        = first_device + uint8_t(D_A_OFF::DIR);
    Mix[int(SHAPE::TRIANGLE)] = EnvGen.NewADSR (num, MixerNames[int(SHAPE::TRIANGLE)], first_device + uint8_t(D_A_OFF::TRIANGLE), usecount);
    Mix[int(SHAPE::SAWTOOTH)] = EnvGen.NewADSR (num, MixerNames[int(SHAPE::SAWTOOTH)], first_device + uint8_t(D_A_OFF::SAWTOOTH), usecount);
    Mix[int(SHAPE::PULSE)]    = EnvGen.NewADSR (num, MixerNames[int(SHAPE::PULSE)], first_device + uint8_t(D_A_OFF::PULSE), usecount);
    Mix[int(SHAPE::SINE)]     = EnvGen.NewADSR (num, MixerNames[int(SHAPE::SINE)], first_device + uint8_t(D_A_OFF::SINE), usecount);
    Mix[int(SHAPE::SQUARE)]   = EnvGen.NewADSR (num, MixerNames[int(SHAPE::SQUARE)], first_device + uint8_t(D_A_OFF::SQUARE), usecount);

    for ( int z = 0;  z < (int)SHAPE::ALL;  z++ )
        Mix[z]->OutputMultiplier (0.60);

    // Configure keyboard MIDI frequencies
    memset (OctaveArray, 0, sizeof (OctaveArray));
    for ( int z = 0, m = 0;  z < FULL_KEYS; z++, m++ )
        {
        OctaveArray[z] = (uint16_t)((float)m * CONST_MULT);     // These DtoA are 9 x 1v / octave-
        if ( OctaveArray[z] > MAX_DA )
            OctaveArray[z] = (uint16_t)((float)MAX_DA * CONST_MULT);
        }

    if ( I2cDevices.IsChannelValid (first_device) && I2cDevices.IsChannelValid (first_device + 7) )
        {
        I2cDevices.D2Analog (PwmChannel, 900);

        ClearState ();
        printf("\t  >> VCO %d started for device %d\n", num, first_device);
        Valid = true;
        }
    else
        printf("\t  ** VCO %d NO USABLE D/A CHANNELS FROM DEVICE %d\n", num, first_device);
    }

//#######################################################################
void SYNTH_OSC_C::TuningAdjust (bool up)
    {
    OctaveArray[this->CurrentNote] += ( up ) ? -1 : +1;
    SetTuningNote (this->CurrentNote);
    }

//#######################################################################
void SYNTH_OSC_C::ClearState ()
    {
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++)
        Mix[z]->Clear ();
    }

//#######################################################################
void SYNTH_OSC_C::Clear ()
    {
    ClearState ();
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_OSC_C::SetTuningVolume (uint8_t select, uint16_t level)
    {
    I2cDevices.D2Analog (Mix[select]->GetChannel (), level);
    }

//#######################################################################
void SYNTH_OSC_C::SetTuningNote (uint8_t note)
    {
    CurrentNote = note;
    DBG ("DownKey = %d\n", note);
    I2cDevices.D2Analog (OscChannel, OctaveArray[note]);
    }

//#######################################################################
void SYNTH_OSC_C::NoteSet (uint8_t note, uint8_t velocity)
    {
    CurrentNote = note;
    DBG ("Key > %d D/A > %d\n", note, OctaveArray[note]);

    ClearState ();
    I2cDevices.D2Analog (OscChannel, OctaveArray[note]);

    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        Mix[z]->Start ();
    }

//#######################################################################
void SYNTH_OSC_C::NoteClear ()
    {
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        Mix[z]->End ();
    }

//#######################################################################
void SYNTH_OSC_C::SawtoothDirection (bool data)
    {
    I2cDevices.D2Analog (SawtoothDirChannel, ( data ) ? DA_MAX : 0);
    }

//#######################################################################
void SYNTH_OSC_C::PulseWidth (float percent)
    {
    I2cDevices.D2Analog (PwmChannel, (percent * (float)DA_MAX));
    }

//#######################################################################
void SYNTH_OSC_C::SoftLFO (uint8_t wave, bool state)
    {
    Mix[wave]->SoftLFO (state);
    }

//#######################################################################
void SYNTH_OSC_C::SetAttackTime (uint8_t wave, float time)
    {
    Mix[wave]->SetTime (ESTATE::ATTACK, time);
    }

//#######################################################################
void SYNTH_OSC_C::SetDecayTime (uint8_t wave, float time)
    {
     Mix[wave]->SetTime (ESTATE::DECAY, time);
    }

//#######################################################################
void SYNTH_OSC_C::SetReleaseTime (uint8_t wave, float time)
    {
    Mix[wave]->SetTime (ESTATE::RELEASE, time);
    }

//#######################################################################
void SYNTH_OSC_C::SetSustainLevel (uint8_t wave, float level_percent)
    {

    Mix[wave]->SetLevel (ESTATE::SUSTAIN, level_percent);
    }

//#######################################################################
void SYNTH_OSC_C::SetSustainTime (uint8_t wave, float time)
    {
    Mix[wave]->SetTime (ESTATE::SUSTAIN, time);
    }

//#######################################################################
void SYNTH_OSC_C::SetMaxLevel (uint8_t wave, float level_percent)
    {
    Mix[wave]->SetLevel(ESTATE::ATTACK, level_percent);
    }


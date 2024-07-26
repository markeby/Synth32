//#######################################################################
// Module:     SyntFront.ino
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include "../Common/DispMessages.h"
#include "config.h"
#include "LFOosc.h"
#include "Noise.h"
#include "Debug.h"
static const char* Label = "NOI";
#define DBG(args...) {if(DebugOsc){ DebugMsg(Label,Number,args);}}

using namespace NOISE_N;

//#######################################################################
    SYNTH_NOISE_C::SYNTH_NOISE_C (uint8_t num, int analog, int digital, uint8_t& usecount, ENVELOPE_GENERATOR_C& envgen) : EnvGen (envgen), Number (num)
    {
    Analog[(int)SELECT::VCA] = analog + 1;
    Analog[(int)SELECT::VCF] = analog;
    Envelope[(int)SELECT::VCF] = EnvGen.NewADSR (num, "Noise VCF", Analog[(int)SELECT::VCF], usecount);
    Envelope[(int)SELECT::VCA] = EnvGen.NewADSR (num, "Noise VCA", Analog[(int)SELECT::VCA], usecount);

    FilterDigital[0] = digital - 1;
    FilterDigital[1] = digital;
    ClearState ();
    printf("\t  >> Noise %d started for D/A %d  Dig %d\n", num, analog, digital);
    }

//#######################################################################
void SYNTH_NOISE_C::SetTuningVolume (uint8_t select, uint16_t level)
    {
    if ( select > 1 )
        return;
    uint8_t chan = Envelope[select]->GetChannel ();
    I2cDevices.D2Analog (chan, level);
    }

//#######################################################################
void SYNTH_NOISE_C::TuningFilterCut (float cutoff)
    {
    DBG ("Filter cutoff %f", cutoff);
    uint16_t val = cutoff * DA_RANGE;
    I2cDevices.D2Analog (Analog[(int)SELECT::VCF], val);
    }

//#######################################################################
void SYNTH_NOISE_C::FilterSelect (uint8_t select)
    {
    FilterSelected = select;

    DBG ("Filter select %d", select);
    I2cDevices.DigitalOut (FilterDigital[0], ((select >> 1) & 1) ^ 1);
    I2cDevices.DigitalOut (FilterDigital[1], (select & 1) ^ 1);
    }

//#######################################################################
void SYNTH_NOISE_C::ClearState ()
    {
    for ( int z = 0;  z < FILTER_ANALOG_COUNT;  z++)
        Envelope[z]->Clear ();
    }

//#######################################################################
void SYNTH_NOISE_C::Clear ()
    {
    ClearState ();
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_NOISE_C::SetLevel (uint16_t level)
    {
    DBG ("Filter level %d", level);
    I2cDevices.D2Analog (Analog[(int)SELECT::VCA], level);
    }

//#######################################################################
void SYNTH_NOISE_C::SetAttackTime (uint8_t sel, float time)
    {
    Envelope[sel]->SetTime (ESTATE::ATTACK, time);
    }

//#######################################################################
void SYNTH_NOISE_C::SetDecayTime (uint8_t sel, float time)
    {
    Envelope[sel]->SetTime (ESTATE::DECAY, time);
    }

//#######################################################################
void SYNTH_NOISE_C::SetReleaseTime (uint8_t sel, float time)
    {
    Envelope[sel]->SetTime (ESTATE::RELEASE, time);
    }

//#######################################################################
void SYNTH_NOISE_C::SetSustainLevel (uint8_t sel, float level_percent)
    {
    Envelope[sel]->SetLevel (ESTATE::SUSTAIN, level_percent);
    }

//#######################################################################
void SYNTH_NOISE_C::SetSustainTime (uint8_t sel, float time)
    {
    Envelope[sel]->SetTime (ESTATE::SUSTAIN, time);
    }

//#######################################################################
void SYNTH_NOISE_C::SetBaseLevel (NOISE_N::SELECT sel, float level_percent)
    {
    if ( sel == SELECT::VCF )
        Envelope[(int)sel]->SetLevel(ESTATE::START, level_percent);
    }

//#######################################################################
void SYNTH_NOISE_C::SetMaxLevel (uint8_t sel, float level_percent)
    {
    Envelope[sel]->SetLevel(ESTATE::ATTACK, level_percent);
    }

//#######################################################################
void SYNTH_NOISE_C::NoteSet (uint8_t note, uint8_t velocity)
    {
    CurrentNote = note;
    DBG ("Note > %d   Vel > %d", note, velocity);

    ClearState ();

    for ( int z = 0;  z < 2;  z++ )
        Envelope[z]->Start ();
    }

//#######################################################################
void SYNTH_NOISE_C::NoteClear ()
    {
    for ( int z = 0;  z < 2;  z++ )
        Envelope[z]->End ();
    }



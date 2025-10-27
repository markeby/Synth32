//#######################################################################
// Module:     LFOusc.cpp
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       9/18/2024
//#######################################################################
#include <Arduino.h>

#include "Config.h"
#include "I2Cmessages.h"
#include "Settings.h"
#include "LFOosc.h"
#include "Debug.h"

#ifdef DEBUG_SYNTH
static const char* Label = "LFO";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,Number,args);}}
#else
#define DBG(args...)
#endif

static  const char*     _MixerNames[] = { "sine", "saw", "pulse" };

//#######################################################################
SYNTH_LFO_C::SYNTH_LFO_C ()
    {
    this->Valid         = false;
    this->UpdateNeded   = false;
    this->CurrentLevel  = 0;
    this->CurrentFreq   = 0.0;
    this->ResetOn       = false;
    this->RampSlope     = false;
    this->InUse         = 0;
    this->Midi          = 1;
    this->Offset        = 0;
    }

//#######################################################################
void SYNTH_LFO_C::ClearState ()
    {
    for ( int z = 0;  z < SOURCE_CNT_LFO;  z++)
        {
        SYNTH_LFO_C::WAVE_LEVEL_T& m = this->Level[z];
        I2cDevices.D2Analog (m.Port, 0);
        }
    }

//#######################################################################
void SYNTH_LFO_C::SetLevel (uint8_t ch, uint8_t data)
    {
    int z = (int)((data * 0.007874) * (float)MAX_DA);
    this->Level[ch].MaximumLevel =  (z > MAX_DA ) ? MAX_DA : z;

    DBG ("Level limit > %d", this->Level[ch].MaximumLevel);
    }

//#######################################################################
void SYNTH_LFO_C::SetInternalLevel (uint8_t ch, uint8_t data)
    {
    int z = (int)((data * 0.007874) * (float)MAX_DA);
    z = (z > Level[ch].MaximumLevel ) ? Level[ch].MaximumLevel : z;

    if ( z != Level[ch].CurrentLevel )
        {
        Level[ch].CurrentLevel = z;
        I2cDevices.D2Analog (Level[ch].Port,  z);
        UpdateNeded = true;
        DBG ("%s level > %d", Level[ch].Name, z);
        }
    }

//#######################################################################
//#######################################################################
void SYNTH_LFO_C::Begin (int num, uint8_t first_device, uint8_t lfo_digital)
    {
    Number = num;
    // D/A configurations
    OscPortIO  = first_device + uint8_t(LFO_D_A_OFF::FREQ);
    PwmPortIO  = first_device + uint8_t(LFO_D_A_OFF::WIDTH);
    BendPortIO = first_device + uint8_t(LFO_D_A_OFF::BEND);
    Level[int(LFO_SHAPE::RAMP)].Port  = first_device + uint8_t(LFO_D_A_OFF::RAMP);
    Level[int(LFO_SHAPE::PULSE)].Port = first_device + uint8_t(LFO_D_A_OFF::PULSE);
    Level[int(LFO_SHAPE::SINE)].Port  = first_device + uint8_t(LFO_D_A_OFF::SINE);
    SlopePortO       = lfo_digital;
    HardResetPortIO  = lfo_digital + 1;
    Midi             = 0;

    // Initialize mixers

    for ( int z = 0;  z < SOURCE_CNT_LFO;  z++ )
        {
        Level[z].Name         = _MixerNames[z];
        Level[z].CurrentLevel = 0;
        Level[z].MaximumLevel = MAX_DA;
        }

    if ( I2cDevices.IsPortValid (first_device) && I2cDevices.IsPortValid (first_device + 5) )
        {
        Valid = true;
        Offset = Settings.GetOffsetLFO (num);
        ClearState ();
        PitchBend (PITCH_BEND_CENTER);
        if ( DebugSynth )
            printf ("\t  >> LFO %d started for device %d\n", this->Number, first_device);
        }
    else
        printf ("\t  ** LFO %d NO USABLE D/A CHANNELS FROM DEVICE %d\n", num, first_device);
    }

//#######################################################################
void SYNTH_LFO_C::Clear ()
    {
    this->ClearState ();
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_LFO_C::ProcessFreq ()
    {
    CurrentFreq = (CurrentFreqCoarse * MIDI_MULTIPLIER) + (CurrentFreqFine * (MIDI_MULTIPLIER * 0.01));
    if ( CurrentFreq > DA_MAX )
        CurrentFreq = DA_MAX;
    OutputFreqIO ();
    }

//#######################################################################
void SYNTH_LFO_C::OutputFreqIO ()
    {
    DBG ("Set frequency %d", this->CurrentFreq);
    I2cDevices.D2Analog (this->OscPortIO, this->CurrentFreq);
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_LFO_C::SetPulseWidth (short value)
    {
    this->CurrentWidth = value;
    DBG ("Set pulse width %d", value);
    I2cDevices.D2Analog (this->PwmPortIO, DA_MAX - value);
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_LFO_C::SetWave (short ch, bool state)
    {
    this->Level[ch].Select = state;
    if ( state )
        {
        InUse++;
        SetInternalLevel (ch, CurrentLevel);
        }
    else
        {
        InUse--;
        SetInternalLevel (ch, 0);
        }

    if ( UpdateNeded )
        {
        I2cDevices.UpdateAnalog ();     // Update D/A ports
        UpdateNeded = false;
        }

    DBG ("%s(%d) selected %s", Level[ch].Name, ch, ((state) ? "ON" : "off"));
    }

//#######################################################################
void SYNTH_LFO_C::SetLevelMidi (byte mchan, uint8_t data)
    {
    if ( mchan == this->Midi )
        {
        this->CurrentLevel = data;
        for (int z = 0;  z < SOURCE_CNT_LFO;  z++ )
            {
            if ( this->Level[z].Select )
                this->SetInternalLevel (z, data);
            else
                this->SetInternalLevel (z, 0);
            }
        if ( this->UpdateNeded )
            {
            I2cDevices.UpdateAnalog ();     // Update D/A ports
            this->UpdateNeded = false;
            }
        if ( InUse )
            DisplayMessage.LfoHardLevel (this->Number, data);
        }
    }

//#######################################################################
void SYNTH_LFO_C::PitchBend (short value)
    {
    I2cDevices.D2Analog (this->BendPortIO, value + this->Offset);
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_LFO_C::SetRampDir (bool state)
    {
    this->RampSlope = state;
    I2cDevices.DigitalOut           (this->SlopePortO, state);
    I2cDevices.UpdateDigital        ();
    }

//#######################################################################
void SYNTH_LFO_C::HardReset (byte mchan)
    {
    if ( mchan == this->Midi )
        {
        I2cDevices.DigitalOut (this->HardResetPortIO, true);
        this->ResetOn = true;
        }
    }

//#######################################################################
void SYNTH_LFO_C::Loop (void)
    {
    if (  this->ResetOn )
        {
        I2cDevices.DigitalOut (this->HardResetPortIO, false);
        this->ResetOn = false;
        }
    }


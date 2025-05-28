//#######################################################################
// Module:     LFOusc.cpp
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       9/18/2023
//#######################################################################
#include <Arduino.h>
#include "../Common/SynthCommon.h"
#include "I2Cmessages.h"
#include "Settings.h"
#include "LFOosc.h"
#include "Debug.h"

#ifdef DEBUG_SYNTH
static const char* Label = "LFO";
#define DBG(args...) {if(DebugOsc){DebugMsg(Label,Number,args);}}
#else
#define DBG(args...)
#endif

#define CONST_MULT      (DA_RANGE / FULL_KEYS)
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
void SYNTH_LFO_C::SetMaxLevel (uint8_t ch, uint8_t data)
    {
    int z = (int)((data * 0.007874) * (float)MAX_DA);
    this->Level[ch].MaximumLevel =  (z > MAX_DA ) ? MAX_DA : z;

    DBG ("Level limit > %d", this->Level[ch].MaximumLevel);
    }

//#######################################################################
void SYNTH_LFO_C::SetLevel (uint8_t ch, uint8_t data)
    {
    int z = (int)((data * 0.007874) * (float)MAX_DA);
    z = (z > this->Level[ch].MaximumLevel ) ? this->Level[ch].MaximumLevel : z;

    if ( z != this->Level[ch].CurrentLevel )
        {
        this->Level[ch].CurrentLevel = z;
        I2cDevices.D2Analog (this->Level[ch].Port,  z);
        this->UpdateNeded = true;
        DBG ("%s level > %d", this->Level[ch].Name, z);
        }
    }

//#######################################################################
//#######################################################################
void SYNTH_LFO_C::Begin (int num, uint8_t first_device, uint8_t lfo_digital)
    {
    this->Number = num;
    // D/A configurations
    this->OscPortIO  = first_device + uint8_t(D_A_OFF::FREQ);
    this->PwmPortIO  = first_device + uint8_t(D_A_OFF::WIDTH);
    this->BendPortIO = first_device + uint8_t(D_A_OFF::BEND);
    this->Level[int(SHAPE::RAMP)].Port = first_device + uint8_t(D_A_OFF::RAMP);
    this->Level[int(SHAPE::PULSE)].Port    = first_device + uint8_t(D_A_OFF::PULSE);
    this->Level[int(SHAPE::SINE)].Port     = first_device + uint8_t(D_A_OFF::SINE);
    this->SlopePortO       = lfo_digital;
    this->HardResetPortIO  = lfo_digital + 1;
    this->Midi             = 0;

    // Initialize mixers

    for ( int z = 0;  z < SOURCE_CNT_LFO;  z++ )
        {
        this->Level[z].Name         = _MixerNames[z];
        this->Level[z].CurrentLevel = 0;
        this->Level[z].MaximumLevel = MAX_DA;
        }

    if ( I2cDevices.IsPortValid (first_device) && I2cDevices.IsPortValid (first_device + 7) )
        {
        this->Valid = true;
        this->Offset = Settings.GetOffsetLFO (num);
        this->ClearState ();
        this->PitchBend (PITCH_BEND_CENTER);
        if ( DebugOsc )
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
void SYNTH_LFO_C::SetFreq (short value)
    {
    this->CurrentFreq = value;
    DBG ("Set frequency %d", value);
    I2cDevices.D2Analog (this->OscPortIO, value);
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
    DisplayMessage.LfoHardSelect (this->Number, ch, state);
    if ( state )
        {
        this->InUse++;
        this->SetLevel (ch, this->CurrentLevel);
        }
    else
        {
        this->InUse--;
        this->SetLevel (ch, 0);
        }


    if ( this->UpdateNeded )
        {
        I2cDevices.UpdateAnalog ();     // Update D/A ports
        this->UpdateNeded = false;
        }

    DBG ("%s(%d) selected %s", this->Level[ch].Name, ch, ((state) ? "ON" : "off"));
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
                this->SetLevel (z, data);
            else
                this->SetLevel (z, 0);
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
    DisplayMessage.LfoHardRampSlope (this->Number, state);
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


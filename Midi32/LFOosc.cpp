//#######################################################################
// Module:     LFOusc.cpp
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       9/18/2023
//#######################################################################
#include <Arduino.h>
#include "../Common/SynthCommon.h"
#include "config.h"
#include "I2Cmessages.h"
#include "LFOosc.h"
#include "Debug.h"

#ifdef DEBUG_ON
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
    }

//#######################################################################
void SYNTH_LFO_C::ClearState ()
    {
    for ( int z = 0;  z < LFO_VCA_COUNT;  z++)
        {
        SYNTH_LFO_C::VCA_T& m = this->Vca[z];
        I2cDevices.D2Analog (m.Channel, 0);
        }
    }

//#######################################################################
void SYNTH_LFO_C::SetMaxLevel (uint8_t ch, uint8_t data)
    {
    int z = (int)((data * 0.007874) * (float)MAX_DA);
    this->Vca[ch].MaximumLevel =  (z > MAX_DA ) ? MAX_DA : z;

    DBG ("Level limit > %d", this->Vca[ch].MaximumLevel);
    }

//#######################################################################
void SYNTH_LFO_C::SetLevel (uint8_t ch, uint8_t data)
    {
    int z = (int)((data * 0.007874) * (float)MAX_DA);
    z = (z > this->Vca[ch].MaximumLevel ) ? this->Vca[ch].MaximumLevel : z;

    if ( z != this->Vca[ch].CurrentLevel )
        {
        this->Vca[ch].CurrentLevel = z;
        I2cDevices.D2Analog (this->Vca[ch].Channel,  z);
        this->UpdateNeded = true;
        DBG ("%s level > %d", this->Vca[ch].Name, z);
        }
    }

//#######################################################################
//#######################################################################
void SYNTH_LFO_C::Begin (int num, uint8_t first_device, uint8_t lfo_digital)
    {
    this->Number = num;
    // D/A configurations
    this->OscChannelIO = first_device + uint8_t(D_A_OFF::FREQ);
    this->PwmChannelIO = first_device + uint8_t(D_A_OFF::WIDTH);
    this->BendChannelIO = first_device + uint8_t(D_A_OFF::BEND);
    this->Vca[int(SHAPE::SAWTOOTH)].Channel = first_device + uint8_t(D_A_OFF::SAWTOOTH);
    this->Vca[int(SHAPE::PULSE)].Channel    = first_device + uint8_t(D_A_OFF::PULSE);
    this->Vca[int(SHAPE::SINE)].Channel     = first_device + uint8_t(D_A_OFF::SINE);
    this->SlopeChannelIO = lfo_digital;
    this->HardResetChannelIO  = lfo_digital + 1;

    // Initialize mixers

    for ( int z = 0;  z < LFO_VCA_COUNT;  z++ )
        {
        this->Vca[z].Name         = _MixerNames[z];
        this->Vca[z].CurrentLevel = 0;
        this->Vca[z].MaximumLevel = MAX_DA;
        }

    if ( I2cDevices.IsChannelValid (first_device) && I2cDevices.IsChannelValid (first_device + 7) )
        {
        this->Valid = true;
        this->PitchBend (2048);
        this->ClearState ();
        if ( DebugOsc )
            printf("\t  >> LFO %d started for device %d\n", this->Number, first_device);
        }
    else
        printf("\t  ** LFO %d NO USABLE D/A CHANNELS FROM DEVICE %d\n", num, first_device);
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
    I2cDevices.D2Analog (this->OscChannelIO, value);
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_LFO_C::SetPulseWidth (short value)
    {
    this->CurrentWidth = value;
    DBG ("Set pulse width %d", value);
    I2cDevices.D2Analog (this->PwmChannelIO, value);
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_LFO_C::SetWave (short ch, bool state)
    {
    Vca[ch].Select = state;
    DisplayMessage.LfoHardSelect (ch, state);
    if ( state )
        {
        this->InUse++;
        this->SetLevel (ch, CurrentLevel);
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

    DBG ("%s(%d) selected %s", this->Vca[ch].Name, ch, ((state) ? "ON" : "off"));
    }

//#######################################################################
void SYNTH_LFO_C::SetLevel (uint8_t data)
    {
    this->CurrentLevel = data;
    for (int z = 0;  z < LFO_VCA_COUNT;  z++ )
        {
        if ( this->Vca[z].Select )
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
        DisplayMessage.LfoHardLevel (data);
    }

//#######################################################################
void SYNTH_LFO_C::PitchBend (short value)
    {
    I2cDevices.D2Analog (this->BendChannelIO, value);
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_LFO_C::SetRampDir (bool state)
    {
    this->RampSlope = state;
    DisplayMessage.LfoHardRampSlope (state);
    I2cDevices.DigitalOut (this->SlopeChannelIO, state);
    I2cDevices.UpdateDigital ();
    }

//#######################################################################
void SYNTH_LFO_C::HardReset (void)
    {
    I2cDevices.DigitalOut (this->HardResetChannelIO, true);
    this->ResetOn = true;
    }

//#######################################################################
void SYNTH_LFO_C::Loop (void)
    {
    if (  this->ResetOn )
        {
        I2cDevices.DigitalOut(this->HardResetChannelIO, false);
        this->ResetOn = false;
        }
    }


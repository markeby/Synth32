//#######################################################################
// Module:     LFOusc.cpp
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       9/18/2023
//#######################################################################
#include <Arduino.h>
#include "../Common/SynthCommon.h"
#include "config.h"
#include "LFOosc.h"
#include "Debug.h"
static const char* Label = "LFO";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,Number,args);}}
#define EDBG(args...) {if(DebugOsc){DebugMsgN(Label,Number,Vca[ch].Name,args);}}

using namespace LFO_N;

#define CONST_MULT      (DA_RANGE / FULL_KEYS)
static  const char*     MixerNames[] = { "sine", "triangle", "saw", "pulse" };

//#######################################################################
SYNTH_LFO_C::SYNTH_LFO_C ()
    {
    Valid       = false;
    UpdateNeded = false;
    FreqDiv     = MAX_FREQ_DEV;
    }

//#######################################################################
void SYNTH_LFO_C::ClearState ()
    {
    for ( int z = 0;  z < LFO_VCA_COUNT;  z++)
        {
        VCA_T& m = Vca[z];
        I2cDevices.D2Analog(m.Channel, 0);
        }
    }

//#######################################################################
void SYNTH_LFO_C::SetMaxLevel (uint8_t ch, uint8_t data)
    {
    int z = (int)((data * 0.007874) * (float)MAX_DA);
    Vca[ch].MaximumLevel =  (z > MAX_DA ) ? MAX_DA : z;

    DBG ("Level limit > %d", Vca[ch].MaximumLevel);
    }

//#######################################################################
void SYNTH_LFO_C::SetLevel (uint8_t ch, uint8_t data)
    {
    int z = (int)((data * 0.007874) * (float)MAX_DA);
    z = (z > Vca[ch].MaximumLevel ) ? Vca[ch].MaximumLevel : z;

    if ( z != Vca[ch].CurrentLevel )
        {
        Vca[ch].CurrentLevel = z;
        I2cDevices.D2Analog (Vca[ch].Channel,  z);
        UpdateNeded = true;
        EDBG ("set > %d", z);
        }
    }

//#######################################################################
//#######################################################################
void SYNTH_LFO_C::Begin (int num, uint8_t first_device)
    {
    Number = num;
    // D/A configuration
    OscChannel = first_device + uint8_t(D_A_OFF::FREQ);
    PwmChannel = first_device + uint8_t(D_A_OFF::WIDTH);
    BendChannel = first_device + uint8_t(D_A_OFF::BEND);
    Vca[int(SHAPE::TRIANGLE)].Channel = first_device + uint8_t(D_A_OFF::TRIANGLE);
    Vca[int(SHAPE::SAWTOOTH)].Channel = first_device + uint8_t(D_A_OFF::SAWTOOTH);
    Vca[int(SHAPE::PULSE)].Channel    = first_device + uint8_t(D_A_OFF::PULSE);
    Vca[int(SHAPE::SINE)].Channel     = first_device + uint8_t(D_A_OFF::SINE);

    // Initialize mixer
    for ( int z = 0;  z < LFO_VCA_COUNT;  z++ )
        {
        Vca[z].Name         = MixerNames[z];
        Vca[z].CurrentLevel = 0;
        Vca[z].MaximumLevel = MAX_DA;
        }

    if ( I2cDevices.IsChannelValid (first_device) && I2cDevices.IsChannelValid (first_device + 7) )
        {
        Valid = true;
        PitchBend (50.0);
        ClearState ();
        printf ("\t  >> LFO %d started for device %d\n", Number, first_device);
        }
    else
        printf("\t  ** LFO %d NO USABLE D/A CHANNELS FROM DEVICE %d\n", num, first_device);
    }

//#######################################################################
void SYNTH_LFO_C::Clear ()
    {
    ClearState ();
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_LFO_C::Range (bool up)
    {
    if ( up )
        {
        if ( FreqDiv < MAX_FREQ_DEV )
                 FreqDiv <<= 1;
        }
    else
        {
        if ( FreqDiv > 1 )
                 FreqDiv >>= 1;
        }
    SetFreq (CurrentPercent);
    }

//#######################################################################
void SYNTH_LFO_C::SetFreq (uint8_t data)
    {
    CurrentPercent = data * PRS_SCALER;
    int z = (CurrentPercent * MAX_DA) / FreqDiv;
    I2cDevices.D2Analog (OscChannel, z);
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    DBG ("%f%", CurrentPercent);
    }

//#######################################################################
void SYNTH_LFO_C::Select (uint8_t ch, bool sel)
    {
    Vca[ch].Select = sel;
    if ( sel )
        SetLevel (ch, CurrentLevel);
    else
        SetLevel (ch, 0);

    if ( UpdateNeded )
        {
        I2cDevices.UpdateAnalog ();     // Update D/A ports
        UpdateNeded = false;
        }

    EDBG ("%s", ((sel) ? " ON " : " off"));
    }

//#######################################################################
void SYNTH_LFO_C::Level (uint8_t data)
    {
    CurrentLevel = data * PRS_SCALER * MAX_DA;
    for (int z = 0;  z < LFO_VCA_COUNT;  z++ )
        {
        if ( Vca[z].Select )
            SetLevel (z, CurrentLevel);
        else
            SetLevel (z, 0);
        }
    if ( UpdateNeded )
        {
        I2cDevices.UpdateAnalog ();     // Update D/A ports
        UpdateNeded = false;
        }
    }

//#######################################################################
void SYNTH_LFO_C::PitchBend (float percent)
    {
    if ( percent < 1.0 )
        percent = 1.0;
    I2cDevices.D2Analog(BendChannel, (percent * 0.01) * (float)MAX_DA);
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }



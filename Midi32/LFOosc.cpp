//#######################################################################
// Module:     LFOusc.cpp
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       9/18/2023
//#######################################################################
#include <Arduino.h>
#include "config.h"
#include "LFOosc.h"

using namespace LFO_N;

#define CONST_MULT      (DA_RANGE / FULL_KEYS)
static  const char*     MixerNames[] = { "sine", "triangle", "saw", "pulse" };

//#######################################################################
SYNTH_LFO_C::SYNTH_LFO_C ()
    {
    Valid = false;
    Update = 0;
    FreqDiv = MAX_FREQ_DEV;
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
void SYNTH_LFO_C::SetMaxLevel (uint8_t wave, float level_percent)
    {
    int z = (int)((level_percent * 0.01) * (float)MAXDA);
    Vca[wave].MaximumLevel =  (z > MAXDA ) ? MAXDA : z;

    if ( DebugOsc )
        Serial << "\r[LFO]" << Vca[wave].Name << "Level limit > " << Vca[wave].MaximumLevel;
    }

//#######################################################################
void SYNTH_LFO_C::SetLevel (uint8_t wave, uint16_t level)
    {
    uint16_t z;

    z = (level > Vca[wave].MaximumLevel ) ? Vca[wave].MaximumLevel : level;

    if ( z != Vca[wave].CurrentLevel )
        {
        Vca[wave].CurrentLevel = z;
        I2cDevices.D2Analog (Vca[wave].Channel,  z);
        Update = true;
        if ( DebugOsc )
            Serial << "\r[LFO]" << Vca[wave].Name << "set > " << z;
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
        Vca[z].MaximumLevel = MAXDA;
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
                 FreqDiv >>= 1
            ;
        }
    SetFreq (CurrentPercent);
    }

//#######################################################################
void SYNTH_LFO_C::SetFreq (float percent)
    {
    CurrentPercent = percent;
    int z = ((percent * 0.01) * MAXDA) / FreqDiv;
    I2cDevices.D2Analog (OscChannel, z);
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    if ( DebugSynth )
        printf ("\r[LFO] %f%", percent);
    }

//#######################################################################
void SYNTH_LFO_C::Select (uint8_t wave, bool sel)
    {
    Vca[wave].Select = sel;
    if ( sel )
        SetLevel (wave, CurrentLevel);
    else
        SetLevel (wave, 0);

    if ( Update )
        {
        I2cDevices.UpdateAnalog ();     // Update D/A ports
        Update = false;
        }

    if ( DebugSynth )
       printf ("\r[LFO] %s %s",Vca[wave].Name, ((sel) ? " ON " : " off"));
    }

//#######################################################################
void SYNTH_LFO_C::Level (float percent)
    {
    CurrentLevel = (percent * 0.01) * MAXDA;
    for (int z = 0;  z < LFO_VCA_COUNT;  z++ )
        {
        if ( Vca[z].Select )
            SetLevel (z, CurrentLevel);
        else
            SetLevel (z, 0);
        }
    if ( Update )
        {
        I2cDevices.UpdateAnalog ();     // Update D/A ports
        Update = false;
        }
    }

//#######################################################################
void SYNTH_LFO_C::PitchBend (float percent)
    {
        if ( percent < 1.0 )
            percent = 1.0;
        I2cDevices.D2Analog(BendChannel, (percent * 0.01) * (float)MAXDA);
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }



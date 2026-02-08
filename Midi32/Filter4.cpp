//#######################################################################
// Module:     Filter4.cpp
// Descrption: Controls for 3 pole variable state filter with 4 outputs
// Creator:    markeby
// Date:       7/26/2025
//#######################################################################
#include <Arduino.h>
#include "Settings.h"
#include "Filter4.h"
#include "I2Cmessages.h"
#include "Debug.h"

#ifdef DEBUG_SYNTH
static const char* label = "FLT";
#define DBG(args...) {if(DebugSynth){ DebugMsg(label,_Number,args);}}
#else
#define DBG(args...)
#endif

static const char*  switchNames[] = { "LP", "LBP", "UBP", "HP" };
//#######################################################################
    FLT4_C::FLT4_C ()
    {
    _Valid  = false;
    }

//#######################################################################
void FLT4_C::Begin (short num, short first_device, byte& usecount, ENV_GENERATOR_C& envgen)
    {
    _Number = num;
    // D/A configuration

    _FreqIO   = first_device;
    _QuIO     = first_device + 1;
    _Envelope = envgen.NewADSR(num, "FLT4_C Freq", _FreqIO, usecount);
    _Envelope->SetDualUse (true);

    short dig = first_device + 4;       // get first digital switch
    if ( num & 1 )                      //   odd numbered units need 2 more
        dig += 2;

    _OutSwitch[0] = dig++;
    _OutSwitch[1] = dig++;
    _OutSwitch[2] = dig++;
    _OutSwitch[3] = dig;
    _OutMap       = 0;

    if ( I2cDevices.IsPortValid (_FreqIO) && I2cDevices.IsPortValid (_QuIO) && I2cDevices.IsPortValid (_OutSwitch[0]) )
        {
        if ( DebugSynth )
            printf("\t  >> VCF %d started for device %d\n", num, first_device);
        _Valid = true;
        }
    else
        printf("\t  ** VCF %d NO USABLE D/A CHANNELS FROM DEVICE %d\n", num, first_device);
    }

//#######################################################################
void FLT4_C::ClearState ()
    {
    _Envelope->Clear ();
    }

//#######################################################################
void FLT4_C::Clear ()
    {
    ClearState ();
    I2cDevices.Update ();     // Update D/A ports
    }

//#######################################################################
void FLT4_C::SetQ (float level_percent)
    {
    _Q = level_percent;
    short z = (short)(level_percent * (float)DA_MAX);
    I2cDevices.D2Analog (_QuIO, z);
    }

//#######################################################################
void FLT4_C::SetOutMap (byte fmap)
    {
    _OutMap = fmap;

    for ( short z = 0;  z < 4;  z++ )
        I2cDevices.DigitalOut (_OutSwitch[z], (fmap >> z) & 1);
    }

//#######################################################################
void FLT4_C::SetTuning (bool qsel, uint16_t level)
    {
    if ( qsel )
        I2cDevices.D2Analog (_QuIO, level);
    else
        _Envelope->SetOverride (level);
    }

//#######################################################################
void FLT4_C::NoteSet (byte key, byte velocity)
    {
    DBG ("FIlter Note set by %d with velocity %d", key, velocity);
    _Envelope->Clear ();
    switch ( _ControlSrc )
        {
        case ENV_CTRL_E::FIXED:
            break;
        case ENV_CTRL_E::ENVELOPE:
            _Envelope->Start (false);
            break;
        case ENV_CTRL_E::MODULATE:
            _Envelope->Start (true);
            break;
        case ENV_CTRL_E::MODWHEEL:
            break;
        default:
            break;
        }
    }

//#######################################################################
void FLT4_C::NoteClear ()
    {
    if ( _ControlSrc >= ENV_CTRL_E::ENVELOPE )
        _Envelope->End ();
    }

//#######################################################################
void FLT4_C::SetStart (float level_percent)
    {
    _Envelope->SetLevel (ESTATE::START, level_percent);

    switch ( _ControlSrc )
        {
        case ENV_CTRL_E::FIXED:
            _Envelope->SetCurrent (level_percent);
            break;
        case ENV_CTRL_E::ENVELOPE:
            break;
        case ENV_CTRL_E::MODULATE:
            break;
        case ENV_CTRL_E::MODWHEEL:
            break;
        default:
            break;
        }
    }

//#######################################################################


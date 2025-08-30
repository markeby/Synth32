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
static const char* Label = "VCO";
#define DBG(args...) {if(DebugSynth){ DebugMsg(Label,Number,args);}}
#else
#define DBG(args...)
#endif

static const char*  envNames[]    = { "Freq", "Q" };
static const char*  switchNames[] = { "LP", "LBP", "UBP", "HP" };
//#######################################################################
    FLT4_C::FLT4_C ()
    {
    Valid  = false;
    }

//#######################################################################
void FLT4_C::Begin (short num, short first_device, byte& usecount, ENVELOPE_GENERATOR_C& envgen)
    {
    Number = num;
    // D/A configuration
    FreqIO   = first_device;
    QuIO     = first_device + 1;
    Funct[0] = envgen.NewADSR (num, envNames[0], FreqIO, usecount);
    Funct[1] = envgen.NewADSR (num, envNames[1], QuIO,   usecount);

    short dig = first_device + 4;       // get first digital switch
    if ( num & 1 )                      //   odd numbered units need 2 more
        dig += 2;

    OutSwitch[0] = dig++;
    OutSwitch[1] = dig++;
    OutSwitch[2] = dig++;
    OutSwitch[3] = dig;
    OutMap       = 0;

    if ( I2cDevices.IsPortValid (FreqIO) && I2cDevices.IsPortValid (QuIO) && I2cDevices.IsPortValid (OutSwitch[0]) )
        {
        if ( DebugSynth )
            printf("\t  >> VCF %d started for device %d\n", num, first_device);
        Valid = true;
        }
    else
        printf("\t  ** VCF %d NO USABLE D/A CHANNELS FROM DEVICE %d\n", num, first_device);
    }

//#######################################################################
void FLT4_C::ClearState ()
    {
    Funct[0]->Clear ();
    Funct[1]->Clear ();
    }

//#######################################################################
void FLT4_C::Clear ()
    {
    ClearState ();
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void FLT4_C::SetOutMap (byte fmap)
    {
    OutMap = fmap;

    for ( short z = 0;  z < 4;  z++ )
        I2cDevices.DigitalOut (OutSwitch[z], (fmap >> z) & 1);
    I2cDevices.UpdateDigital ();
    }

//#######################################################################
void FLT4_C::NoteSet (byte key, byte velocity)
    {
    if ( ControlSrc[0] == FILTER_CTRL_C::ENVELOPE )
        {
        Funct[0]->Clear ();
        Funct[0]->Start ();
        }
    if ( ControlSrc[1] == FILTER_CTRL_C::ENVELOPE )
        {
        Funct[1]->Clear ();
        Funct[1]->Start ();
        }
    }

//#######################################################################
void FLT4_C::NoteClear ()
    {
    if ( ControlSrc[0] == FILTER_CTRL_C::ENVELOPE )
        Funct[0]->End ();
    if ( ControlSrc[1] == FILTER_CTRL_C::ENVELOPE )
        Funct[1]->End ();
    }

//#######################################################################
void FLT4_C::SetStart (byte fn, float level_percent)
    {
    Funct[fn]->SetLevel(ESTATE::START, level_percent);

    switch ( ControlSrc[fn] )
        {
        case FILTER_CTRL_C::FIXED:
            Funct[fn]->SetCurrent (level_percent);
            break;
        case FILTER_CTRL_C::ENVELOPE:
            break;
        case FILTER_CTRL_C::MODULATE:
            break;
        case FILTER_CTRL_C::MODWHEEL:
            break;
        default:
            break;
        }
    }

//#######################################################################


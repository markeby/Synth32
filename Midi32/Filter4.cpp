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
    FLT4_C::FLT4_C (short num, short first_device, byte& usecount, ENVELOPE_GENERATOR_C& envgen) : EnvGen (envgen)
    {
    this->Valid  = false;
    this->Number = num;
    // D/A configuration
    this->FreqIO   = first_device;
    this->QuIO     = first_device + 1;
    this->Funct[0] = EnvGen.NewADSR (num, envNames[0], FreqIO, usecount);
    this->Funct[1] = EnvGen.NewADSR (num, envNames[1], QuIO,   usecount);

    short dig = first_device + 4;       // get first digital switch
    if ( num & 1 )                      //   odd numbered units need 2 more
        dig += 2;

    this->OutSwitch[0] = dig++;
    this->OutSwitch[1] = dig++;
    this->OutSwitch[2] = dig++;
    this->OutSwitch[3] = dig;
    this->OutMap       = 0;

    if ( I2cDevices.IsPortValid (this->FreqIO) && I2cDevices.IsPortValid (this->QuIO) && I2cDevices.IsPortValid (this->OutSwitch[0]) )
        {
        if ( DebugSynth )
            printf("\t  >> VCF %d started for device %d\n", num, first_device);
        this->Valid = true;
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
    this->ClearState ();
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void FLT4_C::SetOutMap (byte fmap)
    {
    this->OutMap = fmap;

    for ( short z = 0;  z < 4;  z++ )
        I2cDevices.DigitalOut (this->OutSwitch[z], (fmap >> z) & 1);
    I2cDevices.UpdateDigital ();
    }



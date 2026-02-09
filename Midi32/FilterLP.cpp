//#######################################################################
// Module:     Filter4.cpp
// Descrption: Controls for 3 pole variable state filter with 4 outputs
// Creator:    markeby
// Date:       7/26/2025
//#######################################################################
#include <Arduino.h>
#include "Settings.h"
#include "FilterLP.h"
#include "I2Cmessages.h"

#ifdef DEBUG_SYNTH
static const char* label = "LPF";
#define DBG(args...) {if(DebugSynth){ DebugMsg(label,_Number,args);}}
#else
#define DBG(args...)
#endif

static const char*  switchNames[] = { "LP", "LBP", "UBP", "HP" };
//#######################################################################
    LPF_C::LPF_C ()
    {
    _Valid  = false;
    }

//#######################################################################
void LPF_C::Begin (short num, short first_device, byte& usecount)
    {
    _Number = num;

    if ( num & 1 )
        {
        _FreqIO      = first_device + 2;   // D/A configuration
        _QuIO        = first_device + 3;
        _QcompMuxIO1 = first_device + 4 + 4;   // base + 4 d/A ports + offset
        _QcompMuxIO2 = first_device + 4 + 6;
        }
    else
        {
        _FreqIO      = first_device;           // D/A configuration
        _QuIO        = first_device + 1;
        _QcompMuxIO1 = first_device + 4 + 2;   // base + 4 d/A ports + offset
        _QcompMuxIO2 = first_device + 4 + 7;
        }
    _StageIO = first_device + 4;

    _Envelope = EnvelopeGenerator.NewADSR (num, "LPF Freq", _FreqIO, DA_MAX, usecount);
    _Envelope->SetDualUse (true);

    if ( I2cDevices.IsPortValid (_FreqIO) && I2cDevices.IsPortValid (_QuIO) && I2cDevices.IsPortValid (_QcompMuxIO1) )
        {
        if ( DebugSynth )
            printf ("\t  >> LPF %d started for device %d\n", num, first_device);
        SetModeQ (0);
        SetPole (0);
        _Valid =  true;
        }
    else
        printf ("\t  ** LPF %d NO USABLE D/A CHANNELS FROM DEVICE %d\n", num, first_device);
    }

//#######################################################################
void LPF_C::ClearState ()
    {
    _Envelope->Clear ();
    }

//#######################################################################
void LPF_C::Clear ()
    {
    ClearState ();
    I2cDevices.Update ();     // Update D/A ports
    }

//#######################################################################
void LPF_C::SetQ (float level_percent)
    {
    _Q = level_percent;
    short z = (short)(level_percent * (float)DA_MAX);
    I2cDevices.D2Analog (_QuIO, z);
    }

//#######################################################################
void LPF_C::SetTuning (bool qsel, uint16_t level)
    {
    if ( qsel )
        {
        _TuningQ = level;
        I2cDevices.D2Analog (_QuIO, level);
        }
    else
        _Envelope->SetOverride (level);
    }

//#######################################################################
void LPF_C::NoteSet (byte key, byte velocity)
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
void LPF_C::NoteClear ()
    {
    if ( _ControlSrc >= ENV_CTRL_E::ENVELOPE )
        _Envelope->End ();
    }

//#######################################################################
void LPF_C::SetModeQ (byte value)
    {
    if ( value > 2 )
        return;

    _CurrentModeQ = value;
    I2cDevices.DigitalOut    (_QcompMuxIO1, value & 1);
    I2cDevices.DigitalOut    (_QcompMuxIO1 + 1, value & 2);
    I2cDevices.DigitalOut    (_QcompMuxIO2, value & 2);
    I2cDevices.Update ();
    }

//#######################################################################
void LPF_C::SetPole (byte value)
    {
    if ( value > 3 )
        return;

    _CurrentStage = value;
    I2cDevices.DigitalOut    (_StageIO, value & 1);
    I2cDevices.DigitalOut    (_StageIO + 1, value & 2);
    I2cDevices.Update ();
    }

//#######################################################################
void LPF_C::SetStart (float level_percent)
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


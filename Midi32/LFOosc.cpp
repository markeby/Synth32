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

#ifdef DEBUG_SYNTH
static const char* Label = "LFO";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,_Number,args);}}
#else
#define DBG(args...)
#endif

static  const char*     mixerNames[] = { "sine", "saw", "pulse" };

//#######################################################################
SYNTH_LFO_C::SYNTH_LFO_C ()
    {
    _Valid         = false;
    _UpdateNeded   = false;
    _CurrentLevel  = 0;
    _CurrentFreq   = 0.0;
    _ResetOn       = false;
    _RampSlope     = false;
    _InUse         = 0;
    _Midi          = 1;
    _Offset        = 0;
    }

//#######################################################################
void SYNTH_LFO_C::ClearState ()
    {
    for ( int z = 0;  z < SOURCE_CNT_LFO;  z++)
        {
        SYNTH_LFO_C::WAVE_LEVEL_T& m = _Level[z];
        I2cDevices.D2Analog (m.Port, 0);
        }
    }

//#######################################################################
void SYNTH_LFO_C::SetLevel (uint8_t ch, uint8_t data)
    {
    int z = (int)((data * 0.007874) * (float)MAX_DA);
    _Level[ch].MaximumLevel =  (z > MAX_DA ) ? MAX_DA : z;

    DBG ("Level limit > %d", _Level[ch].MaximumLevel);
    }

//#######################################################################
void SYNTH_LFO_C::SetInternalLevel (uint8_t ch, uint8_t data)
    {
    int z = (int)((data * 0.007874) * (float)MAX_DA);
    z = (z > _Level[ch].MaximumLevel ) ? _Level[ch].MaximumLevel : z;

    if ( z != _Level[ch].CurrentLevel )
        {
        _Level[ch].CurrentLevel = z;
        I2cDevices.D2Analog (_Level[ch].Port,  z);
        _UpdateNeded = true;
        DBG ("%s level > %d", _Level[ch].Name, z);
        }
    }

//#######################################################################
//#######################################################################
void SYNTH_LFO_C::Begin (int num, uint8_t first_device, uint8_t lfo_digital)
    {
    _Number = num;
    // D/A configurations
    _OscPortIO  = first_device + uint8_t(LFO_D_A_OFF::FREQ);
    _PwmPortIO  = first_device + uint8_t(LFO_D_A_OFF::WIDTH);
    _BendPortIO = first_device + uint8_t(LFO_D_A_OFF::BEND);
    _Level[int(LFO_SHAPE::RAMP)].Port  = first_device + uint8_t(LFO_D_A_OFF::RAMP);
    _Level[int(LFO_SHAPE::PULSE)].Port = first_device + uint8_t(LFO_D_A_OFF::PULSE);
    _Level[int(LFO_SHAPE::SINE)].Port  = first_device + uint8_t(LFO_D_A_OFF::SINE);
    _SlopePortO       = lfo_digital;
    _HardResetPortIO  = lfo_digital + 1;
    _Midi             = 0;

    // Initialize mixers

    for ( int z = 0;  z < SOURCE_CNT_LFO;  z++ )
        {
        _Level[z].Name         = mixerNames[z];
        _Level[z].CurrentLevel = 0;
        _Level[z].MaximumLevel = MAX_DA;
        }

    if ( I2cDevices.IsPortValid (first_device) && I2cDevices.IsPortValid (first_device + 5) )
        {
        _Valid = true;
        _Offset = Settings.GetOffsetLFO (num);
        ClearState ();
        PitchBend (PITCH_BEND_CENTER);
        if ( DebugSynth )
            printf ("\t  >> LFO %d started for device %d\n", _Number, first_device);
        }
    else
        printf ("\t  ** LFO %d NO USABLE D/A CHANNELS FROM DEVICE %d\n", num, first_device);
    }

//#######################################################################
void SYNTH_LFO_C::Clear ()
    {
    ClearState ();
    I2cDevices.Update ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_LFO_C::ProcessFreq ()
    {
    _CurrentFreq = (_CurrentFreqCoarse * MIDI_MULTIPLIER) + (_CurrentFreqFine * (MIDI_MULTIPLIER * 0.01));
    if ( _CurrentFreq > DA_MAX )
        _CurrentFreq = DA_MAX;
    OutputFreqIO ();
    }

//#######################################################################
void SYNTH_LFO_C::OutputFreqIO ()
    {
    DBG ("Set frequency %d", _CurrentFreq);
    I2cDevices.D2Analog (_OscPortIO, _CurrentFreq);
    I2cDevices.Update ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_LFO_C::SetPulseWidth (short value)
    {
    _CurrentWidth = value;
    DBG ("Set pulse width %d", value);
    I2cDevices.D2Analog (_PwmPortIO, DA_MAX - value);
    I2cDevices.Update ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_LFO_C::SetWave (short ch, bool state)
    {
    _Level[ch].Select = state;
    if ( state )
        {
        _InUse++;
        SetInternalLevel (ch, _CurrentLevel);
        }
    else
        {
        _InUse--;
        SetInternalLevel (ch, 0);
        }

    if ( _UpdateNeded )
        {
        I2cDevices.Update ();     // Update D/A ports
        _UpdateNeded = false;
        }

    DBG ("%s(%d) selected %s", _Level[ch].Name, ch, ((state) ? "ON" : "off"));
    }

//#######################################################################
void SYNTH_LFO_C::SetLevelMidi (byte mchan, uint8_t data)
    {
    if ( mchan == _Midi )
        {
        _CurrentLevel = data;
        for (int z = 0;  z < SOURCE_CNT_LFO;  z++ )
            {
            if ( _Level[z].Select )
                SetInternalLevel (z, data);
            else
                SetInternalLevel (z, 0);
            }
        if ( _UpdateNeded )
            {
            I2cDevices.Update ();     // Update D/A ports
            _UpdateNeded = false;
            }
        if ( _InUse )
            DisplayMessage.LfoHardLevel (_Number, data);
        }
    }

//#######################################################################
void SYNTH_LFO_C::ResetControl ()
    {
    for (int z = 0;  z < SOURCE_CNT_LFO;  z++ )     //zero out gain on all sources
        SetInternalLevel (z, 0);
    PitchBend (MAX_DA >> 1);                        //zero out pitch bend
    }

//#######################################################################
void SYNTH_LFO_C::PitchBend (short value)
    {
    I2cDevices.D2Analog (_BendPortIO, value + _Offset);
    I2cDevices.Update ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_LFO_C::SetRampDir (bool state)
    {
    _RampSlope = state;
    I2cDevices.DigitalOut (_SlopePortO, state);
    I2cDevices.Update  ();
    }

//#######################################################################
void SYNTH_LFO_C::HardReset (byte mchan)
    {
    if ( mchan == _Midi )
        {
        I2cDevices.DigitalOut (_HardResetPortIO, true);
        _ResetOn = true;
        }
    }

//#######################################################################
void SYNTH_LFO_C::Loop (void)
    {
    if (  _ResetOn )
        {
        I2cDevices.DigitalOut (_HardResetPortIO, false);
        _ResetOn = false;
        }
    }


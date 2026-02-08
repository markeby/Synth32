//#######################################################################
// Module:     FrontEndTuning.cpp
// Descrption: Synthesizer front end tuning control
// Creator:    markeby
// Date:       12/9/2024
//#######################################################################
#include <Arduino.h>

#include "Config.h"
#include "I2Cmessages.h"
#include "Osc.h"
#include "Settings.h"
#include "FrontEnd.h"
#include "Debug.h"

#ifdef DEBUG_SYNTH
static const char* Label  = "CAL";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#endif

//#######################################################################
static constexpr float fsr6_144 = (6.144 / 32767.0) * 1000;

static bool    change_settings = false;
static ushort  oscillator_level[OSC_MIXER_COUNT+1];
static ushort  level_filter;
static ushort  level_q;
static byte    output_select;
static ushort  calibration_reference;
static short   calibration_phase;
static short   calibration_lfo;

//#######################################################################
//#######################################################################
void Tuning ()
    {
    for ( int zc = 0;  zc < VOICE_COUNT;  zc++ )
        {
        if ( Down.Trigger )
            {
            VoiceArray[zc]->SetTuningNote (Down.Key);     // send key index to oscillator
            if ( VoiceArray[zc]->TuningState () )
                {
                DisplayMessage.TuningNote (Down.Key);
                DisplayMessage.TuningDtoA (VoiceArray[zc]->LastDA ());
                }
            }
        if ( change_settings )
            {
            if ( VoiceArray[zc]->TuningState () )
                {
                for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
                    {
                    if ( z < OSC_MIXER_COUNT )
                        {
                        VoiceArray[zc]->SetTuningVolume (z, oscillator_level[z]);
                        DisplayMessage.TuningLevel (z, oscillator_level[z] * MIDI_INV_MULTIPLIER);
                        }
                    }
                }
            else
                {
                for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
                    {
                    if ( z < OSC_MIXER_COUNT )
                        VoiceArray[zc]->SetTuningVolume(z, 0);
                    }
                }
            VoiceArray[zc]->SetTuningFlt (false, level_filter);
            VoiceArray[zc]->SetTuningFlt (true, level_q);
            if ( VoiceArray[zc]->TuningState () )
                VoiceArray[zc]->SetOutputMask (output_select);
            else
                VoiceArray[zc]->SetOutputMask (0);
            }
        }

    if ( change_settings )
        {
        DisplayMessage.TuningControl (output_select);
        DisplayMessage.TuningFilter (0, level_filter * MIDI_INV_MULTIPLIER);
        DisplayMessage.TuningFilter (1, level_q * MIDI_INV_MULTIPLIER);
        }

    change_settings = false;    // Indicate complete and ready for next not change
    Down.Trigger = 0;           // Clear note change trigger
    I2cDevices.Update ();       // Update all digital port changes
    }

//#######################################################################
void RecoverTuning ()
    {
    DisplayMessage.Unlock ();
    DisplayMessage.PageTuning ();
    change_settings = true;
    }

//#######################################################################
void StartTuning ()
    {
    uint16_t zb = 0;

    if ( SetTuning == false )
        {
        DisplayMessage.PageTuning ();
        for ( int z = 0;  z < OSC_MIXER_COUNT;  z++)
            {
            if ( z == 1 )
                zb = DA_MAX;
            else
                zb = 0;
            oscillator_level[z] = zb;
            DisplayMessage.TuningLevel (z, 0);
            }

        level_filter = 0;
        DisplayMessage.TuningFilter (0, 0);
        DisplayMessage.TuningFilter (1, 0);
        output_select = 0x01;
        VoiceArray[0]->TuningState (true);
        byte note = KEYS_FIRST;                    // start at the C0
        DisplayMessage.TuningNote (note);
        for ( int zc = 0;  zc < VOICE_COUNT;  zc++ )
            {
            change_settings = true;
            VoiceArray[zc]->SetTuningNote (note);
            VoiceArray[zc]->SetFlt4Ctrl (0);
            }
        }
    DisplayMessage.TuningDtoA (VoiceArray[0]->LastDA ());
    DisplayMessage.TuningSelect (0);
    SetTuning = true;
    }

//#######################################################################
void TuningOutputBitFlip (int bit)
    {
    output_select ^= 1 << bit;
    change_settings = true;
    }

//#######################################################################
void TuningAdjust (bool up)
    {
    for (int z = 0;  z < VOICE_COUNT;  z++)
        VoiceArray[z]->TuningAdjust (up);
    }

//#######################################################################
void SetTuningLevel (short ch, short data)
    {
    oscillator_level[ch] = data * MIDI_MULTIPLIER;
    change_settings = true;
    }

//#######################################################################
void SetTuningFilter (short ch, short data)
    {
    if ( ch == 1 )
        level_q = data * MIDI_MULTIPLIER;
    else
        level_filter = data * MIDI_MULTIPLIER;
    change_settings = true;
    }

//#######################################################################
void TuningBump (bool state)
    {
    byte note = (state) ? KEYS_LAST : KEYS_FIRST;         // C8 and C0
    for ( int zc = 0;  zc < VOICE_COUNT;  zc++ )
        {
        VoiceArray[zc]->SetTuningNote (note);
        if ( VoiceArray[zc]->TuningState () )
            {
            DisplayMessage.TuningNote (note);
            DisplayMessage.TuningDtoA (VoiceArray[zc]->LastDA ());
            }
        }
    }

//#######################################################################
void SaveTuning ()
    {
    if ( SetTuning )
        {
        DBG ("Saving synth keyboard arrays");
        SynthConfig.SaveTuning ();
        }
    }

//#######################################################################
static void  cb_Calibration (ushort val)
    {
    ushort zu = ((ushort)((float)val * fsr6_144) >> 1) << 1;    // caclulate voltage * 100 with LSB=0
    DBG ("Calibration phase %d with at %#1.3f volts  [%x]", calibration_phase, 0.001 * (float)zu, val);

    switch ( calibration_phase )
        {
        case 1:                                             // save calibration settings and start LFO calibration
            calibration_reference = zu;
            calibration_lfo   = 0;                     // start with first LFO
                                                            // Fall through with LFO port setup
        case 2:
            for (int z = 0;  z < 16;  z++ )                 // clear all ports
                I2cDevices.DigitalOut (CalibrationBaseDigital + z, false);
            if ( calibration_lfo == 0 )
                {
                for ( int z = 0;  z < 4;  z++)
                    I2cDevices.DigitalOut(CalibrationBaseDigital + z, true);          // select LFO-0
                }
            else
                {
                for ( int z = 4;  z < 8;  z++)
                    I2cDevices.DigitalOut (CalibrationBaseDigital + z, true);      // select LFO-1
                }
            I2cDevices.Update ();                    // Update all digital port changes
            delay (500);                                      // let voltage settle
            I2cDevices.StartAtoD  (CalibrationAtoD);
            calibration_phase = 3;
            break;

        case 3:
            {
            SYNTH_LFO_C& lfo = Lfo[calibration_lfo];
            short offset = 0;
            if ( zu < calibration_reference )
                offset = +1;
            else
                {
                if ( zu > calibration_reference )
                    offset = -1;
                else
                    {
                    DBG ("LFO %d offset = %X", calibration_lfo, lfo.GetOffset ());
                    Settings.SetOffsetLFO (calibration_lfo, lfo.GetOffset ());

                    if ( calibration_lfo == 0 )        // if calibrating the first LFO
                        {
                        calibration_lfo++;             // select the next LFO
                        calibration_phase = 2;         // restart LFO calibraion
                        }
                    else
                        {
                        calibration_phase    = 0;
                        I2cDevices.ResetAnalog     (CalibrationAtoD);

                        for (int z = 0;  z < 8;  z++ )                 // clear LFO ports
                            I2cDevices.DigitalOut (CalibrationBaseDigital + z, false);
                        for (int z = 8;  z < 12;  z++ )                 // enable fixed offset ports
                            I2cDevices.DigitalOut (CalibrationBaseDigital + z, true);
                        StartTuning ();
                        }
                    return;
                    }
                }
            lfo.SetOffset (offset + lfo.GetOffset());   // Update the offset value in the LFO
            lfo.PitchBend (PITCH_BEND_CENTER);          // Update output voltage with new offset
            delay (5);
            I2cDevices.StartAtoD (CalibrationAtoD);
            }
            break;

        default:        // This should never happen
            break;
        }

    }

//#######################################################################
void StartCalibration ()
    {
    if ( calibration_phase > 0 )
        return;

    DisplayMessage.PageCalibration ();
    LaunchControl.SelectTemplate (XL_MIDI_MAP_TUNING);
    DBG ("Starting Calibration");

    Lfo[0].SetOffset(0);
    Lfo[1].SetOffset (0);
    Lfo[0].PitchBend (Lfo[0].GetOffset() + 64);
    Lfo[1].PitchBend (Lfo[1].GetOffset() + 64);

    for (int z = 0;  z < 16;  z++ )                 // clear all ports
        I2cDevices.DigitalOut (CalibrationBaseDigital + z, false);
    for ( int z = 8;  z < 12;  z++)
        I2cDevices.DigitalOut (CalibrationBaseDigital + z,  true);
    I2cDevices.Update ();                    // Update all digital port changes
    delay (500);                                      // let voltage settle
    I2cDevices.SetCallbackAtoD (cb_Calibration);
    I2cDevices.StartAtoD (CalibrationAtoD);   // Start analog sampling
    calibration_phase = 1;
    }


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

static constexpr float fsr6_144 = (6.144 / 32767.0) * 1000;

static void CalibrationCallback (ushort val)
    {
    Calibration (val);
    }

//#######################################################################
void Tuning ()
    {
    for ( int zc = 0;  zc < VOICE_COUNT;  zc++ )
        {
        if ( Down.Trigger )
            {
            pVoice[zc]->SetTuningNote (Down.Key);     // send key index to oscillator
            if ( pVoice[zc]->TuningState () )
                {
                DisplayMessage.TuningNote (Down.Key);
                DisplayMessage.TuningDtoA (pVoice[zc]->LastDA ());
                }
            }
        if ( TuningChange )
            {
            if ( pVoice[zc]->TuningState () )
                {
                for ( int z = 0;  z < ENVELOPE_COUNT;  z++ )
                    {
                    if ( z < OSC_MIXER_COUNT )
                        {
                        pVoice[zc]->SetTuningVolume (z, TuningLevel[z]);
                        DisplayMessage.TuningLevel (z, TuningLevel[z] * MIDI_INV_MULTIPLIER);
                        }
                    }
                }
            else
                {
                for ( int z = 0;  z < ENVELOPE_COUNT;  z++ )
                    {
                    if ( z < OSC_MIXER_COUNT )
                        pVoice[zc]->SetTuningVolume(z, 0);
                    }
                }
            for ( int z = 0;  z < FILTER_DEVICES;  z++ )
                {
                pVoice[zc]->SetTuningFlt (z, TuningFlt[z]);
                if ( pVoice[zc]->TuningState () )
                    pVoice[zc]->SetOutputMask (TuningOutputSelect);
                else
                    pVoice[zc]->SetOutputMask (0);
                }
            }
        }

    if ( TuningChange )
        {
        DisplayMessage.TuningControl (TuningOutputSelect);
        for ( int z = 0;  z < FILTER_DEVICES;  z++ )
            DisplayMessage.TuningFilter (z, TuningFlt[z] * MIDI_INV_MULTIPLIER);
        }

    TuningChange = false;     // Indicate complete and ready for next not change
    Down.Trigger = 0;         // Clear note change trigger
    I2cDevices.UpdateDigital ();    // Update all digital port changes
    I2cDevices.UpdateAnalog  ();    // Update D/A port changes
    }

//#######################################################################
void StartTuning ()
    {
    uint16_t zb = 0;

    if ( SetTuning == false )
        {
        DisplayMessage.PageTuning ();
        for ( int z = 0;  z < ENVELOPE_COUNT;  z++)
            {
            if ( z == 1 )
                zb = DA_MAX;
            else
                zb = 0;
            TuningLevel[z] = zb;
            DisplayMessage.TuningLevel (z, 0);
            }

        for ( int z = 0;  z < FILTER_DEVICES;  z++ )
            {
            TuningFlt[z] = 0;
            DisplayMessage.TuningFilter (z, 0);
            }
        TuningOutputSelect = 0x01;
        pVoice[0]->TuningState (true);
        byte note = KEYS_FIRST;                    // start at the C0
        DisplayMessage.TuningNote (note);
        for ( int zc = 0;  zc < VOICE_COUNT;  zc++ )
            {
            TuningChange = true;
            pVoice[zc]->SetTuningNote (note);
            pVoice[zc]->SetFltCtrl (0);
            }
        }
    DisplayMessage.TuningDtoA (pVoice[0]->LastDA ());
    DisplayMessage.TuningSelect (0);
    SetTuning = true;
    }

//#######################################################################
void TuningAdjust (bool up)
    {
    for (int z = 0;  z < VOICE_COUNT;  z++)
        pVoice[z]->TuningAdjust (up);
    }

//#######################################################################
void SetTuningLevel (short ch, short data)
    {
    TuningLevel[ch] = data * MIDI_MULTIPLIER;
    TuningChange = true;
    }

//#######################################################################
void SetTuningFilter (short ch, short data)
    {
    TuningFlt[ch] = data * MIDI_MULTIPLIER;
    TuningChange = true;
    }

//#######################################################################
void TuningBump (bool state)
    {
    byte note = (state) ? KEYS_LAST : KEYS_FIRST;         // C8 and C0
    for ( int zc = 0;  zc < VOICE_COUNT;  zc++ )
        {
        pVoice[zc]->SetTuningNote (note);
        if ( pVoice[zc]->TuningState () )
            {
            DisplayMessage.TuningNote (note);
            DisplayMessage.TuningDtoA (pVoice[zc]->LastDA ());
            }
        }
    }

//#######################################################################
void SaveTuning ()
    {
    if ( SetTuning )
        {
        DBG ("Saving synth keyboard arrays");
        for ( int z = 0;  z < VOICE_COUNT;  z++ )
            Settings.PutOscBank (z, pVoice[z]->GetBankAddr ());
        }
    }

//#######################################################################
void StartCalibration ()
    {
    if ( CalibrationPhase > 0 )
        return;

    DisplayMessage.PageCalibration ();
    TemplateSelect (XL_MIDI_MAP_TUNING);
    DBG ("Starting Calibration");

    Lfo[0].SetOffset(0);
    Lfo[1].SetOffset (0);
    Lfo[0].PitchBend (Lfo[0].GetOffset() + 64);
    Lfo[1].PitchBend (Lfo[1].GetOffset() + 64);

    for (int z = 0;  z < 16;  z++ )                 // clear all ports
        I2cDevices.DigitalOut (CalibrationBaseDigital + z, false);
    for ( int z = 8;  z < 12;  z++)
        I2cDevices.DigitalOut (CalibrationBaseDigital + z,  true);
    I2cDevices.UpdateDigital ();                    // Update all digital port changes
    delay (500);                                      // let voltage settle
    I2cDevices.SetCallbackAtoD (CalibrationCallback);
    I2cDevices.StartAtoD (CalibrationAtoD);   // Start analog sampling
    CalibrationPhase = 1;
    }

//#######################################################################
void Calibration (ushort val)
    {
    ushort zu = ((ushort)((float)val * fsr6_144) >> 1) << 1;    // caclulate voltage * 100 with LSB=0
    DBG ("Calibration phase %d with at %#1.3f volts  [%x]", CalibrationPhase, 0.001 * (float)zu, val);

    switch ( CalibrationPhase )
        {
        case 1:                                             // save calibration settings and start LFO calibration
            CalibrationReference = zu;
            CalibrationLFO   = 0;                     // start with first LFO
                                                            // Fall through with LFO port setup
        case 2:
            for (int z = 0;  z < 16;  z++ )                 // clear all ports
                I2cDevices.DigitalOut (CalibrationBaseDigital + z, false);
            if ( CalibrationLFO == 0 )
                {
                for ( int z = 0;  z < 4;  z++)
                    I2cDevices.DigitalOut(CalibrationBaseDigital + z, true);          // select LFO-0
                }
            else
                {
                for ( int z = 4;  z < 8;  z++)
                    I2cDevices.DigitalOut (CalibrationBaseDigital + z, true);      // select LFO-1
                }
            I2cDevices.UpdateDigital ();                    // Update all digital port changes
            delay (500);                                      // let voltage settle
            I2cDevices.StartAtoD  (CalibrationAtoD);
            CalibrationPhase = 3;
            break;

        case 3:
            {
            SYNTH_LFO_C& lfo = Lfo[CalibrationLFO];
            short offset = 0;
            if ( zu < CalibrationReference )
                offset = +1;
            else
                {
                if ( zu > CalibrationReference )
                    offset = -1;
                else
                    {
                    DBG ("LFO %d offset = %X", CalibrationLFO, lfo.GetOffset ());
                    Settings.SetOffsetLFO (CalibrationLFO, lfo.GetOffset ());

                    if ( CalibrationLFO == 0 )        // if calibrating the first LFO
                        {
                        CalibrationLFO++;             // select the next LFO
                        CalibrationPhase = 2;         // restart LFO calibraion
                        }
                    else
                        {
                        CalibrationPhase    = 0;
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


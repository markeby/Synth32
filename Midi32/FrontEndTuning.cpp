//#######################################################################
// Module:     FrontEndTuning.cpp
// Descrption: Synthesizer front end tuning control
// Creator:    markeby
// Date:       12/9/2024
//#######################################################################
#include "../Common/SynthCommon.h"
#include "I2Cmessages.h"
#include "Osc.h"
#include "Settings.h"
#include "FrontEnd.h"
#include "Debug.h"

#ifdef DEBUG_SYNTH
static const char* Label  = "CAL";
//#define DBG(args...) {if(DebugSynth){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#define DBG(args...) {if(true){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#endif

static constexpr float fsr6_144 = (6.144 / 32767.0) * 1000;

static void CalibrationCallback (ushort val)
    {
    SynthFront.Calibration (val);
    }

//#######################################################################
void SYNTH_FRONT_C::Tuning ()
    {
    for ( int zc = 0;  zc < VOICE_COUNT;  zc++ )
        {
        if ( this->Down.Trigger )
            {
            this->pVoice[zc]->SetTuningNote (this->Down.Key);     // send key index to oscillator
            DisplayMessage.TuningNote (this->Down.Key);
            }
        if ( this->TuningChange )
            {
            if ( this->TuningOn[zc] )
                {
                for ( int z = 0;  z < ENVELOPE_COUNT;  z++ )
                    {
                    if ( z < OSC_MIXER_COUNT )
                        {
                        this->pVoice[zc]->SetTuningVolume (z, TuningLevel[z]);
                        DisplayMessage.TuningLevel (z, TuningLevel[z] * MIDI_INV_MULTIPLIER);
                        }
                    }
                }
            else
                {
                for ( int z = 0;  z < ENVELOPE_COUNT;  z++ )
                    {
                    if ( z < OSC_MIXER_COUNT )
                        this->pVoice[zc]->SetTuningVolume(z, 0);
                    }
                }
            }
        }
    this->TuningChange = false;     // Indicate complete and ready for next not change
    this->Down.Trigger = 0;         // Clear note change trigger
    I2cDevices.UpdateDigital ();    // Update all digital port changes
    I2cDevices.UpdateAnalog  ();    // Update D/A port changes
    }

//#######################################################################
void SYNTH_FRONT_C::StartTuning ()
    {
    if ( this->SetTuning == false )
        {
        DisplayMessage.PageTuning ();
        for ( int z = 0;  z < ENVELOPE_COUNT;  z++)
            {
            this->TuningLevel[z] = (uint16_t)(this->pVoice[0]->GetMaxLevel (z) * MAX_DA);
            DisplayMessage.TuningLevel (z, this->TuningLevel[z] * MIDI_INV_MULTIPLIER);
            }
        for ( int zc = 0;  zc < VOICE_COUNT;  zc++ )
            {
            this->TuningOn[zc] = false;
            this->TuningChange = true;
            }
        }
    this->SetTuning    = true;
    }

//#######################################################################
void SYNTH_FRONT_C::TuningAdjust (bool up)
    {
    if ( this->SetTuning )
        {
        for (int z = 0;  z < VOICE_COUNT;  z++)
            {
            if ( this->TuningOn[z] )
                this->pVoice[z]->TuningAdjust (up);
            }
        }
    }

//#######################################################################
void SYNTH_FRONT_C::TuningBump (bool state)
    {
    if ( this->SetTuning )
        {
        byte note = (state) ? 125 : 5;         // Highest F or lowest F
        DisplayMessage.TuningNote (note);
        for ( int zc = 0;  zc < VOICE_COUNT;  zc++ )
            this->pVoice[zc]->SetTuningNote (note);
        }
    }

//#######################################################################
void SYNTH_FRONT_C::SaveTuning ()
    {
    if ( SetTuning )
        {
        DBG ("Saving synth keyboard arrays");
        for ( int z = 0;  z < VOICE_COUNT;  z++ )
            Settings.PutOscBank (z, this->pVoice[z]->GetBankAddr ());
        }
    }

//#######################################################################
void SYNTH_FRONT_C::StartCalibration ()
    {
    if ( this->CalibrationPhase > 0 )
        return;

    DisplayMessage.PageCalibration ();

    this->Lfo[0].SetOffset(0);
    this->Lfo[1].SetOffset (0);
    this->Lfo[0].PitchBend (PITCH_BEND_CENTER);
    this->Lfo[1].PitchBend (PITCH_BEND_CENTER);

    for (int z = 0;  z < 16;  z++ )                 // clear all ports
        I2cDevices.DigitalOut (this->CalibrationBaseDigital + z, false);
    I2cDevices.DigitalOut (this->CalibrationBaseDigital + 8, true);
    I2cDevices.UpdateDigital ();                    // Update all digital port changes
    delay (500);                                      // let voltage settle
    I2cDevices.SetCallbackAtoD (CalibrationCallback);
    I2cDevices.StartAtoD (this->CalibrationAtoD);   // Start analog sampling
    this->CalibrationPhase = 1;
    }

//#######################################################################
void SYNTH_FRONT_C::Calibration (ushort val)
    {
    ushort zu = ((ushort)((float)val * fsr6_144) >> 1) << 1;    // caclulate voltage * 100 with LSB=0
    DBG ("Calibration phase %d with at %#1.3f volts  [%x]", this->CalibrationPhase, 0.001 * (float)zu, val);

    switch ( this->CalibrationPhase )
        {
        case 1:                                             // save calibration settings and start LFO calibration
            this->CalibrationReference = zu;
            this->CalibrationLFO   = 0;                     // start with first LFO
                                                            // Fall through with LFO port setup
        case 2:
            for (int z = 0;  z < 16;  z++ )                 // clear all ports
                I2cDevices.DigitalOut (this->CalibrationBaseDigital + z, false);
            if ( this->CalibrationLFO == 0 )
                I2cDevices.DigitalOut (this->CalibrationBaseDigital, true);          // select LFO-0
            else
                I2cDevices.DigitalOut (this->CalibrationBaseDigital + 4, true);      // select LFO-1
            I2cDevices.UpdateDigital ();                    // Update all digital port changes
            delay (500);                                      // let voltage settle
            I2cDevices.StartAtoD  (this->CalibrationAtoD);
            this->CalibrationPhase = 3;
            break;

        case 3:
            {
            SYNTH_LFO_C& lfo = this->Lfo[this->CalibrationLFO];
            short offset = 0;
            if ( zu < this->CalibrationReference )
                offset = +1;
            else
                {
                if ( zu > this->CalibrationReference )
                    offset = -1;
                else
                    {
                    DBG ("LFO %d offset = %X", this->CalibrationLFO, lfo.GetOffset ());
                    Settings.SetOffsetLFO (this->CalibrationLFO, lfo.GetOffset ());

                    if ( this->CalibrationLFO == 0 )        // if calibrating the first LFO
                        {
                        this->CalibrationLFO++;             // select the next LFO
                        this->CalibrationPhase = 2;         // restart LFO calibraion
                        }
                    else
                        {
                        this->CalibrationPhase    = 0;
                        I2cDevices.ResetAnalog     (this->CalibrationAtoD);

                        for (int z = 0;  z < 8;  z++ )                 // clear LFO ports
                            I2cDevices.DigitalOut (this->CalibrationBaseDigital + z, false);
                        for (int z = 8;  z < 12;  z++ )                 // enable fixed offset ports
                            I2cDevices.DigitalOut (this->CalibrationBaseDigital + z, true);
                        this->StartTuning ();
                        }
                    return;
                    }
                }
            lfo.SetOffset (offset + lfo.GetOffset());   // Update the offset value in the LFO
            lfo.PitchBend (PITCH_BEND_CENTER);          // Update output voltage with new offset
            delay (5);
            I2cDevices.StartAtoD (this->CalibrationAtoD);
            }
            break;

        default:        // This should never happen
            break;
        }

    }


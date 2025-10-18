//#######################################################################
// Module:     FrontEnd.cpp
// Descrption: Synthesizer front end controller
// Creator:    markeby
// Date:       12/9/2024
//#######################################################################
#include <Arduino.h>

#include "Config.h"
#include "FrontEnd.h"
#include "I2Cmessages.h"
#include "Osc.h"
#include "Settings.h"
#include "MidiConf.h"

#ifdef DEBUG_SYNTH
static const char* Label  = "TOP";
static const char* LabelM = "M";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#endif

//###################################################################
using namespace MIDI_NAMESPACE;
using namespace DISP_MESSAGE_N;

//#######################################################################
//#######################################################################
SYNTH_FRONT_C::SYNTH_FRONT_C (XL_MIDI_MAP (*xl_map)[XL_MIDI_MAP_SIZE])
    {
    pMidiMapXL          = xl_map;
    Down.Key            = 0;
    Down.Trigger        = 0;
    Down.Velocity       = 0;
    Up.Key              = 0;
    Up.Trigger          = 0;
    Up.Velocity         = 0;
    SetTuning           = false;
    TuningChange        = false;
    CurrentMapSelected  = -1;
    CurrentMidiSelected = 0;
    CalibrationPhase    = 0;
    LoadSaveSelection   = 1;
    TuningSelectionTime = 0;
    }

//#######################################################################
void SYNTH_FRONT_C::Begin (short voice, short mixer, short noise_digital, short lfo_control, short mod_mux_digital, short start_a_d)
    {
    // Setup ports for calibration
    CalibrationBaseDigital = mod_mux_digital;

    printf ("\t>>>\tSynth channels\n");
    short osc = voice;
    VolumeMaster    = mixer + 8;        // Master volume port
    VolumeOscMaster = mixer + 9;        // Oscillator master volume port
    VolumeFltMaster = mixer + 10;       // Filter master volume port
    VolumeSprMaster = mixer + 11;       // Spare master volume port

    MasterVolume (45);
    I2cDevices.D2Analog      (VolumeOscMaster, 0);
    I2cDevices.D2Analog      (VolumeFltMaster, 0);
    I2cDevices.D2Analog      (VolumeSprMaster, DA_MAX);     // mute for now
    I2cDevices.UpdateAnalog  ();

    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        pVoice[z] = new VOICE_C(z, osc, mixer, mod_mux_digital, noise_digital, EnvADSL);
        osc      += 8;
        mixer    += 1;
        if ( z & 1 )
            {
            mod_mux_digital += 1;
            noise_digital   += 4;
            voice           += 28;
            osc              = voice;
            }
        }

    Lfo[0].Begin (0, lfo_control + 8 , lfo_control);
    Lfo[1].Begin (1, lfo_control + 8 + 6, lfo_control + 3);
    CalibrationAtoD = start_a_d;
    ResolutionMode = true;

    LaunchControl.Begin (pMidiMapXL);

    InitMidiControl ();
    InitMidiKeyboard ();
    InitMidiSequence ();
    }

//#######################################################################
void SYNTH_FRONT_C::MasterVolume (short md)
    {
    int x = 0;

    if ( md > 0 )                               // zero is not valid flor log conversion.
        x = (float(log(md))* 845.3431);         // convert linear MIDI value to logorithmic D to A value. (0 -> 127) maps to (0 ->4095)
    x = DA_MAX - x;                             // Volume is a voltage controlled attenuator with 0 = 0 db, 4095 = -100 db change in output.

    DBG ("D to A value for volume = %d", x);
    I2cDevices.D2Analog      (VolumeMaster, x);
    I2cDevices.UpdateAnalog  ();
    }

//#######################################################################
void SYNTH_FRONT_C::Clear ()
    {
    for ( int z = 0;  z < VOICE_COUNT; z++ )
        pVoice[z]->Clear ();
    }

//#######################################################################
void SYNTH_FRONT_C::PageAdvance ()
    {
    byte  m    = CurrentMidiSelected;
    short next = CurrentMapSelected + 1;

    if ( LoadSaveMode || ResolutionMode || MapSelectMode )
        {
        next = 0;
        m = 0;
        LoadSaveMode   = false;
        ResolutionMode = false;
        MapSelectMode  = false;
        }

    while ( next < MAP_COUNT )
        {
        CurrentFilterSelected = -1;
        if ( m == SynthConfig.Voice[next].GetVoiceMidi () )
            {
            next++;
            continue;
            }
        CurrentMapSelected    = next;
        CurrentConfigSelected = next;
        CurrentVoiceSelected  = next << 1;
        CurrentMidiSelected   = SynthConfig.Voice[next].GetVoiceMidi ();
        UpdateOscDisplay ();
        return;
        }

    short index = next - MAP_COUNT;
    if ( next == MAP_COUNT )
        {
        CurrentVoiceSelected  = -1;
        CurrentMapSelected    = next;
        CurrentConfigSelected = index;
        CurrentFilterSelected = index << 1;
        CurrentMidiSelected   = SynthConfig.Voice[index].GetVoiceMidi ();
        UpdateFltDisplay ();
        return;
        }

    while ( next < (MAP_COUNT * 2) )
        {
        CurrentVoiceSelected = -1;
        if ( m == SynthConfig.Voice[index].GetVoiceMidi () )
            {
            next++;
            continue;
            }
        CurrentMapSelected    = next;
        CurrentConfigSelected = index;
        CurrentFilterSelected = index << 1;
        CurrentMidiSelected   = SynthConfig.Voice[index].GetVoiceMidi ();
        UpdateFltDisplay ();
        return;
        }

    index = next - (MAP_COUNT * 2) + 2;
    if ( DisplayMessage.Page () == (byte)PAGE_C::PAGE_MOD )
        {
        CurrentMapSelected    = 0;
        CurrentVoiceSelected  = 0;
        CurrentConfigSelected = 0;
        CurrentFilterSelected = -1;
        CurrentMidiSelected   = SynthConfig.Voice[0].GetVoiceMidi ();
        UpdateOscDisplay ();
        }
    else
        {
        CurrentMapSelected    = next;
        CurrentMidiSelected   = 0;
        CurrentFilterSelected = -1;       // de-select functions to disable
        CurrentVoiceSelected  = -1;
        CurrentConfigSelected = -1;
        UpdateLfoDisplay ();
        }
    }

//#######################################################################
void SYNTH_FRONT_C::Loop ()
    {
    int oldest = -1;
    int doit   = -1;

    Lfo[0].Loop ();
    Lfo[1].Loop ();

    Usb.Task    ();
    Midi_0.read ();
    Midi_1.read ();
    Midi_2.read ();

    if ( !SetTuning )
        {
        if ( Up.Trigger )
            {
            DBG ("Key up > %d", Up.Key);
            for ( int z = 0;  z < VOICE_COUNT;  z++ )
                pVoice[z]->NoteClear (Up.Trigger, Up.Key);
            Up.Trigger = 0;
            }

        if ( Down.Trigger )                          // Key went down so look for a channel to use
            {
            for ( int z = 0;  z <  VOICE_COUNT;  z++ )
                {
                VOICE_C& ch = *(pVoice[z]);
                if ( Down.Trigger == ch.GetMidi () )
                    {
                    if ( !ch.IsActive () )              // grab the first channel not in use
                        {
                        doit = z;
                        break;
                        }
                    else
                        {
                        if ( oldest < 0 )               // channel is inu use so this is the first one to check for oldest
                            oldest = z;
                        else
                            {
                            if ( ch.IsActive () > pVoice[oldest]->IsActive () )      // check if current channel older than the oldest so far
                                oldest = z;
                            }
                        }
                    }
                }
            if ( doit < 0 )                                 // no unused channel so we will capture the one used the longest
                doit = oldest;
            if ( doit >= 0 )                                // only process valid channels
                {
                Lfo[0].HardReset (Down.Trigger);
                Lfo[1].HardReset (Down.Trigger);
                pVoice[doit]->NoteSet (Down.Trigger, Down.Key, Down.Velocity);   // set the channel
                }
            DBG ("Key down > %d   Velocity > %d  Port > %d", Down.Key, Down.Velocity, doit);
            Down.Trigger = 0;                         // release the trigger
            }

        EnvADSL.Loop ();                                    // process all envelope generators
        for ( int z = 0;  z < VOICE_COUNT;  z++ )           // Check all channels for done
            pVoice[z]->Loop ();
        I2cDevices.UpdateDigital ();
        I2cDevices.UpdateAnalog  ();     // Update D/A ports
        }
    else
        Tuning ();

    LaunchControl.Loop ();
    }

//#####################################################################
void SYNTH_FRONT_C::TemplateSelect (byte index)
    {
    byte *pb = nullptr;

    switch ( index )
        {
        case XL_MIDI_MAP_OSC:
            pb = SynthConfig.Voice[CurrentVoiceSelected >> 1].GetButtonStateOscPtr ();
            break;
        case XL_MIDI_MAP_FLT:
            pb = SynthConfig.Voice[CurrentFilterSelected >> 1].GetButtonStateFltPtr ();
            break;
        case XL_MIDI_MAP_LFO:
            pb = SynthConfig.GetButtonStateLfoPtr ();
            break;
        default:
            break;
        }

    LaunchControl.SelectTemplate (index, pb);
    }


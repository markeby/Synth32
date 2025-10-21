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
static short    volumeMaster;
static short    volumeOscMaster;
static short    volumeFltMaster;
static short    volumeSprMaster;

//###################################################################
using namespace MIDI_NAMESPACE;
using namespace DISP_MESSAGE_N;

//#######################################################################
//#######################################################################
void InitializeSynth (short voice, short mixer, short noise_digital, short lfo_control, short mod_mux_digital, short start_a_d)
    {
    // Setup ports for calibration
    CalibrationBaseDigital = mod_mux_digital;

    printf ("\t>>>\tSynth channels\n");
    short osc = voice;
    volumeMaster    = mixer + 8;        // Master volume port
    volumeOscMaster = mixer + 9;        // Oscillator master volume port
    volumeFltMaster = mixer + 10;       // Filter master volume port
    volumeSprMaster = mixer + 11;       // Spare master volume port

    MasterVolume (45);
    I2cDevices.D2Analog      (volumeOscMaster, 0);
    I2cDevices.D2Analog      (volumeFltMaster, 0);
    I2cDevices.D2Analog      (volumeSprMaster, DA_MAX);     // mute for now
    I2cDevices.UpdateAnalog  ();

    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        VoiceArray[z] = new VOICE_C(z, osc, mixer, mod_mux_digital, noise_digital, EnvADSL);
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

    InitMidiControl ();
    InitMidiKeyboard ();
    InitMidiSequence ();
    }

//#######################################################################
void MasterVolume (short md)
    {
    int x = 0;

    if ( md > 0 )                               // zero is not valid flor log conversion.
        x = (float(log(md))* 845.3431);         // convert linear MIDI value to logorithmic D to A value. (0 -> 127) maps to (0 ->4095)
    x = DA_MAX - x;                             // Volume is a voltage controlled attenuator with 0 = 0 db, 4095 = -100 db change in output.

    DBG ("D to A value for volume = %d", x);
    I2cDevices.D2Analog      (volumeMaster, x);
    I2cDevices.UpdateAnalog  ();
    }

//#######################################################################
void ClearSynth ()
    {
    for ( int z = 0;  z < VOICE_COUNT; z++ )
        VoiceArray[z]->Clear ();
    }

//#######################################################################
void PageAdvance ()
    {
    byte  m    = SelectedMidi;
    short next = SelectedMap + 1;

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
        SelectedFilter = -1;
        if ( m == SynthConfig.Voice[next].GetVoiceMidi () )
            {
            next++;
            continue;
            }
        SelectedMap    = next;
        SelectedConfig = next;
        SelectedVoice  = next << 1;
        SelectedMidi   = SynthConfig.Voice[next].GetVoiceMidi ();
        StartOscDisplay ();
        return;
        }

    short index = next - MAP_COUNT;
    if ( next == MAP_COUNT )
        {
        SelectedVoice  = -1;
        SelectedMap    = next;
        SelectedConfig = index;
        SelectedFilter = index << 1;
        SelectedMidi   = SynthConfig.Voice[index].GetVoiceMidi ();
        StartFltDisplay ();
        return;
        }

    while ( next < (MAP_COUNT * 2) )
        {
        SelectedVoice = -1;
        if ( m == SynthConfig.Voice[index].GetVoiceMidi () )
            {
            next++;
            continue;
            }
        SelectedMap    = next;
        SelectedConfig = index;
        SelectedFilter = index << 1;
        SelectedMidi   = SynthConfig.Voice[index].GetVoiceMidi ();
        StartFltDisplay ();
        return;
        }

    index = next - (MAP_COUNT * 2) + 2;
    if ( DisplayMessage.Page () == (byte)PAGE_C::PAGE_MOD )
        {
        SelectedMap    = 0;
        SelectedVoice  = 0;
        SelectedConfig = 0;
        SelectedFilter = -1;
        SelectedMidi   = SynthConfig.Voice[0].GetVoiceMidi ();
        StartOscDisplay ();
        }
    else
        {
        SelectedMap    = next;
        SelectedMidi   = 0;
        SelectedFilter = -1;       // de-select functions to disable
        SelectedVoice  = -1;
        SelectedConfig = -1;
        UpdateLfoDisplay ();
        }
    }

//#######################################################################
void LoopSynth ()
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
                VoiceArray[z]->NoteClear (Up.Trigger, Up.Key);
            Up.Trigger = 0;
            }

        if ( Down.Trigger )                          // Key went down so look for a channel to use
            {
            for ( int z = 0;  z <  VOICE_COUNT;  z++ )
                {
                VOICE_C& ch = *(VoiceArray[z]);
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
                            if ( ch.IsActive () > VoiceArray[oldest]->IsActive () )      // check if current channel older than the oldest so far
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
                VoiceArray[doit]->NoteSet (Down.Trigger, Down.Key, Down.Velocity);   // set the channel
                }
            DBG ("Key down > %d   Velocity > %d  Port > %d", Down.Key, Down.Velocity, doit);
            Down.Trigger = 0;                         // release the trigger
            }

        EnvADSL.Loop ();                                    // process all envelope generators
        for ( int z = 0;  z < VOICE_COUNT;  z++ )           // Check all channels for done
            VoiceArray[z]->Loop ();
        I2cDevices.UpdateDigital ();
        I2cDevices.UpdateAnalog  ();     // Update D/A ports
        }
    else
        Tuning ();

    LaunchControl.Loop ();
    }

//#####################################################################
void SystemExDebug (byte* array, unsigned size)
    {
    for ( short z = 0;  z < size;  z += 16 )
        {
        String st = "\n";
        for ( short zz = 0;  (zz < 16) && ((z + zz) < size);  zz++)
            st += String(array[z+zz], HEX) + " ";
        printf ("%s", st.c_str ());
        }
    printf("\n");
    }

//#####################################################################
//#####################################################################
KEY_T           Down;
KEY_T           Up;

ENV_GENERATOR_C EnvADSL;
SYNTH_LFO_C     Lfo[2];
VOICE_C*        VoiceArray[VOICE_COUNT];

bool            ResolutionMode          = false;
bool            MapSelectMode           = false;
bool            LoadSaveMode            = false;
SYNTH_CONFIG_C  SynthConfig;
byte            SelectedMidi     = 0;
short           SelectedMap      = -1;
short           SelectedVoice    = -1;
short           SelectedFilter   = -1;
short           SelectedConfig   = -1;
short           CurrentDisplayPage      = 0;

NOVATION_XL_C   LaunchControl;

bool            SetTuning               = false;
short           CalibrationBaseDigital;
ushort          CalibrationAtoD;


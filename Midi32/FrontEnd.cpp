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
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#endif

//###################################################################
static short    volumeMaster;
static short    volumeOscMaster;
static short    volumeFltMaster;
static short    volumeFltLPMaster;

//###################################################################
using namespace MIDI_NAMESPACE;
using namespace DISP_MESSAGE_N;

//#######################################################################
//#######################################################################
void InitializeSynth ()
    {
    // I2C device index initialization
    short voice           = START_VOICE_CONTROL;
    short osc             = START_VOICE_CONTROL;
    short mixer           = START_MIXER;
    short noise_digital   = START_NOISE_DIGITAL;
    short lfo_control     = START_LFO_CONTROL;
    short mod_mux_digital = START_MOD_MUX;
    short start_a_d       = START_A_D;

    // Setup ports for calibration
    CalibrationBaseDigital = mod_mux_digital;

    // setup I2C indexes for mixers
    volumeMaster      = mixer + 16;       // Master volume port
    volumeFltLPMaster = mixer + 17;       // LP filter master volume port
    volumeFltMaster   = mixer + 18;       // Filter master volume port
    volumeOscMaster   = mixer + 19;       // Oscillator master volume port

    MasterVolume (45);
    I2cDevices.D2Analog      (volumeOscMaster, 0);
    I2cDevices.D2Analog      (volumeFltMaster, 0);
    I2cDevices.D2Analog      (volumeFltLPMaster, 0);
    I2cDevices.Update  ();

    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        VoiceArray[z] = new VOICE_C(z, osc, mixer, mod_mux_digital, noise_digital);
        osc      += 8;
        mixer    += 1;
        if ( z & 1 )
            {
            mod_mux_digital += 1;
            noise_digital   += 4;
            voice           += 40;
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
    I2cDevices.Update  ();
    }

//#######################################################################
void PageAdvance ()
    {
    byte  m    = SelectedMidi;
    short next = SelectedMap + 1;
    short index;

    if ( LoadMode || SaveMode|| ResolutionMode || MapSelectMode )
        {
        next = 0;
        m = 0;
        LoadMode       = false;
        SaveMode       = false;
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

    index = next - MAP_COUNT;
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
        index = next - MAP_COUNT;
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
                DBG ("Key down > %d   Velocity > %d  Port > %d", Down.Key, Down.Velocity, doit);
                Lfo[0].HardReset (Down.Trigger);
                Lfo[1].HardReset (Down.Trigger);
                VoiceArray[doit]->NoteSet (Down.Trigger, Down.Key, Down.Velocity);   // set the channel
                }
            Down.Trigger = 0;                         // release the trigger
            }

        EnvelopeGenerator.Loop (DeltaTimeMilli);            // process all envelope generators
        for ( int z = 0;  z < VOICE_COUNT;  z++ )           // Check all channels for done
            VoiceArray[z]->Loop ();
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

NOVATION_XL_C   LaunchControl;              //Novation controller
SYNTH_LFO_C     Lfo[2];                     //Hardware LFOs
VOICE_C*        VoiceArray[VOICE_COUNT];    //Oscillators, filters, routing controls
byte            PitchBendFactor[MAX_MIDI] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
bool            ResolutionMode      = false;
bool            MapSelectMode       = false;
bool            LoadMode            = false;
bool            SaveMode            = false;
byte            SelectedMidi        = 0;
short           SelectedMap         = -1;
short           SelectedVoice       = -1;
short           SelectedFilter      = -1;
short           SelectedConfig      = -1;
short           CurrentDisplayPage  = 0;

bool            SetTuning               = false;
short           CalibrationBaseDigital;
ushort          CalibrationAtoD;


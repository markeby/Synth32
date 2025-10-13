//#######################################################################
// Module:     FrontEnd.cpp
// Descrption: Synthesizer front end controller
// Creator:    markeby
// Date:       12/9/2024
//#######################################################################
#include <UHS2-MIDI.h>
#include <MIDI.h>

#include "../Common/SynthCommon.h"
#include "../Common/DispMessages.h"
#include "I2Cmessages.h"
#include "Osc.h"
#include "Settings.h"
#include "FrontEnd.h"
#include "Debug.h"

#ifdef DEBUG_SYNTH
//#define DEBUG_FUNC
//#define DEBUG_MIDI_MSG

static const char* Label  = "TOP";
static const char* LabelM = "M";
static const char* LabelS = "SEQ";
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#define DBGM(args...) {if(DebugMidi){DebugMsg(LabelM,mchan,args);}}
#define DBGS(args...) {if(DebugSeq){DebugMsg(LabelS,mchan,args);}}
#else
#define DBG(args...)
#define DBGM(args...)
#define DBGS(args...)
#endif

//###################################################################
static      USB Usb;
static      UHS2MIDI_CREATE_INSTANCE(&Usb, MIDI_PORT, Midi_0);
static      MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, Midi_1);
//static      MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, Midi_2);
static      MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, Midi_2);

using namespace MIDI_NAMESPACE;
using namespace DISP_MESSAGE_N;

typedef Message<MIDI_NAMESPACE::DefaultSettings::SysExMaxSize> MidiMessage;

//###################################################################
#ifdef DEBUG_MIDI_MSG
void FuncMessage0 (const MidiMessage& msg)
    {
    printf ("*** MIDI 0 MESSAGE:    type = 0x%02X  channel = %2d   data1 = 0x%02X   data2 = 0x%02X   length = 0x%02X\n",
            msg.type, msg.channel, msg.data1, msg.data2, msg.length);
    }
void FuncMessage1 (const MidiMessage& msg)
    {
    printf ("*** MIDI 1 MESSAGE:    type = 0x%02X   channel = %2d   data1 = 0x%02X   data2 = 0x%02X   length = 0x%02X\n",
            msg.type, msg.channel, msg.data1, msg.data2, msg.length);
    }
void FuncMessage2 (const MidiMessage& msg)
    {
    printf ("*** MIDI 2 MESSAGE:    type = 0x%02X   channel = %2d   data1 = 0x%02X   data2 = 0x%02X   length = 0x%02X\n",
            msg.type, msg.channel, msg.data1, msg.data2, msg.length);
    }
#endif

//###################################################################
void FuncError (int8_t err)
    {
    printf ("\n\n*** MIDI ERROR %d\n", err);
    }

//###################################################################
void FuncSystemReset0 (void)
    {
#ifdef DEBUG_FUNC
     printf ("\n\n*** MIDI RESET 0\n");
#endif
    }

//###################################################################
void FuncSystemReset1 (void)
    {
#ifdef DEBUG_FUNC
     printf ("\n\n*** MIDI RESET 1\n");
#endif
    }

//###################################################################
void FuncSystemReset2 (void)
    {
#ifdef DEBUG_FUNC
     printf ("\n\n*** MIDI RESET 2\n");
#endif
    }

//###################################################################
void FuncSystemEx (byte * array, unsigned size)
    {
#ifdef DEBUG_FUNC
    printf ("\n\n*** MIDI SYSEX");
    for ( short z = 0;  z < size;  z += 16 )
        {
        String st = "\n";
        for ( short zz = 0;  (zz < 16) && ((z + zz) < size);  zz++)
            st += String(array[z+zz], HEX) + " ";
        printf ("%s", st.c_str ());
        }
    printf("\n");
#endif
    }

//###################################################################
static void  FuncKeyDownDevice (uint8_t mchan, uint8_t key, uint8_t velocity)
    {
    if ( SynthFront.KeyboardKapture () )
        {
        if ( SynthFront.CurrentMidi() > 0 )
            mchan = SynthFront.CurrentMidi ();
        }
    DBGM ("Key down: %d  velocity: %d", mchan, key, velocity);
    SynthFront.KeyDown(mchan, key, velocity);
    }

//###################################################################
static void  FuncKeyUpDevice (uint8_t mchan, uint8_t key, uint8_t velocity)
    {
    if ( SynthFront.KeyboardKapture () )
        {
        if ( SynthFront.CurrentMidi() > 0 )
            mchan = SynthFront.CurrentMidi ();
        }
    DBGM ("Key up: %d  velocity: %d", mchan, key, velocity);
    SynthFront.KeyUp (mchan, key, velocity);
    }

//###################################################################
static void  FuncKeyDownSeq (uint8_t mchan, uint8_t key, uint8_t velocity)
    {
    DBGS ("Key down: %d  velocity: %d", mchan, key, velocity);
    SynthFront.KeyDown(mchan, key, velocity);
    }

//###################################################################
static void  FuncKeyUpSeq (uint8_t mchan, uint8_t key, uint8_t velocity)
    {
    DBGS ("Key up: %d  velocity: %d", mchan, key, velocity);
    SynthFront.KeyUp (mchan, key, velocity);
    }

//###################################################################
static void FuncControlChange (uint8_t mchan, uint8_t type, uint8_t value)
    {
    DBGM ("Function Control change: 0x%2.2X  value 0x%2.2X", mchan, type, value);
    SynthFront.FuncController (mchan, type, value);
    }

//###################################################################
static void SeqControlChange (uint8_t mchan, uint8_t type, uint8_t value)
    {
    DBGM ("Sequence Control change: 0x%2.2X  value 0x%2.2X", mchan, type, value);
    SynthFront.SeqController (mchan, type, value);
    }

//###################################################################
static void FuncPitchBend (uint8_t mchan, int value)
    {
    DBGM ("Pitch Bend value %d", mchan, value);
    value = (value + 8192) >> 2;
    if ( SynthFront.KeyboardKapture () )
        {
        if ( SynthFront.CurrentMidi() > 0 )
            mchan = SynthFront.CurrentMidi ();
        }
    SynthFront.PitchBend (mchan, value);
    }

//#######################################################################
//#######################################################################
SYNTH_FRONT_C::SYNTH_FRONT_C (G49_FADER_MIDI_MAP* g49map_fader, G49_ENCODER_MIDI_MAP* g49map_knob, G49_BUTTON_MIDI_MAP *g49map_button, XL_MIDI_MAP (*xl_map)[XL_MIDI_MAP_SIZE])
    {
    G49MidiMapFader     = g49map_fader;
    G49MidiMapEcoder    = g49map_knob;
    G49MidiMapButton    = g49map_button;
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
void SYNTH_FRONT_C::MidiCommandConfiguration ()
    {
    Midi_0.setHandleNoteOn               (FuncKeyDownDevice);
    Midi_1.setHandleNoteOn               (FuncKeyDownDevice);
    Midi_2.setHandleNoteOn               (FuncKeyDownSeq);

    Midi_0.setHandleNoteOff              (FuncKeyUpDevice);
    Midi_1.setHandleNoteOff              (FuncKeyUpDevice);
    Midi_2.setHandleNoteOff              (FuncKeyUpSeq);

    Midi_0.setHandleControlChange        (FuncControlChange);
    Midi_1.setHandleControlChange        (FuncControlChange);
    Midi_2.setHandleControlChange        (SeqControlChange);

    Midi_0.setHandlePitchBend            (FuncPitchBend);
    Midi_1.setHandlePitchBend            (FuncPitchBend);
//  Midi_2.setHandlePitchBend            (FuncPitchBend);

    Midi_0.setHandleError                (FuncError);
    Midi_1.setHandleError                (FuncError);
    Midi_2.setHandleError                (FuncError);

//  Midi_0.setHandleAfterTouchPoly       (AfterTouchPolyCallback fptr);
//  Midi_1.setHandleAfterTouchPoly       (AfterTouchPolyCallback fptr);
//  Midi_2.setHandleAfterTouchPoly       (AfterTouchPolyCallback fptr);

//  Midi_0.setHandleProgramChange        (ProgramChangeCallback fptr);
//  Midi_1.setHandleProgramChange        (ProgramChangeCallback fptr);
//  Midi_2.setHandleProgramChange        (ProgramChangeCallback fptr);

//  Midi_0.setHandleAfterTouchChannel    (AfterTouchChannelCallback fptr);
//  Midi_1.setHandleAfterTouchChannel    (AfterTouchChannelCallback fptr);
//  Midi_2.setHandleAfterTouchChannel    (AfterTouchChannelCallback fptr);

//  Midi_0.setHandleTimeCodeQuarterFrame (TimeCodeQuarterFrameCallback fptr);
//  Midi_1.setHandleTimeCodeQuarterFrame (TimeCodeQuarterFrameCallback fptr);
//  Midi_2.setHandleTimeCodeQuarterFrame (TimeCodeQuarterFrameCallback fptr);

//  Midi_0.setHandleSongPosition         (SongPositionCallback fptr);
//  Midi_1.setHandleSongPosition         (SongPositionCallback fptr);
//  Midi_2.setHandleSongPosition         (SongPositionCallback fptr);

//  Midi_0.setHandleSongSelect           (SongSelectCallback fptr);
//  Midi_1.setHandleSongSelect           (SongSelectCallback fptr);
//  Midi_2.setHandleSongSelect           (SongSelectCallback fptr);

//  Midi_0.setHandleTuneRequest          (TuneRequestCallback fptr);
//  Midi_1.setHandleTuneRequest          (TuneRequestCallback fptr);
//  Midi_2.setHandleTuneRequest          (TuneRequestCallback fptr);

//  Midi_0.setHandleClock                (ClockCallback fptr);
//  Midi_1.setHandleClock                (ClockCallback fptr);
//  Midi_2.setHandleClock                (ClockCallback fptr);

//  Midi_0.setHandleStart                (StartCallback fptr);
//  Midi_1.setHandleStart                (StartCallback fptr);
//  Midi_2.setHandleStart                (StartCallback fptr);

//  Midi_0.setHandleTick                 (TickCallback fptr);
//  Midi_1.setHandleTick                 (TickCallback fptr);
//  Midi_2.setHandleTick                 (TickCallback fptr);

//  Midi_0.setHandleContinue             (ContinueCallback fptr);
//  Midi_1.setHandleContinue             (ContinueCallback fptr);
//  Midi_2.setHandleContinue             (ContinueCallback fptr);

//  Midi_0.setHandleStop                 (StopCallback fptr);
//  Midi_1.setHandleStop                 (StopCallback fptr);
//  Midi_2.setHandleStop                 (StopCallback fptr);

//  Midi_0.setHandleActiveSensing        (ActiveSensingCallback fptr)
//  Midi_1.setHandleActiveSensing        (ActiveSensingCallback fptr);
//  Midi_2.setHandleActiveSensing        (ActiveSensingCallback fptr);

    Midi_0.setHandleSystemReset          (FuncSystemReset0);
    Midi_1.setHandleSystemReset          (FuncSystemReset1);
    Midi_2.setHandleSystemReset          (FuncSystemReset2);
    }

//#######################################################################
void SYNTH_FRONT_C::Begin (short voice, short mixer, short noise_digital, short lfo_control, short mod_mux_digital, short start_a_d)
    {
#ifdef DEBUG_MIDI_MSG
    Midi_0.setHandleMessage              (FuncMessage0);
    Midi_1.setHandleMessage              (FuncMessage1);
    Midi_2.setHandleMessage              (FuncMessage2);
#endif
    Midi_0.setHandleSystemExclusive      (FuncSystemEx);
//  Midi_1.setHandleSystemExclusive      (FuncSystemEx);
//  Midi_2.setHandleSystemExclusive      (FuncSystemEx);

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

    printf ("\t>>>\tMidi interfaces startup\n");
    // MIDI interface for Novation Launch Control XT
    while ( Usb.Init () == -1 )
        {
        delay (200);
        printf ("Usb midi init retry!\n");
        }
    printf ("\t>>>\tUsb midi ready\n");

    // MIDI interface for keyboard
    Serial1.begin (31250, SERIAL_8N1, RXD1, TXD1, false);
    Midi_1.begin (MIDI_CHANNEL_OMNI);
    printf ("\t>>>\tSerial1 midi ready\n");

    // MIDI interface from computer for sequencing
    Serial2.begin (31250, SERIAL_8N1, RXD2, TXD2, false);
    Midi_2.begin (MIDI_CHANNEL_OMNI);
    printf ("\t>>>\tSerial2 midi ready\n");

    LaunchControl.Begin (pMidiMapXL);

    MidiCommandConfiguration ();
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
void SYNTH_FRONT_C::FuncController (short mchan, byte type, byte value)
    {
    int   z;
    short chan = mchan - 1;;

    switch ( type )
        {
        case 1:
            // mod wheel
            SetLevelLFO (0, mchan, value);
            SetLevelLFO (1, mchan, value);
            SoftLFO.Multiplier (mchan, (float)value * PRS_SCALER * 0.5);
            DBG ("MIDI channel = %d   modulation = %d    ", mchan, value);
            break;
        case 7:          // Faders controls
            {
            G49_FADER_MIDI_MAP& m = G49MidiMapFader[chan];
            DBG ("%s > %d    ", m.Desc, value);
            if ( m.CallBack != nullptr )
                m.CallBack (m.Index, value);
            }
            break;
        case 10:          // Rotatational encoder controls
            {
            G49_ENCODER_MIDI_MAP& m = G49MidiMapEcoder[chan];
            z = m.Value + (value & 0x1F) * ((value & 0x40) ? -1 : 1);
            if ( z > 4095 )   z = 4095;
            if ( z < 1 )      z = 1;
            DBG("%s > %d (%d)", m.Desc, z, value);
            if ( m.CallBack != nullptr )
                G49MidiMapEcoder[chan].CallBack(m.Index, z);
            }
            break;
        case 16:
            {
            if ( SetTuning )
                {
                switch ( chan )
                    {
                    case 0 ... 7:
                        if ( (RunTime - TuningSelectionTime) < 5000 )
                            {
                            pVoice[chan]->TuningState (true);
                            DisplayMessage.TuningSelectSecond (chan);
                            TuningSelectionTime = 0;
                            }
                        else
                            {
                            for ( z = 0;  z < VOICE_COUNT;  z++ )
                                pVoice[z]->TuningState (false);
                            pVoice[chan]->TuningState (true);
                            DisplayMessage.TuningSelect (chan);
                            TuningSelectionTime = RunTime;
                            }
                        break;
                    case 8 ... 12:
                        this->TuningOutputBitFlip (chan - 8);
                        break;
                    default:
                        break;
                    }
                TuningChange = true;
                }
            else
                {
                G49_BUTTON_MIDI_MAP& m = G49MidiMapButton[chan];
                m.State = !m.State;
                DBG ("%s %d", m.Desc, m.State);
                if ( m.CallBack != nullptr )
                    m.CallBack (m.Index, m.State);
                }
            }
            break;
        case 17:
            {
            chan += 16;
            G49_BUTTON_MIDI_MAP& m = G49MidiMapButton[chan];
            m.State = !m.State;
            DBG ("%s %d", m.Desc, m.State);
            if ( m.CallBack != nullptr )
                m.CallBack (m.Index, m.State);
            }
            break;
        case 0x30 ... 0x47:
        case 0x60 ... 0x67:
            {
            chan = type - 0x30;
            XL_MIDI_MAP& m = pMidiMapXL[LaunchControl.GetCurrentMap()][chan];
            DBG ("%s > %d    ", m.Desc, value);
            if ( m.CallBack != nullptr )
                m.CallBack (m.Index, value);
            }
            break;
        case 0x48 ... 0x5F:
            {
            chan     = type - 0x30;                         // offset to start of control map
            bool tgl = value > 0x3C;                        // use color green as threshold for button down
            XL_MIDI_MAP& m = pMidiMapXL[LaunchControl.GetCurrentMap()][chan];
            DBG ("%s %s", m.Desc, (( tgl ) ? "Dn" : "Up"));

            if ( !tgl && (m.CallBack != nullptr) )
                m.CallBack (m.Index, (short)tgl);
            }
            break;

        case 120 ... 127:           // all notes stop
            DBG ("All note clear");
            Clear ();
            break;

        default:
            DBG ("Invalid type code = %d [%x]\n", type, type);
            break;
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
            pb = SynthConfig.Voice[CurrentVoiceSelected >> 1].GetButtonStateOsc ();
            break;
        case XL_MIDI_MAP_FLT:
            pb = SynthConfig.Voice[CurrentFilterSelected >> 1].GetButtonStateFlt ();
            break;
        case XL_MIDI_MAP_LFO:
            pb = SynthConfig.GetButtonStateLfo ();
            break;
        default:
            break;
        }

    LaunchControl.SelectTemplate (index, pb);
    }

//#######################################################################
//#######################################################################
void NOVATION_XL_C::Begin (XL_MIDI_MAP (*xl_map)[XL_MIDI_MAP_SIZE])
    {
    pMidiMap = xl_map;

    for ( short z = 0;  z < 20;  z++ )
        {
        Usb.Task    ();
        Midi_0.read ();
        delay (100);
        }
    }

//#######################################################################
void NOVATION_XL_C::SendTo (unsigned length, byte* buff)
    {
    // Debugging message
//  String str;
//  for ( short z = 0;  z < length;  z++ )
//      str += " " + String (buff[z], 16);
//  printf("@@ message %s\n", str.c_str ());

    // Message sent to deice
    Midi_0.sendSysEx (length, buff, false);
    }

//#######################################################################
void NOVATION_XL_C::ResetAllLED (byte index)
    {
    Midi_0.send ((MidiType)(0x90 | index), 0, 0, 0);
    delay (100);
    }



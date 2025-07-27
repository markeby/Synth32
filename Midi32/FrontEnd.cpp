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
#define DBG(args...) {if(DebugSynth){DebugMsg(Label,DEBUG_NO_INDEX,args);}}
#define DBGM(args...) {if(DebugMidi){DebugMsg(LabelM,DEBUG_NO_INDEX,args);}}
#else
#define DBG(args...)
#define DBGM(args...)
#endif

//###################################################################
static      USB Usb;
static      UHS2MIDI_CREATE_INSTANCE(&Usb, MIDI_PORT, Midi_0);
static      MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, Midi_1);
static      MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, Midi_2);

#define SENDcc0(k,c) {Midi_0.send(midi::MidiType::ControlChange,k,c,1);DBGM("code 0x%X Color = 0x%X  [%s]", k, c, __PRETTY_FUNCTION__);}

using namespace MIDI_NAMESPACE;
typedef Message<MIDI_NAMESPACE::DefaultSettings::SysExMaxSize> MidiMessage;

//###################################################################
//void FuncMessage (const MidiInterface::MidiMessage& msg)
#ifdef DEBUG_MIDI_MSG
void FuncMessage0 (const MidiMessage& msg)
    {
    printf ("*** MIDI 0 MESSAGE:    type = %X   channel = %X   data1 = %X   data2 = %X   length = %X\n",
            msg.type, msg.channel, msg.data1, msg.data2, msg.length);
    }
void FuncMessage1 (const MidiMessage& msg)
    {
    printf ("*** MIDI 1 MESSAGE:    type = %X   channel = %X   data1 = %X   data2 = %X   length = %X\n",
            msg.type, msg.channel, msg.data1, msg.data2, msg.length);
    }
void FuncMessage2 (const MidiMessage& msg)
    {
    printf ("*** MIDI 2 MESSAGE:    type = %X   channel = %X   data1 = %X   data2 = %X   length = %X\n",
            msg.type, msg.channel, msg.data1, msg.data2, msg.length);
    }
#endif

//###################################################################
void FuncError (int8_t err)
    {
#ifdef DEBUG_FUNC
     printf ("\n\n*** MIDI ERROR %d\n", err);
#endif
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
static void  FuncKeyDown (uint8_t mchan, uint8_t key, uint8_t velocity)
    {
    DBGM ("Key down  MIDI: %d  key: %d  velocity: %d", mchan, key, velocity);
    SynthFront.KeyDown (mchan, key, velocity);
    }

//###################################################################
static void  FuncKeyUp (uint8_t mchan, uint8_t key, uint8_t velocity)
    {
    DBGM ("Key up  MIDI: %d  key: %d  velocity: %d", mchan, key, velocity);
    SynthFront.KeyUp (mchan, key, velocity);
    }

//###################################################################
static void FuncController (uint8_t mchan, uint8_t type, uint8_t value)
    {
    SynthFront.Controller (mchan, type, value);
    DBGM ("Controller  MIDI %2.2X  type %2.2X  value %2.2X", mchan, type, value);
    }

//###################################################################
static void FuncPitchBend (uint8_t mchan, int value)
    {
    DBGM ("Pitch Bend  MIDI %2.2X  value %d", mchan, value);
    value = (value + 8192) >> 2;
    SynthFront.PitchBend (mchan, value);
    }

//#######################################################################
//#######################################################################
SYNTH_FRONT_C::SYNTH_FRONT_C (G49_FADER_MIDI_MAP* g49map_fader, G49_ENCODER_MIDI_MAP* g49map_knob, G49_BUTTON_MIDI_MAP *g49map_button, XL_MIDI_MAP (*xl_map)[XL_MIDI_MAP_SIZE])
    {
    this->G49MidiMapFader         = g49map_fader;
    this->G49MidiMapEcoder    = g49map_knob;
    this->G49MidiMapButton        = g49map_button;
    this->pMidiMapXL              = xl_map;
    this->Down.Key            = 0;
    this->Down.Trigger        = 0;
    this->Down.Velocity       = 0;
    this->Up.Key              = 0;
    this->Up.Trigger          = 0;
    this->Up.Velocity         = 0;
    this->SetTuning           = false;
    this->TuningChange        = false;
    this->CurrentMapSelected  = 0;
    this->CurrentMidiSelected = 1;
    this->CalibrationPhase    = 0;
    this->LoadSaveSelection   = 1;
    this->NovationCounter     = 1;
    }

//#######################################################################
void SYNTH_FRONT_C::Begin (short voice, short mux_digital, short noise_digital, short lfo_control, short mod_mux_digital, short start_a_d)
    {
#ifdef DEBUG_MIDI_MSG
    Midi_0.setHandleMessage              (FuncMessage0);
#endif
    Midi_0.setHandleNoteOn               (FuncKeyDown);
    Midi_0.setHandleNoteOff              (FuncKeyUp);
    Midi_0.setHandleControlChange        (FuncController);
    Midi_0.setHandlePitchBend            (FuncPitchBend);
    Midi_0.setHandleError                (FuncError);
//  Midi_0.setHandleAfterTouchPoly       (AfterTouchPolyCallback fptr);
//  Midi_0.setHandleProgramChange        (ProgramChangeCallback fptr);
//  Midi_0.setHandleAfterTouchChannel    (AfterTouchChannelCallback fptr);
    Midi_0.setHandleSystemExclusive      (FuncSystemEx);
//  Midi_0.setHandleTimeCodeQuarterFrame (TimeCodeQuarterFrameCallback fptr);
//  Midi_0.setHandleSongPosition         (SongPositionCallback fptr);
//  Midi_0.setHandleSongSelect           (SongSelectCallback fptr);
//  Midi_0.setHandleTuneRequest          (TuneRequestCallback fptr);
//  Midi_0.setHandleClock                (ClockCallback fptr);
//  Midi_0.setHandleStart                (StartCallback fptr);
//  Midi_0.setHandleTick                 (TickCallback fptr);
//  Midi_0.setHandleContinue             (ContinueCallback fptr);
//  Midi_0.setHandleStop                 (StopCallback fptr);
//  Midi_0.setHandleActiveSensing        (ActiveSensingCallback fptr);
    Midi_0.setHandleSystemReset          (FuncSystemReset0);

#ifdef DEBUG_MIDI_MSG
    Midi_1.setHandleMessage              (FuncMessage1);
#endif
    Midi_1.setHandleNoteOn               (FuncKeyDown);
    Midi_1.setHandleNoteOff              (FuncKeyUp);
    Midi_1.setHandleControlChange        (FuncController);
    Midi_1.setHandlePitchBend            (FuncPitchBend);
//  Midi_1.setHandleError                (ErrorCallback fptr);
//  Midi_1.setHandleAfterTouchPoly       (AfterTouchPolyCallback fptr);
//  Midi_1.setHandleProgramChange        (ProgramChangeCallback fptr);
//  Midi_1.setHandleAfterTouchChannel    (AfterTouchChannelCallback fptr);
//  Midi_1.setHandleSystemExclusive      (SystemExclusiveCallback fptr);
//  Midi_1.setHandleTimeCodeQuarterFrame (TimeCodeQuarterFrameCallback fptr);
//  Midi_1.setHandleSongPosition         (SongPositionCallback fptr);
//  Midi_1.setHandleSongSelect           (SongSelectCallback fptr);
//  Midi_1.setHandleTuneRequest          (TuneRequestCallback fptr);
//  Midi_1.setHandleClock                (ClockCallback fptr);
//  Midi_1.setHandleStart                (StartCallback fptr);
//  Midi_1.setHandleTick                 (TickCallback fptr);
//  Midi_1.setHandleContinue             (ContinueCallback fptr);
//  Midi_1.setHandleStop                 (StopCallback fptr);
//  Midi_1.setHandleActiveSensing        (ActiveSensingCallback fptr);
    Midi_1.setHandleSystemReset          (FuncSystemReset1);

#ifdef DEBUG_MIDI_MSG
    Midi_2.setHandleMessage              (FuncMessage2);
#endif
    Midi_2.setHandleNoteOn               (FuncKeyDown);
    Midi_2.setHandleNoteOff              (FuncKeyUp);
//  Midi_2.setHandleControlChange        (FuncController);
//  Midi_2.setHandlePitchBend            (FuncPitchBend);
//  Midi_2.setHandleError                (ErrorCallback fptr);
//  Midi_2.setHandleAfterTouchPoly       (AfterTouchPolyCallback fptr);
//  Midi_2.setHandleProgramChange        (ProgramChangeCallback fptr);
//  Midi_2.setHandleAfterTouchChannel    (AfterTouchChannelCallback fptr);
//  Midi_2.setHandleSystemExclusive      (SystemExclusiveCallback fptr);
//  Midi_2.setHandleTimeCodeQuarterFrame (TimeCodeQuarterFrameCallback fptr);
//  Midi_2.setHandleSongPosition         (SongPositionCallback fptr);
//  Midi_2.setHandleSongSelect           (SongSelectCallback fptr);
//  Midi_2.setHandleTuneRequest          (TuneRequestCallback fptr);
//  Midi_2.setHandleClock                (ClockCallback fptr);
//  Midi_2.setHandleStart                (StartCallback fptr);
//  Midi_2.setHandleTick                 (TickCallback fptr);
//  Midi_2.setHandleContinue             (ContinueCallback fptr);
//  Midi_2.setHandleStop                 (StopCallback fptr);
//  Midi_2.setHandleActiveSensing        (ActiveSensingCallback fptr);
    Midi_2.setHandleSystemReset          (FuncSystemReset2);

    printf ("\t>>>\tMidi interfaces startup\n");
    while ( Usb.Init () == -1 )
        {
        delay (200);
        printf ("Usb midi init retry!\n");
        }
    printf ("\t>>>\tUsb midi ready\n");

    Serial1.begin (31250, SERIAL_8N1, RXD1, TXD1, false);
    Midi_1.begin (MIDI_CHANNEL_OMNI);
    printf ("\t>>>\tSerial1 midi ready\n");

    Serial2.begin (31250, SERIAL_8N1, RXD2, TXD2, false);
    Midi_2.begin (MIDI_CHANNEL_OMNI);
    printf ("\t>>>\tSerial2 midi ready\n");

    // Setup ports for calibration
    CalibrationBaseDigital = mod_mux_digital;

    printf ("\t>>>\tSynth channels\n");
    short osc = voice;
    for ( int z = 0;  z < VOICE_COUNT;  z++ )
        {
        this->pVoice[z] = new VOICE_C(z, osc, mux_digital, mod_mux_digital, noise_digital, EnvADSL);
        osc         += 8;
        mux_digital += 1;
        if ( z & 1 )
            {
            mod_mux_digital += 1;
            noise_digital   += 4;
            voice           += 28;
            osc              = voice;
            }
        }

    this->Lfo[0].Begin (0, lfo_control + 8 , lfo_control);
    this->Lfo[1].Begin (1, lfo_control + 8 + 6, lfo_control + 3);
    this->CalibrationAtoD = start_a_d + 2;
    this->ResolutionMode = true;
    }

//#######################################################################
void SYNTH_FRONT_C::Clear ()
    {
    for ( int z = 0;  z < VOICE_COUNT; z++ )
        this->pVoice[z]->Clear ();
    }

//#######################################################################
void SYNTH_FRONT_C::PageAdvance ()
    {
    byte  m    = this->CurrentMidiSelected;
    short next = this->CurrentMapSelected + 1;

    while ( next < MAP_COUNT )
        {
        if ( m == this->SynthConfig.Voice[next].GetVoiceMidi () )
            {
            next++;
            continue;
            }
        this->CurrentMidiSelected  = this->SynthConfig.Voice[next].GetVoiceMidi ();
        this->CurrentMapSelected   = next;
        this->CurrentVoiceSelected = next >> 1;
        DisplayMessage.SelectVoicePage (next);
        return;
        }
    if ( next < (short)DISP_MESSAGE_N::PAGE_C::PAGE_MIDI_MAP )
        {
        this->CurrentMapSelected   = -1;
        DisplayMessage.Page (next);
        this->CurrentMidiSelected  = 0;
        this->CurrentVoiceSelected = -1;
        }
    else
        {
        this->CurrentMapSelected   = 0;
        this->CurrentVoiceSelected = 0;
        this->CurrentMidiSelected  = this->SynthConfig.Voice[this->CurrentMapSelected].GetVoiceMidi ();
        DisplayMessage.SelectVoicePage (this->CurrentMapSelected);
        }
    }

//#######################################################################
void SYNTH_FRONT_C::Controller (short mchan, byte type, byte value)
    {
    int   z;
    short chan = mchan - 1;;

    switch ( type )
        {
        case 1:
            // mod wheel
            this->SetLevelLFO (0, mchan, value);
            this->SetLevelLFO (1, mchan, value);
            SoftLFO.Multiplier (mchan, (float)value * PRS_SCALER * 0.5);
            DBG ("MIDI channel = %d   modulation = %d    ", mchan, value);
            break;
        case 7:          // Faders controls
            {
            G49_FADER_MIDI_MAP& m = this->G49MidiMapFader[chan];
            if ( m.CallBack != nullptr )
                {
                DBG ("%s > %d    ", m.Desc, value);
                m.CallBack (m.Index, value);
                }
            }
            break;
        case 10:          // Rotatational encoder controls
            {
            G49_ENCODER_MIDI_MAP& m = this->G49MidiMapEcoder[chan];
            if ( m.CallBack != nullptr )
                {
                z = m.Value + (value & 0x1F) * ((value & 0x40) ? -1 : 1);
                if ( z > 4095 )   z = 4095;
                if ( z < 1 )      z = 1;
                DBG("%s > %d (%d)", m.Desc, z, value);
                G49MidiMapEcoder[chan].CallBack(m.Index, z);
                }
            }
            break;
        case 16:
            {
            if ( this->SetTuning && (chan < 8) )
                {
                for ( z = 0;  z < VOICE_COUNT;  z++ )
                    this->TuningOn[z] = false;
                this->TuningOn[chan] = true;
                DBG ("Tuning for channel %d", chan);
                DisplayMessage.TuningSelect (chan);
                this->TuningChange = true;
                return;
                }
            G49_BUTTON_MIDI_MAP& m = this->G49MidiMapButton[chan];
            if ( m.CallBack != nullptr )
                {
                m.State = !m.State;
                DBG ("%s %d", m.Desc, m.State);
                m.CallBack (m.Index, m.State);
                }
            }
            break;
        case 17:
            {
            chan += 16;
            G49_BUTTON_MIDI_MAP& m = this->G49MidiMapButton[chan];

            if ( m.CallBack != nullptr )
                {
                m.State = !m.State;
                DBG ("%s %d", m.Desc, m.State);
                m.CallBack (m.Index, m.State);
                }
            }
            break;
        case 0x30 ... 0x47:
        case 0x60 ... 0x67:
            {
            chan = type - 0x30;
            XL_MIDI_MAP& m = this->pMidiMapXL[this->LaunchControl.GetCurrentMap()][chan];
            if ( m.CallBack != nullptr )
                {
                DBG ("%s > %d    ", m.Desc, value);
                m.CallBack (m.Index, value);
                }
            }
            break;
        case 0x48 ... 0x5F:
            {
            chan           = type - 0x30;                           // offset to start of control map
            bool    tgl    = value > 0x3C;                          // use color white to show selectable and green to show selected.
            XL_MIDI_MAP& m = this->pMidiMapXL[this->LaunchControl.GetCurrentMap()][chan];

            if ( m.CallBack != nullptr )
                {
                DBG ("%s %s", m.Desc, (( tgl ) ? "ON" : "Off"));
                m.CallBack (m.Index, (short)tgl);
                }
            }
            break;

        case 120 ... 127:           // all notes stop
            this->Clear ();
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

    this->Lfo[0].Loop ();
    this->Lfo[1].Loop ();

    Usb.Task    ();
    Midi_0.read ();
    Midi_1.read ();
    Midi_2.read ();

    if ( !SetTuning )
        {
        if ( this->Up.Trigger )
            {
            DBG ("Key up > %d", this->Up.Key);
            for ( int z = 0;  z < VOICE_COUNT;  z++ )
                this->pVoice[z]->NoteClear (this->Up.Trigger, this->Up.Key);
            this->Up.Trigger = 0;
            }

        if ( this->Down.Trigger )                          // Key went down so look for a channel to use
            {
            for ( int z = 0;  z <  VOICE_COUNT;  z++ )
                {
                VOICE_C& ch = *(pVoice[z]);
                if ( this->Down.Trigger == ch.GetMidi () )
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
                this->Lfo[0].HardReset (this->Down.Trigger);
                this->Lfo[1].HardReset (this->Down.Trigger);
                this->pVoice[doit]->NoteSet(this->Down.Trigger, this->Down.Key, this->Down.Velocity);   // set the channel
                }
            DBG ("Key down > %d   Velocity > %d  Port > %d", this->Down.Key, this->Down.Velocity, doit);
            this->Down.Trigger = 0;                         // release the trigger
            }

        EnvADSL.Loop ();                                    // process all envelope generators
        for ( int z = 0;  z < VOICE_COUNT;  z++ )           // Check all channels for done
            this->pVoice[z]->Loop ();
        I2cDevices.UpdateDigital ();
        I2cDevices.UpdateAnalog  ();     // Update D/A ports
        }
    else
        this->Tuning ();

    if ( --this->NovationCounter == 0 )
        {
        this->LaunchControl.Loop ();
        this->NovationCounter = 800;
        }
    }

//#####################################################################
void SYNTH_FRONT_C::NonOscPageSelect (DISP_MESSAGE_N::PAGE_C page)
    {
    DisplayMessage.Page (page);
    this->CurrentMidiSelected   =  0;
    this->CurrentMapSelected    = -1;
    this->CurrentVoiceSelected  = -1;
    }

//#####################################################################
void SYNTH_FRONT_C::TemplateSelect (byte index)
    {
    byte *pb = nullptr;
    if ( index == XL_MIDI_MAP_OSC )
        pb = SynthConfig.Voice[this->CurrentVoiceSelected].GetButtonState();
    this->LaunchControl.SelectTemplate (index, pb);
    }

//#######################################################################
//#######################################################################
    NOVATION_XL_C::NOVATION_XL_C ()
    {
    this->CurrentMap = 0;
    this->FlashState = false;
    this->pButtonState = nullptr;
    }

//#######################################################################
void NOVATION_XL_C::TemplateReset (byte index)
    {
    static byte midi_sysex_led[7 + ((XL_MIDI_MAP_SIZE - 8) * 2)] =
        { 0x00, 0x20, 0x29, 0x02, 0x11, 0x78 };

    delay (22);
    Midi_0.send ((MidiType)(0x90 | index), 0, 0, 0);
    memset(&midi_sysex_led[7], 0, sizeof (midi_sysex_led) - 7);         // clear the map space in the sysex message
    midi_sysex_led[6] = index;                                          // setup template index in message
    short zi = 7;                                                       // start loading array at this index
    for ( int z = 0;  z < XL_MIDI_MAP_SIZE;  z++ )
        {
        if ( this->pMidiMap[index][z].Color != 0 )                      // if color is non zero
            {
            midi_sysex_led[zi] = z;                                     // setup index of LED
            midi_sysex_led[zi + 1] = this->pMidiMap[index][z].Color;    // get color value for LED
            }
        zi += 2;                                                        // bump target array index
        }
    delay (22);
    Midi_0.sendSysEx (sizeof (midi_sysex_led), midi_sysex_led, false);  // Send message to set all LEDs
    }

//#######################################################################
void NOVATION_XL_C::Begin (XL_MIDI_MAP (*xl_map)[XL_MIDI_MAP_SIZE])
    {
    this->pMidiMap = xl_map;

    for ( int z = 0;  z < XL_MIDI_MAP_PAGES;  z++ )
        this->TemplateReset (z);
    this->SelectTemplate (0);                                               // starting with tempate zero
    }

//#######################################################################
void NOVATION_XL_C::Loop ()
    {
    switch ( this->CurrentMap  )
        {
        case XL_MIDI_MAP_OSC:
            this->UpdateButtons ();
            break;
        case XL_MIDI_MAP_FILTER:
            break;
        case XL_MIDI_MAP_LFO:
            break;
        case XL_MIDI_MAP_MAPPING:
            this->FlashState = !this->FlashState;
            this->SetColor(XL_MIDI_MAP_MAPPING, 40, XL_LED::OFF);
            this->SetColor(XL_MIDI_MAP_MAPPING, 42, XL_LED::OFF);
            this->SetColor(XL_MIDI_MAP_MAPPING, 43, ( this->FlashState ) ? XL_LED::AMBER : XL_LED::OFF);
            break;
        case XL_MIDI_MAP_SPARE:
            break;
        default:        // should never get here
            break;
        }
    }

//#######################################################################
void NOVATION_XL_C::ResetUSB ()
    {
    DBG ("USB reset \n\n");

    while ( Usb.Init () == -1 )
        {
        DBG ("Usb midi init retry!\n");
        delay (200);
        }
    delay (3000);
    }

//#######################################################################
void NOVATION_XL_C::SelectTemplate (byte index, byte* pbuttons)
    {
    static byte midi_msg_template[] = { 0x00, 0x20, 0x29, 0x02, 0x11, 0x77, 0x00 };

    this->pButtonState = pbuttons;
    delay (10);
    midi_msg_template[6] = index;
    this->CurrentMap     = index;
    Midi_0.sendSysEx (sizeof (midi_msg_template), midi_msg_template, false);
    this->UpdateButtons ();
    }

//#####################################################################
void NOVATION_XL_C::UpdateButtons ()
    {
    static byte midi_button_sysex[7 + (XL_BUTTON_COUNT * 2)] = { 0x00, 0x20, 0x29, 0x02, 0x11, 0x78, 0x00 };
    String st;

    if ( this->pButtonState != nullptr )
        {
        delay(10);
        midi_button_sysex[6] = this->CurrentMap;
        for ( int z = 0;  z < XL_BUTTON_COUNT;  z++ )
            {
            midi_button_sysex[(z * 2) + 7] = XL_BUTTON_START + z;
            midi_button_sysex[(z * 2) + 8] = this->pButtonState[z];
            }
        Midi_0.sendSysEx (sizeof (midi_button_sysex), midi_button_sysex, false);
        }
    }

//#####################################################################
void NOVATION_XL_C::SetColor (byte index, byte led, XL_LED color)
    {
    static byte midi_color_sysex[9] = { 0x00, 0x20, 0x29, 0x02, 0x11, 0x78 };

    delay (10);
    midi_color_sysex[6] = index;
    midi_color_sysex[7] = led;
    midi_color_sysex[8] = (byte)color;
    Midi_0.sendSysEx (sizeof (midi_color_sysex), midi_color_sysex, false);
    }



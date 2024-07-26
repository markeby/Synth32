//#######################################################################
// Module:     SerialMonitor.ino
// Descrption: Serial control for setup and debug
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include "config.h"
#include "settings.h"
#include "SerialMonitor.h"
#include "SynthFront.h"
#include "UpdateOTA.h"
using namespace SERIAL_MONITOR;

//#######################################################################
inline void DispRunTime (void)
    {
    float fsc = RunTime * 0.000001;
    float fmn = fsc / 60;
    float fhr = fmn / 60;
    int   hr  = fhr;
    int   mhr = hr * 60.0;
    int   mn  = fmn - mhr;
    float sc  = fsc - ((float)(mhr + mn) * 60.0);
    Serial << hr << ":" << mn << ":" << sc << endl;
    }

//#######################################################################
const char* StateDebug (bool d)
    {
    static const char* StateOn  = "  <ON>";
    static const char* StateOff = " <off>";

    if ( d )
        return (StateOn);
    return (StateOff);
    }

//#######################################################################
void MONITOR_C::DumpStats (void)
    {
    static const char* hh = " ## ";

    Serial << hh << ESP.getChipModel () << " rev " << ESP.getChipRevision () << ", ";
    Serial << hh << ESP.getChipCores () << " cores.  " << ESP.getCpuFreqMHz () << " MHz" << endl;
    Serial << hh << " SDK " << ESP.getSdkVersion () << endl;
    Serial << hh << "       Sketch size = " << ESP.getSketchSize () << endl;
    Serial << hh << "         Heap size = " << ESP.getHeapSize () << endl;
    Serial << hh << " Minimum heap size = " << ESP.getMinFreeHeap () << endl;
    Serial << hh << "  Max alloced Heap = " << ESP.getMaxAllocHeap () << endl;
    Serial << hh << "         Free heap = " << ESP.getFreeHeap () << endl;
    Serial << hh << " Free sketch space = " << ESP.getFreeSketchSpace () << endl;
// Current part has no Psram
//  Serial << hh << "    Min free Psram = " << ESP.getMinFreePsram () << endl;
//  Serial << hh << " Max alloced Psram = " << ESP.getMaxAllocPsram () << endl;
//  Serial << hh << "        Free Psram = " << ESP.getFreePsram () << endl;
    Serial << hh << "   Flash chip size = " << ESP.getFlashChipSize () << endl;
    Serial << hh << "  Flash chip speed = " << ESP.getFlashChipSpeed() << endl;
    Serial << hh << "   Flash chip mode = " << ESP.getFlashChipMode() << endl << endl;
    Serial << hh << "        Update URL = " << UpdateOTA.GetIP() << endl << endl;
    Serial << hh << "       Runing Time = "; DispRunTime ();
    Serial << hh << "     Last interval = " << DeltaTimeMicro << " uSec" << endl;
    Serial << hh << "  Average interval = " << DeltaTimeMicroAvg << " uSec" << endl;
    }

//#######################################################################
bool MONITOR_C::Save (SMODE m)
    {
    int z;
    switch ( m )
        {
        case INSSID:
            Settings.PutSSID (this->InputString);
            break;
        case INPWD:
            Settings.PutPasswd (this->InputString);
            break;
        default:
            break;
        }

    this->InputString.clear ();
    return (true);
    }

//#######################################################################
void MONITOR_C::InputPrompt (int num, const char* text)
    {
    Serial << num << "\n\n" << text << " >" << this->InputString;
    }

//#######################################################################
void MONITOR_C::Tuning ()
    {
    SynthFront.StartTuning ();
    this->Menu ();
    }

//#######################################################################
void MONITOR_C::Reset ()
    {
    Serial << endl << "  ********** Reset requested **********";
    Serial << endl << endl;
    Serial << endl << endl;
    Serial << endl << endl;
    ESP.restart ();
    }

//#######################################################################
bool MONITOR_C::PromptZap (void)
    {
    char in_char = (char)(Serial.read () & 0xFF);
    Serial << in_char << "\n\n";
    switch ( in_char )
        {
        case 'y':
        case 'Y':
            Settings.ClearAll ();
            Serial << 9 << endl << "\nCleared preferences." << endl;
            ESP.restart ();
        case 'n':
        case 'N':
        default:
            return (true);
            break;
        }
    return (false);
    }

//#######################################################################
void MONITOR_C::MenuSel (void)
    {
    char s = Serial.read ();

    if ( s == 27 )                  // detect escape 00x1B
        {
        int funct = 0;
        while ( Serial.available () )
            {
            s = Serial.read ();
            switch ( s )
                {
                case 91:            // 0x5B
                    if ( !funct )
                        funct++;
                    break;
                case 50:            // 0x32
                    if ( funct == 1 )
                        funct++;
                    break;
                case 52:            // 0x34
                    if ( funct == 2 )
                        funct++;
                    break;
                case 126:           // 0x7E
                    if ( funct == 3 )
                        {
                        ESP.restart ();
                        }
                    break;
                case 0x41:          // arrow up
                    break;
                case 0x42:          // arrow down
                    break;
                case 0x43:          // arrow right
                    break;
                case 0x44:          // arrow left
                    break;
                default:
                    break;
                }
            }
        return;
        }

    switch ( this->InputMode )
        {
        case CMD:
            switch ( s )
                {
                case 's':
                    Serial << endl;
                    this->DumpStats ();
                    this->Mode (MENU);
                    break;
                case '1':
                    DebugMidi  = !DebugMidi;
                    Serial << "  MIDI debugging " << (( DebugMidi ) ? "Enabled" : "Disabled") << endl;
                    this->Mode (MENU);
                    break;
                case '2':
                    DebugI2C  = !DebugI2C;
                    Serial << "  I2C debugging " << (( DebugI2C ) ? "Enabled" : "Disabled") << endl;
                    this->Mode (MENU);
                    break;
                case '3':
                    DebugOsc   = !DebugOsc;
                    Serial << "  Oscillator debugging " << (( DebugOsc ) ? "Enabled" : "Disabled") << endl;
                    this->Mode (MENU);
                    break;
                case '4':
                    DebugSynth = !DebugSynth;
                    Serial << "  Synth debugging " << (( DebugSynth ) ? "Enabled" : "Disabled") << endl;
                    this->Mode (MENU);
                    break;
                case '5':
                    DebugSynth = !DebugSynth;
                    Serial << "  Display debugging " << (( DebugDisp ) ? "Enabled" : "Disabled") << endl;
                    this->Mode (MENU);
                    break;
                case 'S':
                    this->InputString = Settings.GetSSID ();
                    this->InputPrompt (7, "  Enter SSID");
                    this->Mode (INSSID);
                    break;
                case 'P':
                    this->InputString = Settings.GetPasswd ();
                    this->InputPrompt (8, "  Enter PWD");
                    this->Mode (INPWD);
                    break;
                case 'C':
                    this->InputPrompt (9, "  Cleared preferences.");
                    this->Mode (ZAP);
                    break;
                case 'q':
                    AnalogDiagEnabled = true;
                    SynthActive       = false;
                    this->Mode (MENU);
                    break;
                case ' ':           // Just move the cursor down a couple of lines
                    Serial << "...\n\n";
                    break;
                case 'Z':
                    break;
                default:
                    Serial << "       ??" << endl;
                    this->Mode (MENU);
                    break;
                }
            break;
        }
    }

//#######################################################################
void MONITOR_C::Menu (void)
    {
    Serial << endl;
    Serial << "\t######   Synth32 -- Midi32  ######" << endl;
    Serial << "\t######    Midi Subsystem    ######" << endl;
    if (  SynthFront.IsInTuning () )
        Serial << "\t******     Tuning mode      ******" << endl;
    if ( AnalogDiagEnabled )
        Serial << "\t******    D/A Test mode     ******" << endl;
    Serial << StateDebug (DebugMidi)  << "\t1   - Debug MIDI interface   " << endl;
    Serial << StateDebug (DebugI2C)   << "\t2   - Debug I2C interface " << endl;
    Serial << StateDebug (DebugOsc)   << "\t3   - Debug Oscillators & Noise   " << endl;
    Serial << StateDebug (DebugSynth) << "\t4   - Debug Synth            " << endl;
    Serial << StateDebug (DebugDisp ) << "\t5   - Debug Display Interface " << endl;
    Serial << "\tq   - D/A Diagnostic mode" << endl;
    Serial << "\ts   - Dump process Stats" << endl;
    Serial << "\tZ   - Test function" << endl;
    Serial << "\tS   - SSID" << endl;
    Serial << "\tP   - Password" << endl;
    Serial << "\tC   - Clear Preferences" << endl;
    Serial << "\tF12 - Reset" << endl;
    Serial << endl;
    }

//#######################################################################
void MONITOR_C::TextIn (void)
    {
    char in_char = (char)(Serial.read () & 0xFF);
    switch ( in_char )
        {
        case '\r':              // return (enter)
            if ( this->Save (this->InputMode) )
                this->Mode (MENU);
            break;
        case (char)127:         // backspace
            if ( this->InputString.length () )
                {
                Serial << (char)8 << ' ' << (char)8;
                this->InputString.remove (this->InputString.length () - 1);
                }
            break;
        case (char)27:          // escape for exit with no change
            this->Mode (MENU);
            break;
        case '\t':              // Tab for special loops
            this->Save (this->InputMode);
            break;
        default:                // all other characters go into string
            this->InputString += in_char;
            Serial << in_char;
            break;
        }
    }

//#######################################################################
MONITOR_C::MONITOR_C (void)
    {
    this->InputString = "";
    this->InputMode   = MENU;
    }

//#######################################################################
MONITOR_C::~MONITOR_C (void)
    {
    }

//#######################################################################
void MONITOR_C::Begin (void)
    {
    Serial.begin (115200);
    this->DumpStats ();
    Serial << "\n\n\nHow the hell did I get here man?\n\n\n";
    }

//#######################################################################
bool MONITOR_C::Detect (void)
    {
    return (Serial.available ());
    }

//#######################################################################
void MONITOR_C::Loop (void)
    {
    if ( this->InputMode != MENU )
        {
        while ( Serial.available () )
            {
            switch ( this->InputMode )
                {
                case CMD:
                    this->MenuSel ();
                    break;
                case INSSID:
                case INPWD:
                    this->TextIn ();
                    break;
                case ZAP:
                    if ( this->PromptZap () )
                        this->Mode(MENU);
                    break;
                default:
                    break;
                }
            }
        }
    else
        {
        this->Menu ();
        this->Mode (CMD);
        }
    }

MONITOR_C Monitor;


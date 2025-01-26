//#######################################################################
// Module:     SerialMonitor.ino
// Descrption: Serial control for setup and debug
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include <chip-debug-report.h>
#include "config.h"
#include "I2Cmessages.h"
#include "settings.h"
#include "SerialMonitor.h"
#include "FrontEnd.h"
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

    Serial << "==========================================" << endl;
    printBeforeSetupInfo ();
    Serial << "==========================================" << endl << endl;
    Serial << hh << "        Stack size = " << getArduinoLoopTaskStackSize () << endl;
    Serial << hh << "  Free stack space = " << uxTaskGetStackHighWaterMark (NULL) << endl << endl;
    printAfterSetupInfo ();
    Serial << "==========================================" << endl << endl;
    Serial << hh << "        Update URL = " << UpdateOTA.GetIP() << endl << endl;
    Serial << hh << "       Runing Time = ";
    DispRunTime ();
    Serial << hh << "     Last interval = " << DeltaTimeMilli << " mSec" << endl;
    Serial << hh << "  Average interval = " << DeltaTimeMilliAvg << " mSec" << endl;
    Serial << hh << "  Longest interval = " << LongestTimeMilli << " mSec" << endl;
    LongestTimeMilli = 0;
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
        case VARIABLE:
            Serial << endl;
//            SynthFront.FreqLFO (1, this->InputString.toInt ());
            break;
        default:
            break;
        }

    this->InputString.clear ();
    return (true);
    }

//#######################################################################
void MONITOR_C::InputPrompt (const char* text)
    {
    Serial << "\n\n" << text << " >" << this->InputString;
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
    DisplayMessage.Reset ();
    Serial << endl << "  ********** Reset requested **********";
    Serial << endl << endl;
    Serial << endl << endl;
    Serial << endl << endl;
    delay (200);            // Pause a little bit so that the display message can get out.
    ESP.restart ();
    }

//#######################################################################
bool MONITOR_C::PromptZap (void)
    {
    Serial << " y/N >";
    char in_char = (char)(Serial.read () & 0xFF);
    Serial << in_char << "\n\n";
    switch ( in_char )
        {
        case 'y':
        case 'Y':
            switch ( this->InputMode )
                {
                case ZAP1:
                    Settings.ClearAllSynth();
                    Serial << 9 << endl << "\nCleared Synth settings." << endl;
                    ESP.restart ();
                    break;
                case ZAP2:
                    break;
                default:
                    break;
                }
            break;
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
                    DebugDisp = !DebugDisp;
                    Serial << "  Display debugging " << (( DebugDisp ) ? "Enabled" : "Disabled") << endl;
                    this->Mode (MENU);
                    break;
                case 'd':
                    Settings.SaveDebugFlags ();
                    Serial << "  Saving debug flags" << endl;
                    this->Mode (MENU);
                    break;
                case 'S':
                    this->InputString = Settings.GetSSID ();
                    this->InputPrompt ("  Enter SSID");
                    this->Mode (INSSID);
                    break;
                case 'X':
                    SynthFront.SaveAllSettings ();
                    this->Mode (MENU);
                    break;
                case 'P':
                    this->InputString = Settings.GetPasswd ();
                    this->InputPrompt ("  Enter PWD");
                    this->Mode (INPWD);
                    break;
                case 'F':
                    this->InputPrompt ("  Formatting file system");
                    this->Mode (ZAP2);
                    break;
                case 'C':
                    this->InputPrompt ("  Clearing Synth settings");
                    this->Mode (ZAP1);
                    break;
                case 'q':
                    AnalogDiagEnabled = true;
                    SynthActive       = false;
                    this->Mode (MENU);
                    break;
                case ' ':           // Just move the cursor down a couple of lines
                    Serial << "...\n\n";
                    break;
                case 'z':           // Test function #1
                    this->InputString.clear ();
                    this->InputPrompt ("  Enter value");
                    this->Mode (VARIABLE);
                    break;
                case 'x':           // Test function #2
                    break;
                case 'c':           // Test function #3
                    break;
                case 'v':           // Test function #4
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
    Serial << "\tX   - Save synth oscillator tunning" << endl;
    Serial << "\td   - Save debug flags" << endl;
    Serial << "\n";
    Serial << "\tz   - Test function #1" << endl;
    Serial << "\tx   - Test function #2" << endl;
    Serial << "\tc   - Test function #3" << endl;
    Serial << "\tv   - Test function #4" << endl;
    Serial << "\tS   - SSID" << endl;
    Serial << "\tP   - Password" << endl;
    Serial << "\tF   - Format file system" << endl;
    Serial << "\tC   - Clear Synth settings" << endl;
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
                case VARIABLE:
                    this->TextIn ();
                    break;
                case ZAP1:
                case ZAP2:
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

//#######################################################################
MONITOR_C Monitor;


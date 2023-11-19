//#######################################################################
// Module:     SerialMonitor.cpp
// Descrption: Serial control for setup and debug
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include "config.h"
#include "settings.h"
#include "SerialMonitor.h"
#include "DispFrontEnd.h"

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
    Serial << hr << ":" << mn << ":" << sc;
    }

//#######################################################################
inline static void DumpTitle (void)
    {
    Serial << endl;
    Serial << "\t####  Synth32 -- Disp32  ####" << endl;
    Serial << "\t####  Display Subsystem  ####" << endl;
    }

//#######################################################################
const char* StateDebug (bool d)
    {
    static const char* StateOn  = " <ON> ";
    static const char* StateOff = " <off>";

    if ( d )
        return (StateOn);
    return (StateOff);
    }

//#######################################################################
void MONITOR_C::Mode (SMODE m)
    {
    InputMode = m;
    }

//#######################################################################
void MONITOR_C::DumpStats (void)
    {
    static const char* hh = " ## ";

    DumpTitle ();
    Serial << hh << ESP.getChipModel () << " rev " << ESP.getChipRevision () << ", ";
    Serial << hh << ESP.getChipCores () << " cores.  " << ESP.getCpuFreqMHz () << " MHz" << endl;
    Serial << hh << "SDK " << ESP.getSdkVersion () << endl;
    Serial << hh << "      Flash size = " << ESP.getFlashChipSize () << endl;
    Serial << hh << "     Sketch size = " << ESP.getSketchSize () << endl;
    Serial << hh << "       Free heap = " << ESP.getFreeHeap () << endl;
    Serial << hh << "      Free space = " << ESP.getFreeSketchSpace () << endl << endl;
    Serial << hh << "Average interval = " << AverageDeltaTime << " mSec" << endl;
    Serial << hh << "   Last interval = " << DeltaTime << " mSec" << endl;
    Serial << hh << "     Runing Time = ";
    DispRunTime ();
    Serial << endl;
    }

//#######################################################################
bool MONITOR_C::Save (SMODE m)
    {
    int z;
    switch ( m )
        {
        case INSSID:
            Settings.PutSSID (InputString);
            break;
        case INPWD:
            Settings.PutPasswd (InputString);
            break;
        default:
            break;
        }

    InputString.clear ();
    return (true);
    }

//#######################################################################
void MONITOR_C::InputPrompt (int num, const char* text)
    {
    Serial << num << "\n\n" << text << " >" << InputString;
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
//        Mode (MENU);
        return;
        }

    switch ( InputMode )
        {
        case CMD:
            switch ( s )
                {
                case 's':
                    Serial << endl;
                    DumpStats ();
                    Mode (MENU);
                    break;
                case '1':
                    DebugInterface  = !DebugInterface;
                    Serial << "  INTF debugging " << (( DebugInterface ) ? "Enabled" : "Disabled") << endl;
                    Mode (MENU);
                    break;
                case '2':
                    DebugGraphics  = !DebugGraphics;
                    Serial << "  GRPH debugging " << (( DebugGraphics ) ? "Enabled" : "Disabled") << endl;
                    Mode (MENU);
                    break;
                case 'S':
                    InputString = Settings.GetSSID ();
                    InputPrompt (7, "  Enter SSID");
                    Mode (INSSID);
                    break;
                case 'P':
                    InputString = Settings.GetPasswd ();
                    InputPrompt (8, "  Enter PWD");
                    Mode (INPWD);
                    break;
                case 'C':
                    InputPrompt (9, "  Cleared preferences.");
                    Mode (ZAP);
                    break;
                case ' ':           // Just move the cursor down a couple of lines
                    Serial << "...\n\n";
                    break;
                case 'Z':
                    Serial << "        TEST" << endl;
                    Mode (MENU);
                    break;
                default:
                    Serial << "        ??" << endl;
                    Mode (MENU);
                    break;
                }
            break;
        }
    }

//#######################################################################
void MONITOR_C::Menu (void)
    {
    DumpTitle ();
    Serial << StateDebug (DebugInterface) << "\t1 - Debug interface        " << endl;
    Serial << StateDebug (DebugGraphics)  << "\t2 - Debug graphics         " << endl;
    Serial << "\ts - Dump process Stats" << endl;
    Serial << "\tZ - Test function" << endl;
    Serial << "\tS - SSID" << endl;
    Serial << "\tP - Password" << endl;
    Serial << "\tC - Clear Preferences" << endl;
    Serial << "      F12 - Reset" << endl << endl;
    }

//#######################################################################
void MONITOR_C::TextIn (void)
    {
    char in_char = (char)(Serial.read () & 0xFF);
    switch ( in_char )
        {
        case '\r':              // return (enter)
            if ( Save (InputMode) )
                Mode (MENU);
            break;
        case (char)127:         // backspace
            if ( InputString.length () )
                {
                Serial << (char)8 << ' ' << (char)8;
                InputString.remove (InputString.length () - 1);
                }
            break;
        case (char)27:          // escape for exit with no change
            Mode (MENU);
            break;
        case '\t':              // Tab for special loops
            Save (InputMode);
            break;
        default:                // all other characters go into string
            InputString += in_char;
            Serial << in_char;
            break;
        }
    }

//#######################################################################
MONITOR_C::MONITOR_C (void) : InputMode (MENU), InputString ("")
    {
    }

//#######################################################################
MONITOR_C::~MONITOR_C (void)
    {
    }

//#######################################################################
void MONITOR_C::Begin (void)
    {
    Serial.begin (115200);
    DumpStats ();
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
    if ( InputMode != MENU )
        {
        while ( Serial.available () )
            {
            switch ( InputMode )
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
                        Mode(MENU);
                    break;
                default:
                    break;
                }
            }
        }
    else
        {
        if ( RunTime > MILLI_TO_MICRO (1000) )
            {
            Menu();
            Mode (CMD);
            }
        }
    }

MONITOR_C Monitor;


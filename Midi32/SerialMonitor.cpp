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

using namespace SERIAL_MONITOR;

//#######################################################################
const char* StateDebug (bool d)
    {
    static const char* StateOn  = "\t<ON> ";
    static const char* StateOff = "\t<off>";

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
    Serial << endl << endl;
    Serial << " ## " << ESP.getChipModel () << " rev " << ESP.getChipRevision () << ", ";
    Serial << " ## " << ESP.getChipCores () << " cores.  " << ESP.getCpuFreqMHz () << " MHz" << endl;
    Serial << " ## " << "SDK " << ESP.getSdkVersion () << endl;
    Serial << " ## " << "Flash size = " << ESP.getFlashChipSize () << endl;
    Serial << " ## " << "Sketch size = " << ESP.getSketchSize () << endl;
    Serial << " ## " << "Free heap = " << ESP.getFreeHeap () << endl;
    Serial << " ## " << "Free space = " << ESP.getFreeSketchSpace () << endl;
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
void MONITOR_C::Tuning (int num)
    {
    Serial << endl << "  Tuning oscillator " << num << "...";
    SynthFront.StartTuning (num);
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
                    if ( InputMode == ADIAG )
                        AnalogDiagDevice = ((AnalogDiagDevice / 4) * 4) - 4; // select first channel on previous chip
                    break;
                case 0x42:          // arrow down
                    if ( InputMode == ADIAG )
                        AnalogDiagDevice = ((AnalogDiagDevice / 4) * 4) + 4; // select first channel on next chip
                    break;
                case 0x43:          // arrow right
                    Serial << "<<<<========\n\n";
                    if ( InputMode == ADIAG )
                        AnalogDiagDevice++;                                  // advance to next channel
                    break;
                case 0x44:          // arrow left
                    if ( InputMode == ADIAG )
                        AnalogDiagDevice--;                                  // move to previous channel
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
            printf ("%c", s);
            switch ( s )
                {
                case 's':
                    Serial << endl;
                    DumpStats ();
                    Mode (MENU);
                    break;
                case '1':
                    DebugMidi  = !DebugMidi;
                    Serial << "  MIDI " << (( DebugMidi ) ? "Enabled" : "Disabled") << endl;
                    Mode (MENU);
                    break;
                case '2':
                    DebugDtoA  = !DebugDtoA;
                    Serial << "  D/A " << (( DebugDtoA ) ? "Enabled" : "Disabled") << endl;
                    Mode (MENU);
                    break;
                case '3':
                    DebugOsc   = !DebugOsc;
                    Serial << "  Oscillator " << (( DebugOsc ) ? "Enabled" : "Disabled") << endl;
                    Mode (MENU);
                    break;
                case '4':
                    DebugSynth = !DebugSynth;
                    Serial << "  Synth " << (( DebugSynth ) ? "Enabled" : "Disabled") << endl;
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
                case 't':
                    Tuning (1);
                    Mode (TUNING);
                    break;
                case 'q':
                    AnalogDiagEnabled = true;
                    SynthActive       = false;
                    AnalogDiagDevice  = 0;
                    Serial << endl << endl;
                    Mode (ADIAG);
                    break;
                case ' ':           // Just move the cursor down a couple of lines
                    Serial << "...\n\n";
                    break;
                case 'Z':
                    SynthFront.SendControl ();
                    break;
                default:
                    Serial << "       ??" << endl;
                    Mode (MENU);
                    break;
                }
            break;

        case TUNING:
            switch ( s )
                {
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                    Tuning (s - '0');
                    break;
                default:
                    break;
                }
        }
    }

//#######################################################################
void MONITOR_C::Menu (void)
    {
    Serial << endl << endl;
    Serial << "    1 - Debug MIDI interface   " << StateDebug (DebugMidi) << endl;
    Serial << "    2 - Debug D to A interface " << StateDebug (DebugDtoA) << endl;
    Serial << "    3 - Debug Oscillators      " << StateDebug (DebugOsc) << endl;
    Serial << "    4 - Debug Synth            " << StateDebug (DebugSynth) << endl;
    Serial << endl;
    Serial << "    q - D/A Diagnostic mode" << endl;
    Serial << "    s - Dump process Stats" << endl;
    Serial << "    t - Tuning mode" << endl;
    Serial << "    Z - Test funciton" << endl;
    Serial << "    S - SSID" << endl;
    Serial << "    P - Password" << endl;
    Serial << "    C - Clear Preferences" << endl;
    Serial << "  F12 - Reset" << endl << endl;
    Serial << "Select >";
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
                case TUNING:
                case ADIAG:
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
        Menu ();
        Mode (CMD);
        }
    }

MONITOR_C Monitor;


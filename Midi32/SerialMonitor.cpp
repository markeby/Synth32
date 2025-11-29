//#######################################################################
// Module:     SerialMonitor.ino
// Descrption: Serial control for setup and debug
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include <chip-debug-report.h>
#include "I2Cmessages.h"
#include "settings.h"
#include "SerialMonitor.h"
#include "FrontEnd.h"
#include "UpdateOTA.h"

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
    Serial << hh << "total sketch code = " << ESP.getFreeSketchSpace () << endl;
    Serial << hh << "      sketch size = " << Settings.GetSketchSize () << endl;
    Serial << hh << "      sketch used = " << (float)((ESP.getSketchSize () * 100) / (float)ESP.getFreeSketchSpace ()) << "%" << endl;
    Serial << hh << "    previous size = " << Settings.GetSketchSizePrev () << endl;
    Serial << hh << "      size change = " << (Settings.GetSketchSize () - Settings.GetSketchSizePrev ()) << endl << endl;

    Serial << hh << "        Heap size = " << ESP.getHeapSize ()     << endl;
    Serial << hh << "larget heap block = " << ESP.getMaxAllocHeap () << endl;
    Serial << hh << "lowest heap space = " << ESP.getMinFreeHeap ()  << endl;
    Serial << hh << "  free heap space = " << ESP.getFreeHeap ()     << endl << endl;

    Serial << hh << "       Stack size = " << getArduinoLoopTaskStackSize ()    << endl;
    Serial << hh << " Free stack space = " << uxTaskGetStackHighWaterMark (NULL) << endl << endl;

    uint64_t zl = ESP.getEfuseMac ();
    Serial << hh << "       Update MAC = " <<  _WIDTHZ (_HEX ( zl        & 0xFF), 2) <<
                                      ":" <<  _WIDTHZ (_HEX ((zl >> 8)  & 0xFF), 2) <<
                                      ":" <<  _WIDTHZ (_HEX ((zl >> 16) & 0xFF), 2) <<
                                      ":" <<  _WIDTHZ (_HEX ((zl >> 24) & 0xFF), 2) <<
                                      ":" <<  _WIDTHZ (_HEX ((zl >> 32) & 0xFF), 2) << endl;
    Serial << hh << "       Update URL = " << UpdateOTA.GetIP() << endl << endl;
    Serial << hh << "      Runing Time = ";
    DispRunTime ();
    Serial << hh << "    Last interval = " << DeltaTimeMilli << " mSec" << endl;
    Serial << hh << " Average interval = " << DeltaTimeMilliAvg << " mSec" << endl;
    Serial << hh << " Longest interval = " << LongestTimeMilli << " mSec" << endl;
    LongestTimeMilli = 0;
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
        case VARIABLE:
            Serial << endl;
            z = InputString.toInt ();
            break;
        default:
            break;
        }

    InputString.clear ();
    return (true);
    }

//#######################################################################
void MONITOR_C::InputPrompt (const char* text)
    {
    Serial << "\n\n" << text << " >" << InputString;
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
            switch ( InputMode )
                {
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
                    DebugSeq  = !DebugSeq;
                    Serial << "  MIDI sequence debugging " << (( DebugSeq ) ? "Enabled" : "Disabled") << endl;
                    Mode (MENU);
                    break;
                case '2':
                    DebugMidi  = !DebugMidi;
                    Serial << "  MIDI debugging " << (( DebugMidi ) ? "Enabled" : "Disabled") << endl;
                    Mode (MENU);
                    break;
                case '3':
                    DebugI2C  = !DebugI2C;
                    Serial << "  I2C debugging " << (( DebugI2C ) ? "Enabled" : "Disabled") << endl;
                    Mode (MENU);
                    break;
                case '4':
                    DebugSynth = !DebugSynth;
                    Serial << "  Synth debugging " << (( DebugSynth ) ? "Enabled" : "Disabled") << endl;
                    Mode (MENU);
                    break;
                case '5':
                    DebugDisp = !DebugDisp;
                    Serial << "  Display debugging " << (( DebugDisp ) ? "Enabled" : "Disabled") << endl;
                    Mode (MENU);
                    break;
                case 'd':
                    Settings.SaveDebugFlags ();
                    Serial << "  Saving debug flags" << endl;
                    Mode (MENU);
                    break;
                case 'S':
                    InputString = Settings.GetSSID ();
                    InputPrompt ("  Enter SSID");
                    Mode (INSSID);
                    break;
                case 'P':
                    InputString = Settings.GetPasswd ();
                    InputPrompt ("  Enter PWD");
                    Mode (INPWD);
                    break;
                case 'F':
                    SynthConfig.DumpFiles ();
                    Mode (MENU);
                    break;
                case ' ':           // Just move the cursor down a couple of lines
                    Serial << "...\n\n";
                    break;
                case 'z':           // Test function #1
                    {
                    static short z1 = 0;

                    z1 = z1 ^ 1;
                    I2cDevices.DigitalOut(208, z1);
                    I2cDevices.UpdateDigital();
                    DbgD(z1);
                    }
                    break;
                case 'x':           // Test function #2
                    {
                    static short x1 = 0;

                    x1 = x1 ^ 1;
                    I2cDevices.DigitalOut(209, x1);
                    I2cDevices.UpdateDigital();
                    DbgD(x1);
                    }
                    break;
                case 'c':           // Test function #3
                    break;
                case 'v':           // Test function #4
                    break;
                default:
                    Serial << "       ??" << endl;
                    Mode (MENU);
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
    if (  SetTuning )
        Serial << "\t******     Tuning mode      ******" << endl;
    Serial << StateDebug (DebugSeq)   << "\t1   - Debug MIDI Sequencer" << endl;
    Serial << StateDebug (DebugMidi)  << "\t2   - Debug MIDI devices" << endl;
    Serial << StateDebug (DebugI2C)   << "\t3   - Debug I2C interface" << endl;
    Serial << StateDebug (DebugSynth) << "\t4   - Debug Synth" << endl;
    Serial << StateDebug (DebugDisp ) << "\t5   - Debug Display Interface" << endl;
    Serial << "\td   - Save debug flags" << endl;
    Serial << "\ts   - Dump process Stats" << endl;
    Serial << "\tF   - Dump json files" << endl;
    Serial << endl;
//    Serial << "\tz   - Test function #1" << endl;
//    Serial << "\tx   - Test function #2" << endl;
//    Serial << "\tc   - Test function #3" << endl;
//    Serial << "\tv   - Test function #4" << endl;
    Serial << endl;
    Serial << "\tS   - SSID" << endl;
    Serial << "\tP   - Password" << endl;
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
MONITOR_C::MONITOR_C (void)
    {
    Serial.begin (115200);
    InputString = "";
    InputMode   = MENU;
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
                    MenuSel ();
                    break;
                case INSSID:
                case INPWD:
                case VARIABLE:
                    TextIn ();
                    break;
                case CLR_CONFIG:
                    if ( PromptZap () )
                        Mode (MENU);
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


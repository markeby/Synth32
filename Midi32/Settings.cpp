//#######################################################################
// Module:     Settings.cpp
// Descrption: Load and save settings for system and synth
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include <Preferences.h>
#include "Settings.h"
#include "FrontEnd.h"

//#######################################################################
Preferences Prefs;

//#######################################################################
// Preference keys
static const char* sysKeySpace    = "SysP";     // WIFI and debug data directory
static const char* sysKeySSID     = "SSID";
static const char* sysKeyPSWD     = "PASSD";
static const char* sysKeyDBG      = "DBG";
static const char* sysKeySketch   = "SkSize";

static const char* synthTuning    = "SynthP";   // Tuning directory
static const char* synthKeyBank   = "BANK";     // Keyboard bank prefix "BANKx"
static const char* synthKeyBender = "BEND";     // LFO/Bender offset "BENDx"

static const char* synthConfig    = "SynthC";   // Configureation directory

//#######################################################################
void SETTINGS_C::ClearAllSys (void)
    {
    Prefs.begin (sysKeySpace, false);
    Prefs.clear ();
    Prefs.end   ();
    }

//#######################################################################
void SETTINGS_C::PutSSID (String& str)
    {
    Prefs.begin     (sysKeySpace, false);
    Prefs.putString (sysKeySSID, str);
    s_SSID = str;
    Prefs.end       ();
    }

//#######################################################################
void SETTINGS_C::PutPasswd (String& str)
    {
    Prefs.begin     (sysKeySpace, false);
    Prefs.putString (sysKeyPSWD, str);
    s_PSWD = str;
    Prefs.end       ();
    }

//#######################################################################
void SETTINGS_C::PutSystemParam (const char* name, int param)
    {
    Prefs.begin  (sysKeySpace, false);
    Prefs.putInt (name, param);
    Prefs.end    ();
    }

//#######################################################################
int SETTINGS_C::GetSystemParam (const char* name)
    {
    Prefs.begin (sysKeySpace, false);
    int z = Prefs.getInt (name, 0);
    Prefs.end ();
    return (z);
    }

//#######################################################################
void SETTINGS_C::PutDebugSwitch (uint8_t num, bool state)
    {
    char buf[8];

    sprintf       (buf, "%s%d", sysKeyDBG, num);
    Prefs.begin   (sysKeySpace, false);
    Prefs.putBool ((const char *)buf, state);
    Prefs.end     ();
    }

//#######################################################################
bool SETTINGS_C::GetDebugSwitch (uint8_t num)
    {
    char buf[8];

    sprintf (buf, "%s%d", sysKeyDBG, num);
    Prefs.begin (sysKeySpace, false);
    bool zb = Prefs.getBool ((const char *)buf);
    Prefs.end ();
    return (zb);
    }

//#######################################################################
void SETTINGS_C::SaveDebugFlags ()
    {
    PutDebugSwitch (0, DebugSeq);
    PutDebugSwitch (1, DebugMidi);
    PutDebugSwitch (2, DebugI2C);
    PutDebugSwitch (3, DebugSynth);
    PutDebugSwitch (4, DebugDisp);
    }

//#######################################################################
void SETTINGS_C::RestoreDebugFlags ()
    {
    DebugSeq    = GetDebugSwitch (0);
    DebugMidi   = GetDebugSwitch (1);
    DebugI2C    = GetDebugSwitch (2);
    DebugSynth  = GetDebugSwitch (3);
    DebugDisp   = GetDebugSwitch (4);
    }

//#######################################################################
SETTINGS_C::SETTINGS_C (void)
    {
    }

//#######################################################################
void SETTINGS_C::Begin (void)
    {
    Prefs.begin (sysKeySpace, false);
        {
        s_SSID = Prefs.getString (sysKeySSID);
        s_PSWD = Prefs.getString (sysKeyPSWD);
        }
    Prefs.end ();
    Settings.RestoreDebugFlags ();
    SketchSizePrev = GetSystemParam (sysKeySketch);
    SketchSize = ESP.getSketchSize ();
    PutSystemParam (sysKeySketch, SketchSize);
    }

//#######################################################################
//#######################################################################
void SETTINGS_C::ClearTuning (void)
    {
    Prefs.begin (synthTuning, false);
    Prefs.clear ();
    Prefs.end();
    }

//#######################################################################
void SETTINGS_C::ClearConfig(void)
    {
    Prefs.begin (synthConfig, false);
    Prefs.clear ();
    Prefs.end ();
    }

//#######################################################################
bool SETTINGS_C::GetOscBank (uint8_t num, uint16_t* pbank)
    {
    char buf[8];
    bool exist = true;

    sprintf     (buf, "%s%d", synthKeyBank, num);
    Prefs.begin (synthTuning, false);
    if (  Prefs.isKey ((const char *)buf) )
        {
        bool zb = Prefs.getBytes((const char *)buf, pbank, KEYS_FULL * sizeof(uint16_t));
        exist = false;
        }
    Prefs.end ();
    return (exist);
    }

//#######################################################################
void SETTINGS_C::PutOscBank (uint8_t num, uint16_t* pbank)
    {
    char buf[8];

    sprintf        (buf, "%s%d", synthKeyBank, num);
    Prefs.begin    (synthTuning, false);
    Prefs.putBytes ((const char *)buf, pbank, KEYS_FULL * sizeof (uint16_t));
    Prefs.end      ();
    }

//#######################################################################
short SETTINGS_C::GetOffsetLFO (uint8_t num)
    {
    char buf[8];

    sprintf     (buf, "%s%d", synthKeyBender, num);
    Prefs.begin (synthTuning, false);
    short z     = Prefs.getShort (buf);
    Prefs.end   ();
    return      (z);
    }

//#######################################################################
void SETTINGS_C::SetOffsetLFO (uint8_t num, short offset)
    {
    char buf[8];

    sprintf        (buf, "%s%d", synthKeyBender, num);
    Prefs.begin    (synthTuning, false);
    Prefs.putShort (buf, offset);
    Prefs.end      ();
    }

//#######################################################################
bool SETTINGS_C::GetConfig (const char* name, void* ptr, size_t len)
    {
    size_t rtn = 0;

    Prefs.begin (synthConfig, false);
    if ( Prefs.isKey (name) )
        rtn = Prefs.getBytes (name, ptr, len);
    Prefs.end();
    if ( rtn == len )
        return (false);
    return (true);
    }

//#######################################################################
void SETTINGS_C::PutConfig (const char* name, const void* ptr, size_t len)
    {
    bool zbl;
    size_t rtn;

    Prefs.begin (synthConfig, false);
    if ( Prefs.isKey (name) )
        zbl = Prefs.remove (name);
    rtn = Prefs.putBytes (name, ptr, len);
    Prefs.end ();
    }

//#######################################################################
SETTINGS_C Settings;        // Settings storage and retrieval


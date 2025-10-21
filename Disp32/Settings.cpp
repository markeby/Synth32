//#######################################################################
// Module:     Settings.cpp
// Descrption: Load and save settings
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include <Preferences.h>
#include "Settings.h"

//#######################################################################
Preferences Prefs;
static const char* sysKeySpace  = "SysP";
static const char* sysKeySSID   = "SSID";
static const char* sysKeyPSWD   = "PASSD";
static const char* sysKeyDBG    = "DBG";
static const char* sysKeySketch   = "SkSize";

//#######################################################################
void SETTINGS_C::ClearAll (void)
    {
    Prefs.begin (sysKeySpace, false);
    Prefs.clear ();
    Prefs.end ();
    }

//#######################################################################
void SETTINGS_C::PutSSID (String& str)
    {
    Prefs.begin (sysKeySpace, false);
    Prefs.putString (sysKeySSID, str);
    s_SSID = str;
    Prefs.end ();
    }

//#######################################################################
void SETTINGS_C::PutPasswd (String& str)
    {
    Prefs.begin (sysKeySpace, false);
    Prefs.putString (sysKeyPSWD, str);
    s_PSWD = str;
    Prefs.end ();
    }

//#######################################################################
void SETTINGS_C::PutDebugSwitch (uint8_t num, bool state)
    {
    char buf[8];

    sprintf (buf, "%s%d", sysKeyDBG, num);
    Prefs.begin(sysKeySpace, false);
    Prefs.putBool ((const char *)buf, state);
    Prefs.end ();
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
    PutDebugSwitch (0, DebugInterface);
    PutDebugSwitch (1, DebugGraphics);
    }

//#######################################################################
void SETTINGS_C::RestoreDebugFlags ()
    {
    DebugInterface = GetDebugSwitch (0);
    DebugGraphics  = GetDebugSwitch (1);
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
SETTINGS_C Settings;        // System settings


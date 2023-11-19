//#######################################################################
// Module:     Settings.cpp
// Descrption: Load and save setting and message settings
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>
#include <Preferences.h>
#include "Settings.h"

//#######################################################################
Preferences Prefs;
const char* PrefNameSpace   = "Synth";
const char* PrefSSID        = "SSID";
const char* PrefPSWD        = "PASSD";
const char* prefTunning     = "TUNE";

//#######################################################################
void SETTINGS_RAM_C::ClearAll (void)
    {
    Prefs.begin (PrefNameSpace, false);
    Prefs.clear ();
    Prefs.end ();
    }

//#######################################################################
void SETTINGS_RAM_C::PutSSID (String& str)
    {
    Prefs.begin (PrefNameSpace, false);
    Prefs.putString (PrefSSID, str);
    s_SSID = str;
    Prefs.end ();
    }

//#######################################################################
void SETTINGS_RAM_C::PutPasswd (String& str)
    {
    Prefs.begin (PrefNameSpace, false);
    Prefs.putString (PrefPSWD, str);
    s_PSWD = str;
    Prefs.end ();
    }

//#######################################################################
const char* SETTINGS_RAM_C::GetSSID (void)
    {
    return (s_SSID.c_str ());
    }

//#######################################################################
const char* SETTINGS_RAM_C::GetPasswd (void)
    {
    return (s_PSWD.c_str ());
    }

//#######################################################################
uint16_t* SETTINGS_RAM_C::GetVCO (uint8_t num)
    {
    }

//#######################################################################
void SETTINGS_RAM_C::PutVCO (uint8_t num, uint16_t* pvco)
    {
    }

//#######################################################################
SETTINGS_RAM_C::SETTINGS_RAM_C (void)
    {
    }

//#######################################################################
void SETTINGS_RAM_C::Begin (void)
    {
    Prefs.begin (PrefNameSpace, false);
        {
        s_SSID       = Prefs.getString (PrefSSID);
        s_PSWD       = Prefs.getString (PrefPSWD);
        }

    Prefs.end ();
    }

//#######################################################################
SETTINGS_RAM_C Settings;        // System settings


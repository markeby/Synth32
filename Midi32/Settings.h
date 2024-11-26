//#######################################################################
// Module:     Settings.h
// Descrption: Load and save setting and message settings
// Creator:    markeby
// Date:       8/1/2022
//#######################################################################
#pragma once

#include <Preferences.h>
#include "config.h"

//#######################################################################
class SETTINGS_C
    {
private:
    String    s_SSID;
    String    s_PSWD;
    bool      DebugFlags;

    bool        GetDebugSwitch      (byte num);
    void        PutDebugSwitch      (byte num, bool state);

public:
                SETTINGS_C  (void);
    void        Begin           (void);

    //----------------------------------------
    // SYS model storage
    //----------------------------------------
    void        ClearAllSys         (void);
    void        PutSSID             (String& str);
    void        PutPasswd           (String& str);
    void        SaveDebugFlags      (void);
    void        RestoreDebugFlags   (void);

    //------------------------------------
    inline const char* GetPasswd (void)
        {
        return (s_PSWD.c_str ());
        }

    //------------------------------------
    inline const char* GetSSID (void)
        {
        return (s_SSID.c_str ());
        }

    //----------------------------------------
    // Synth model storage
    //----------------------------------------
    void        ClearAllSynth   (void);
    bool        GetOscBank      (byte num, uint16_t* pbank);
    void        PutOscBank      (byte num, uint16_t* pbank);
    };

//#######################################################################
extern SETTINGS_C Settings;        // System settings


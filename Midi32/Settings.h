//#######################################################################
// Module:     Settings.h
// Descrption: Load and save setting and message settings
// Creator:    markeby
// Date:       8/1/2022
//#######################################################################
#pragma once

#include <Preferences.h>
#include "Config.h"

//#######################################################################
class SETTINGS_C
    {
private:
    String  s_SSID;
    String  s_PSWD;
    bool    DebugFlags;

    bool    GetDebugSwitch      (uint8_t num);
    void    PutDebugSwitch      (uint8_t num, bool state);

public:
            SETTINGS_C  (void);
    void    Begin           (void);

    //----------------------------------------
    // SYS model storage
    //----------------------------------------
    void    ClearAllSys         (void);
    void    PutSSID             (String& str);
    void    PutPasswd           (String& str);
    void    SaveDebugFlags      (void);
    void    RestoreDebugFlags   (void);

    //------------------------------------
    inline const char* GetPasswd (void)
        { return (s_PSWD.c_str ()); }

    //------------------------------------
    inline const char* GetSSID (void)
        { return (s_SSID.c_str ()); }

    //----------------------------------------
    // Synth tuning storage
    //----------------------------------------
    void     ClearAllSynth       (void);
    bool     GetOscBank          (uint8_t num, uint16_t* pbank);
    void     PutOscBank          (uint8_t num, uint16_t* pbank);
    short    GetBenderOffset     (void);
    void     PutBenderOffset     (short offset);

    //----------------------------------------
    // Synth configure storage
    //----------------------------------------
    void     GetDefaultConfig    (int num, void* ptr, size_t len);
    void     PutDefaultConfig    (int num, const void* ptr, size_t len);
    };

//#######################################################################
extern SETTINGS_C Settings;        // System settings


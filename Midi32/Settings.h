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
    String   s_SSID;
    String   s_PSWD;
    int      SketchSize;
    int      SketchSizePrev;

    bool    GetDebugSwitch      (uint8_t num);
    void    PutDebugSwitch      (uint8_t num, bool state);
    void    PutSystemParam      (const char* name, int param);
    int     GetSystemParam      (const char* name);

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
    const char* GetPasswd (void)
        { return (s_PSWD.c_str ()); }

    //------------------------------------
    const char* GetSSID (void)
        { return (s_SSID.c_str ()); }

    //------------------------------------
    int GetSketchSize (void)
        { return (SketchSize); }

    //------------------------------------
    int GetSketchSizePrev (void)
        { return (SketchSizePrev); }

    //----------------------------------------
    // Synth tuning storage
    //----------------------------------------
    void     ClearTuning         (void);
    void     ClearConfig         (void);
    bool     GetOscBank          (uint8_t num, uint16_t* pbank);
    void     PutOscBank          (uint8_t num, uint16_t* pbank);
    short    GetOffsetLFO        (uint8_t num);
    void     SetOffsetLFO        (uint8_t num, short offset);

    //----------------------------------------
    // Synth configure storage
    //----------------------------------------
    bool     GetConfig    (const char* name, void* ptr, size_t len);
    void     PutConfig    (const char* name, const void* ptr, size_t len);
    };

//#######################################################################
extern SETTINGS_C Settings;        // System settings


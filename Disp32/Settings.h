//#######################################################################
// Module:     Settings.h.hpp
// Descrption: Load and save setting and message settings
// Creator:    markeby
// Date:       8/1/2022
//#######################################################################
#pragma once

#include <Preferences.h>
#include "config.h"

//#######################################################################
class   SETTINGS_C
    {
private:
    String    s_SSID;
    String    s_PSWD;
    int      SketchSize;
    int      SketchSizePrev;

    bool    GetDebugSwitch      (uint8_t num);
    void    PutDebugSwitch      (uint8_t num, bool state);
    void    PutSystemParam      (const char* name, int param);
    int     GetSystemParam      (const char* name);

public:
    SETTINGS_C  (void);
    void    ClearAll    (void);
    void    PutSSID     (String& str);
    void    PutPasswd   (String& str);
    void    Begin       (void);

    void    SaveDebugFlags      (void);
    void    RestoreDebugFlags   (void);

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

     //------------------------------------
    int GetSketchSize (void)
        { return (SketchSize); }

    //------------------------------------
    int GetSketchSizePrev (void)
        { return (SketchSizePrev); }

   };

//#######################################################################
extern SETTINGS_C Settings;        // System settings


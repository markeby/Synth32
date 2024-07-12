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
class   SETTINGS_RAM_C
    {
private:
    String    s_SSID;
    String    s_PSWD;

public:
    SETTINGS_RAM_C          (void);
    void        ClearAll    (void);
    void        PutSSID     (String& str);
    void        PutPasswd   (String& str);
    const char* GetSSID     (void);
    const char* GetPasswd   (void);
    void        Begin       (void);
    };

//#######################################################################
extern SETTINGS_RAM_C Settings;        // System settings


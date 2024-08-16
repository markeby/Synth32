//#######################################################################
// Module:     Files.h
// Descrption: SIFFS file management
// Creator:    markeby
// Date:       8/16/2024
//#######################################################################
#pragma once

#include "config.h"
#include "FS.h"
#include "SPIFFS.h"

//#######################################################################
class FILES_C
    {
private:
    bool    Ready;

public:
         FILES_C    (void);
    void Begin      (void);
    void Format     (void);
    };

//#######################################################################
extern FILES_C Files;


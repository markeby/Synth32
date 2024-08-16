//#######################################################################
// Module:     Files.cpp
// Descrption: SIFFS file management
// Creator:    markeby
// Date:       8/16/2024
//#######################################################################
#include <Arduino.h>
#include "config.h"
#include "Files.h"

//#######################################################################
static const char* filesReady = "\t>>> File system (SPIFFS) ready.\n";

//#######################################################################
FILES_C::FILES_C () : Ready(false)
    {
    }

//#######################################################################
void FILES_C::Begin ()
    {
    if ( !SPIFFS.begin (false) )
        return;
    printf (filesReady);
    Ready = true;
    }

//#######################################################################
void FILES_C::Format ()
    {
    printf ("\t>>> Formatting file system...\n");
    if ( !SPIFFS.begin (true) )
        return;
    printf (filesReady);
    Ready = true;
    }

//#######################################################################
FILES_C Files;


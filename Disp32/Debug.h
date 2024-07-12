//#######################################################################
// Module:     Debug.h
// Descrption: Varialble length debug output
// Creator:    markeby
// Date:       3/17/2024
//#######################################################################
#pragma once

#define DEBUG_NO_INDEX      255

const String vFormat (const char *const zcFormat, ...);
const String vsFormat (const char *const zcFormat, va_list args);

void DebugMsg  (const char* label, int index, const char *const fmt, ...);
void DebugMsgN (const char* label, int index, String name,  const char *const fmt, ...);
void DebugMsgF (const char* label, int index, String name, char* flag, const char *const fmt, ...);


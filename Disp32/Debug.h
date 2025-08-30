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

void DebugMsg (const char* label, uint8_t index, const char *const fmt, ...);
void DebugMsgN (const char* label, uint8_t index, String name,  const char *const fmt, ...);
void DebugMsgF (const char* label, uint8_t index, String name, char* flag, const char *const fmt, ...);

void BootDebug (void);

#define PAUSE   {printf("--- %s:%d\n",__FILE__,__LINE__);while(!Serial.available ()) continue;char s=Serial.read();}
#define DbgD(d) {printf("==> %s:%d %s = %d\n",__FILE__,__LINE__, #d, d);}
#define DbgX(x) {printf("==> %s:%d %s = 0x%X\n",__FILE__,__LINE__, #x, x);}
#define DbgF(f) {printf("==> %s:%d %s = %f\n",__FILE__,__LINE__, #f, f);}
#define DbgS(s) {printf("==> %s:%d %s = %s\n",__FILE__,__LINE__, #s, s);}
#define DbgN    {printf("\n");}


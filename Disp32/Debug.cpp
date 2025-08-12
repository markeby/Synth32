//#######################################################################
// Module:     Debug.cpp
// Descrption: Varialble length debug output
// Creator:    markeby
// Date:       3/17/2023
//#######################################################################
//#include <Arduino.h>
#include <Streaming.h>
#include <vector>
#include "Debug.h"
using namespace std;

//#######################################################################
const String vFormat (const char *const zcFormat, ...)
    {
    // initialize use of the variable argument array
    va_list vaArgs;
    va_start (vaArgs, zcFormat);

    // reliably acquire the size  from a copy of the variable argument array
    // and a functionally reliable call to mock the formatting
    va_list vaArgsCopy;
    va_copy (vaArgsCopy, vaArgs);
    const int iLen = vsnprintf (NULL, 0, zcFormat, vaArgsCopy);
    va_end (vaArgsCopy);

    // return a formatted string without risking memory mismanagement
    // and without assuming any compiler or platform specific behavior
    vector<char> zc (iLen + 1);
    vsnprintf (zc.data (), zc.size (), zcFormat, vaArgs);
    va_end (vaArgs);
    return (String (zc.data (), iLen));
    }

//#######################################################################
const String vsFormat (const char *const zcFormat, va_list args)
    {
    // reliably acquire the size  from a copy of the variable argument array
    // and a functionally reliable call to mock the formatting
    va_list vaArgsCopy;
    va_copy (vaArgsCopy, args);
    const int iLen = vsnprintf (NULL, 0, zcFormat, vaArgsCopy);
    va_end (vaArgsCopy);

    // return a formatted string without risking memory mismanagement
    // and without assuming any compiler or platform specific behavior
    vector<char> zc (iLen + 1);
    vsnprintf (zc.data (), zc.size (), zcFormat, args);
    va_end (args);
    return (String (zc.data (), iLen));
    }

//#######################################################################
void DebugMsg (const char* label, uint8_t index, const char *const fmt, ...)
    {
    va_list ap;
    va_start (ap, fmt);
    String str;

    if ( index == DEBUG_NO_INDEX )
        str = "[" + String(label) + "] ";
    else
        str = "[" + String(label) + "-" + String(index) + "] ";
    str += vsFormat (fmt, ap);
    Serial << str << endl;
    }

//#######################################################################
void DebugMsgN (const char* label, uint8_t index, String name,  const char *const fmt, ...)
    {
    va_list ap;
    va_start (ap, fmt);

    String str = "[" + String(label) + "-" + String(index) + "]{";
    str += name + "} " + vsFormat (fmt, ap);
    Serial << str << endl;
    }

//#######################################################################
void DebugMsgF (const char* label, uint8_t index, String name, char* flag, const char *const fmt, ...)
    {
    va_list ap;
    va_start (ap, fmt);

    String str = "[" + String(label) + "-" + String(index) + "]{" + name;
    str += "} - " + String(flag) + " - " + vsFormat (fmt, ap);
    Serial << str << endl;
    }

//#######################################################################
static const char* esp_rst_unknown      = "Reset reason not determined";
static const char* esp_rst_poweron      = "Reset power-on";
static const char* esp_rst_sw           = "Software reset esp_restart";
static const char* esp_rst_panic        = "Software reset exception/panic";
static const char* esp_rst_int_wdt      = "Reset interrupt watchdog";
static const char* esp_rst_task_wdt     = "Reset task watchdog";
static const char* esp_rst_wdt          = "Reset other watchdogs";
static const char* esp_rst_deepsleep    = "Reset exiting deep sleep";
static const char* esp_rst_brownout     = "Brownout reset";
static const char* esp_rst_sdio         = "Reset over SDIO";
static const char* esp_rst_not_known    = "Reset uknown reason";

//#######################################################################
void BootDebug ()
    {
    const char* str;

    esp_reset_reason_t  reason = esp_reset_reason ();
    switch ( reason )
        {
        case ESP_RST_UNKNOWN:
            str = esp_rst_unknown;
            break;

        case ESP_RST_POWERON:
            delay (500);
            str = esp_rst_poweron;
            break;

        case ESP_RST_SW:
            str = esp_rst_sw;
            break;

        case ESP_RST_PANIC:
            str = esp_rst_panic;
            break;

        case ESP_RST_INT_WDT:
            str = esp_rst_int_wdt;
            break;

        case ESP_RST_TASK_WDT:
            str = esp_rst_task_wdt;
            break;

        case ESP_RST_WDT:
            str = esp_rst_wdt;
            break;

        case ESP_RST_DEEPSLEEP:
            str = esp_rst_deepsleep;
            break;

        case ESP_RST_BROWNOUT:
            str = esp_rst_brownout;
            break;

        case ESP_RST_SDIO:
            str = esp_rst_sdio;
            break;

        default:
            printf ("%s $d\n", esp_rst_not_known, (uint32_t)reason);
            return;
        }
    printf ("\n\t\t********** %s **********\n\n", str);
    }



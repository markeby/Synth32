//#######################################################################
// Module:     Disp32.cpp
// Descrption: Display for Synt32
// Creator:    markeby
// Date:       7/11/2024
//#######################################################################
#include <Arduino.h>

#include "config.h"
#include "Settings.h"
#include "SerialMonitor.h"
#include "Graphics.h"
#include "../Common/DispMessages.h"
#include "ClientI2C.h"

//#######################################################################

float      DeltaTime           = 0;
int        DeltaMicro          = 0;             // micro second interval.
float      AverageDeltaTime    = 0;
uint64_t   RunTime             = 0;

bool       DebugInterface      = false;
bool       DebugGraphics       = false;

lv_style_t  GlobalKeyStyle;

//#######################################################################
inline void TimeDelta (void)
    {
    static uint64_t strt = 0;
    static uint64_t loop_cnt_10hz = 0;
    static uint64_t icount = 0;

    uint64_t current =  micros ();
    DeltaMicro = (int)(current - strt);
    strt = current;
    DeltaTime = MICRO_TO_MILLI (DeltaMicro);

    RunTime       += DeltaMicro;
    loop_cnt_10hz += DeltaMicro;
    icount++;

    if ( loop_cnt_10hz >= MILLI_TO_MICRO (100)  )
        {
        AverageDeltaTime = MICRO_TO_MILLI (loop_cnt_10hz / icount);
        loop_cnt_10hz = 0;
        icount = 0;
        }
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
    delay (1000);
    switch ( reason )
        {
        case ESP_RST_UNKNOWN:
            str = esp_rst_unknown;
            break;

        case ESP_RST_POWERON:
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


//#######################################################################
//#######################################################################
void setup (void)
    {
    bool fault = false;
    Serial.begin (115200);
    BootDebug ();

    printf ("\t>>> Start Settings config...\n");
    Settings.Begin ();

    printf ("\t>>> Startup Graphics...\n");
    Graphics.Begin ();

    printf ("\t>>> Startup I2C inputs...\n");
    Client.Begin (DISPLAY_I2C_ADDRESS);
    delay (1500);   // Give time for the graphics subsystem threads to start

    printf ("\t>>> System startup complete.\n\n");

    pinMode (MIDI_TRIGGER_PORT, OUTPUT);
    }

//#######################################################################
//#######################################################################
void loop (void)
    {
    static bool first = true;

    TimeDelta ();

    Client.Process ();

    Monitor.Loop ();
    if ( first )
        {
        Client.TriggerInitialMsgs ();
        first = false;
        }
    }


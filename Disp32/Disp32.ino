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
#include "DispFrontEnd.h"
#include "Graphics.h"
#include "WebOTA.h"

//#######################################################################
DISP_FRONT_END_C  DispFront;

float      DeltaTime           = 0;
int        DeltaMicro          = 0;             // micro second interval.
float      AverageDeltaTime    = 0;
uint64_t   RunTime             = 0;

bool       DebugInterface      = true;
bool       DebugGraphics       = true;

// this is used to add a task to core 0
//TaskHandle_t  Core0TaskHnd;

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
//#######################################################################
void setup (void)
    {
    bool fault = false;
    Serial.begin (115200);

    printf ("\t>>> Start Settings config...\n");
    Settings.Begin ();

    printf ("\t>>> Startup OTA...\n");
    UpdateOTA.Setup (Settings.GetSSID (), Settings.GetPasswd ());

    printf ("\t>>> Startup Graphics...\n");
    Graphics.Begin ();
    DispFront.Begin ();


 //   xTaskCreatePinnedToCore (Core0Task, "Core0Task", 8000, NULL, 999, &Core0TaskHnd, 0);

    delay (1500);   // Give time for the Wifi to connect
    printf ("\t>>> System startup complete.\n\n");
    }

//#######################################################################
void Core0TaskSetup (void)
    {
    // init your stuff for core0 here
    }

//#######################################################################
void Core0TaskLoop (void)
    {
    // put your loop stuff for core0 here
    }

//#######################################################################
void Core0Task (void *parameter)
    {
    Core0TaskSetup ();

    while ( true )
        {
        Core0TaskLoop ();

        /* this seems necessary to trigger the watchdog */
        delay (1);
        yield ();
        }
    }


//#######################################################################
//#######################################################################
void loop (void)
    {
    TimeDelta ();

    if ( !UpdateOTA.WiFiStatus () )
        {
//        delay (2000);
        if ( UpdateOTA.WaitWiFi () )
            {
            UpdateOTA.Begin ();
            }
        }

    DispFront.Loop ();
    Monitor.Loop ();
    UpdateOTA.Loop ();
    }


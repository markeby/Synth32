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
//#######################################################################
void setup (void)
    {
 // Pause here so that init of serial port in the monitor class can complete
    BootDebug ();
    printf ("\n\t>>> Startup of Midi32 %s %s\n", __DATE__, __TIME__);
    printf ("\t>>> Start Settings config...\n");


    printf ("\t>>> Start Settings config...\n");
    Settings.Begin ();

    printf ("\t>>> Startup Graphics...\n");
    Graphics.Begin ();

    printf ("\t>>> Startup I2C inputs...\n");
    Client.Begin (DISPLAY_I2C_ADDRESS);
    delay (200);   // Give time for the graphics subsystem threads to start

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


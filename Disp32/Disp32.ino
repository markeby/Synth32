//
// pinout of ESP32 DevKit found here:
// https://circuits4you.com/2018/12/31/esp32-devkit-esp32-wroom-gpio-pinout/
//
#include <Arduino.h>

#include "config.h"
#include "Settings.h"
#include "SerialMonitor.h"
#include "WebOTA.h"
#include "SynthFront.h"

//#######################################################################
SYNTH_FRONT_C   SynthFront (0, FaderMapArray, KnobMapArray, PitchMapArray, SwitchMapArray);

bool       SystemError         = false;
bool       SystemFail          = false;
float      DeltaTime           = 0;
int        DeltaMicro          = 0;             // micro second interval.
float      AverageDeltaTime    = 0;
uint64_t   RunTime             = 0;
bool       DebugMidi           = false;
bool       DebugDtoA           = false;
bool       DebugOsc            = false;
bool       DebugSynth          = false;

bool       AnalogDiagEnabled   = false;
int        AnalogDiagDevice    = 0;

// this is used to add a task to core 0
//TaskHandle_t  Core0TaskHnd;

#include "Test.h"

//#######################################################################
inline int TimeDeltaMiicro (void)
    {
    static uint64_t strt = 0;
    int             delta;

    RunTime =  micros ();
    delta = (int)((uint64_t)RunTime - (uint64_t)strt);
    strt = RunTime;
    return (delta);
    }

//#######################################################################
inline bool TickTime (void)
    {
    static uint64_t loop_cnt_10hz = 0;
    static uint64_t icount = 0;

    RunTime       += DeltaMicro;
    loop_cnt_10hz += DeltaMicro;
    icount++;

    if ( loop_cnt_10hz >= MILLI_TO_MICRO (100)  )
        {
        AverageDeltaTime = MICRO_TO_MILLI (loop_cnt_10hz / icount);
        loop_cnt_10hz = 0;
        icount = 0;
        return (true);
        }
    return (false);
    }

//#######################################################################
inline void TickState (void)
    {
    static uint32_t counter = 1;

    if ( --counter == 0 )
        {
        digitalWrite (HEARTBEAT_PIN, LOW);       // LED on
        counter = 10;
        }
    if ( SystemError || SystemFail )
        {
        if ( counter % 4 )
            {
            digitalWrite (HEARTBEAT_PIN, HIGH);  // LED off
            }
        else
            {
            digitalWrite (HEARTBEAT_PIN, LOW);   // LED on
            }
        }
    if ( counter == 9 )
        digitalWrite (HEARTBEAT_PIN, HIGH);      // LED off
    }


//#######################################################################
//#######################################################################
void setup (void)
    {
    bool fault = false;
    Serial.begin (115200);

    Settings.Begin ();        // System settings
    printf ("\t>>> Start Settings config...\n");
    Settings.Begin ();

    pinMode (HEARTBEAT_PIN, OUTPUT);
    pinMode (RED_PIN,       OUTPUT);
    pinMode (GREEN_PIN,     OUTPUT);
    digitalWrite (RED_PIN, HIGH);           // Red off
    digitalWrite (GREEN_PIN, HIGH);         // Green off
    digitalWrite (HEARTBEAT_PIN, LOW);      // Blue hearbeat LED off

    printf ("\t>>> Startup OTA...\n");
    UpdateOta.Setup (Settings.GetSSID (), Settings.GetPasswd ());

 //   xTaskCreatePinnedToCore (Core0Task, "Core0Task", 8000, NULL, 999, &Core0TaskHnd, 0);

    SynthFront.Begin ();

    printf ("\t>>> System startup complete.\n\n\n");
    }

//#######################################################################
void Core0TaskSetup (void)
    {
    // init your stuff for core0 here
#ifdef ADC_TO_MIDI_ENABLED
    AdcMul_Init ();
#endif //ADC_TO_MIDI_ENABLED
    }

//#######################################################################
void Core0TaskLoop (void)
    {
    // put your loop stuff for core0 here
#ifdef ADC_TO_MIDI_ENABLED
    AdcMul_Process ();
#endif //ADC_TO_MIDI_ENABLED
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
    // heartbeat and error alerts based on time intervals
    DeltaMicro = TimeDeltaMiicro ();
    DeltaTime = MICRO_TO_MILLI (DeltaMicro);
    if ( TickTime () )
        TickState ();

    // Wifi connection manager
    if ( !UpdateOta.WiFiStatus () )
        {
        if ( UpdateOta.WaitWiFi () )
            {
            UpdateOta.Begin ();
            SynthFront.SendReset ();
            }

        }
    SynthFront.Loop ();
    Monitor.Loop ();
    }


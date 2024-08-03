//
// pinout of ESP32 DevKit found here:
// https://circuits4you.com/2018/12/31/esp32-devkit-esp32-wroom-gpio-pinout/
//
#include <Arduino.h>

#include "config.h"
#include "Settings.h"
#include "SerialMonitor.h"
#include "FrontEnd.h"
#include "Diagnostics.h"
#include "UpdateOTA.h"

//#######################################################################
I2C_LOCATION_T  BusI2C[] =
//    Cluster   Slice   Channel   DtoA  AtoD    Dig     Name
    { { 0,        0,     0x60,     4,    0,      0,  "D/A #00, 01, 02, 03" },
      { 0,        1,     0x64,     4,    0,      0,  "D/A #04, 05, 06, 07" },
      { 0,        2,     0x60,     4,    0,      0,  "D/A #08, 09, 10, 11" },
      { 0,        3,     0x60,     4,    0,      0,  "D/A #12, 13, 14, 15" },
      { 0,        4,     0x60,     4,    0,      0,  "D/A #16, 17, 18, 19" },
      { 0,        5,     0x60,     4,    0,      0,  "D/A #20, 21, 22, 23" },
      { 0,        6,     0x60,     4,    0,      0,  "D/A #24, 25, 26, 27" },
      { 0,        7,     0x60,     4,    0,      0,  "D/A #28, 29, 30, 31" },
      { 1,        0,     0x60,     4,    0,      0,  "D/A #32, 33, 35, 35" },
      { 1,        1,     0x60,     4,    0,      0,  "D/A #36, 37, 38, 39" },
      { 1,        2,     0x60,     4,    0,      0,  "D/A #40, 41, 42, 43" },
      { 1,        3,     0x60,     4,    0,      0,  "D/A #44, 45, 46, 47" },
      { 1,        4,     0x60,     4,    0,      0,  "D/A #48, 49, 50, 51" },
      { 1,        5,     0x60,     4,    0,      0,  "D/A #52, 53, 54, 55" },
      { 1,        6,     0x60,     4,    0,      0,  "D/A #56, 57, 58, 59" },
      { 1,        7,     0x60,     4,    0,      0,  "D/A #60, 61, 62, 63" },
      { 2,        0,     0x60,     4,    0,      0,  "D/A #64, 65, 66, 67" },
      { 2,        1,     0x60,     4,    0,      0,  "D/A #68, 69, 70, 71" },
      { 2,        2,     0x60,     4,    0,      0,  "D/A #72, 73, 74, 75" },
      { 2,        3,     0x60,     4,    0,      0,  "D/A #76, 77, 78, 79" },
      { 2,        4,     0x60,     4,    0,      0,  "D/A #80, 81, 82, 83" },
      { 2,        5,     0x60,     4,    0,      0,  "D/A #84, 85, 86, 87" },
      { 2,        7,     0x20,     0,    0,     16,  "Dig #88  - 103     " },
      { 2,        7,     0x21,     0,    0,     16,  "Dig #104 - 119     " },
      { 2,        7,     0x22,     0,    0,     16,  "Dig #120 - 135     " },
      { -1,      -1,     -1,     -1,    -1,      -1,  nullptr },
    };

//#######################################################################
I2C_INTERFACE_C I2cDevices (BusI2C);
SYNTH_FRONT_C   SynthFront (FaderMapArray, KnobMapArray, SwitchMapArray);

bool       SystemError          = false;
bool       SystemFail           = false;
bool       SynthActive          = false;
float      DeltaTimeMilli       = 0;             // Millisecond interval.
float      DeltaTimeMicro       = 0;             // Microsecond interval
float      DeltaTimeMilliAvg    = 0;
float      LongestTimeMilli     = 0;
uint64_t   RunTime              = 0;
bool       DebugMidi            = false;
bool       DebugI2C             = false;
bool       DebugOsc             = false;
bool       DebugSynth           = false;
bool       DebugDisp            = false;
bool       AnalogDiagEnabled    = false;
int        AnalogDiagDevice     = 0;

// this is used to add a task to core 0
//TaskHandle_t  Core0TaskHnd;

#include "Test.h"

//#######################################################################
inline void TimeDelta (void)
    {
    static uint64_t strt = 0;       // Starting time for next frame delta calculation

    RunTime = micros ();
    DeltaTimeMicro = (int)((uint64_t)RunTime - (uint64_t)strt);
    DeltaTimeMilli = MICRO_TO_MILLI (DeltaTimeMicro);
    if ( DeltaTimeMilliAvg == 0 )
        DeltaTimeMilliAvg = DeltaTimeMilli;
    else
        DeltaTimeMilliAvg = (DeltaTimeMilliAvg + DeltaTimeMilli) / 2;
    strt = RunTime;
    if ( DeltaTimeMilli > 210 )     // throw out long serial debug outputs
        return;
    if ( DeltaTimeMilli > LongestTimeMilli )
        LongestTimeMilli = DeltaTimeMilli;
    }

//#######################################################################
inline bool TickTime (void)
    {
    static uint64_t loop_cnt_100hz = 0;
    static uint64_t icount = 0;

    loop_cnt_100hz += DeltaTimeMicro;
    icount++;

    if ( loop_cnt_100hz >= MILLI_TO_MICRO (10)  )
        {
        loop_cnt_100hz = 0;
        icount = 0;
        return (true);
        }
    return (false);
    }

//#######################################################################
inline void TickState (void)
    {
    static uint32_t counter0 = 1;

    if ( --counter0 == 0 )
        {
        digitalWrite (HEARTBEAT_PIN, HIGH);     // LED on
        counter0 = 100;
        }
    if ( SystemError || SystemFail )
        {
        if ( counter0 % 4 )
            {
            digitalWrite (BEEP_PIN, LOW);       // Tone off
            digitalWrite (HEARTBEAT_PIN, LOW);  // LED off
            }
        else
            {
            digitalWrite (BEEP_PIN, HIGH);      // Tone on
            digitalWrite (HEARTBEAT_PIN, HIGH); // LED on
            }
        }
    if ( counter0 == 98 )
        digitalWrite (HEARTBEAT_PIN, LOW);      // LED off
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
    pinMode (BEEP_PIN,      OUTPUT);
    digitalWrite (BEEP_PIN, LOW);           // Tone off
    digitalWrite (HEARTBEAT_PIN, LOW);      // LED off

    printf ("\t>>> Startup OTA...\n");
    UpdateOTA.Setup (Settings.GetSSID (), Settings.GetPasswd ());

    printf ("\t>>> Starting I2C & devices...\n");
    int errcnt = I2cDevices.Begin ();
    if ( errcnt > 0 )
        {
//        if ( errcnt == I2cDevices.NumBoards () )
            SystemFail = true;
//        else
            SystemError = true;
        }
    if ( SystemFail )
        Serial << "*******  Synth interface is not operational *******" << endl << endl << endl;

 //   xTaskCreatePinnedToCore (Core0Task, "Core0Task", 8000, NULL, 999, &Core0TaskHnd, 0);

    printf ("\t>>> System startup complete.\n");
    if ( SystemError )
        {
        printf ("\t>## Not starting synth.\n");
        }
    else
        {
        printf ("\t>>> Starting Synth...\n");
        SynthActive = true;

        // Setup initial state of synth
        SynthFront.Begin (START_OSC_ANALOG, START_NOISE_ANALOG, START_NOISE_DIGITAL);

        printf ("\t>>> Starting display communications port SDA = %d  SCL = %d\n", MSG_SDA, MSG_SCL);
        DisplayMessage.Begin (DISPLAY_I2C_ADDRESS, MSG_SDA, MSG_SCL);

        delay (1500);   // Give time for the graphics subsystem threads to start and Wifi to connect

        SynthFront.DisplayUpdate ();

        // initial test settings
        for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
            {
            SynthFront.SetSustainLevel (z, MAX_MVAL);
            SynthFront.ChannelSetSelect(z, false);
            SynthFront.SetMaxLevel (z, 0);
            }
//        SynthFront.ChannelSetSelect (4, true);
//        SynthFront.SetAttackTime (2);
//        SynthFront.SetDecayTime (0);
//        SynthFront.SetReleaseTime (22);
//        SynthFront.SetSustainTime (0);
//        SynthFront.SetMaxLevel (4, 100);
//        SynthFront.ChannelSetSelect (4, false);

        printf("\t>>> Synth ready.\n");
        }
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
void loop (void)
    {
    TimeDelta ();
    if ( TickTime () )
        TickState ();

    // Wifi connection manager
    if ( !UpdateOTA.WiFiStatus () )
        UpdateOTA.WaitWiFi ();

    if ( SynthActive )
        {
        SineWave.Loop (DeltaTimeMilli);     // Process sine wave for envelope generator modulation
        SynthFront.Loop ();
        if ( DisplayMessage.Loop () )
            SynthFront.DisplayUpdate ();
        }
    else if ( AnalogDiagEnabled )
        AnalogDiagnostics ();
    UpdateOTA.Loop ();
    Monitor.Loop ();
    }


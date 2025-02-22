//
// pinout of ESP32 DevKit found here:
// https://circuits4you.com/2018/12/31/esp32-devkit-esp32-wroom-gpio-pinout/
//
#include <Arduino.h>

#include "Settings.h"
#include "SerialMonitor.h"
#include "I2Cmessages.h"
#include "FrontEnd.h"
#include "Diagnostics.h"
#include "UpdateOTA.h"

// SET_LOOP_TASK_STACK_SIZE(16 * 1024);  // 16KB

//#######################################################################
I2C_LOCATION_T  BusI2C[] =
//    Cluster   Slice   Port   DtoA  AtoD    Dig     Name
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
      { 2,        6,     0x20,     0,    0,     16,  "Dig #88  - 103     " },   // Multiplexer
      { 2,        6,     0x21,     0,    0,     16,  "Dig #104 - 119     " },   // Noise output
      { 2,        6,     0x38,     0,    0,      8,  "Dig #120 - 135     " },   // LFO controls
      //      { 2,        7,     0x20,     0,    0,     16,  "Dig #136 - 151     " },   // Digital 48
      //      { 2,        7,     0x21,     0,    0,     16,  "Dig #152 - 167     " },   // Digital 48
      //      { 2,        7,     0x22,     0,    0,     16,  "Dig #168 - 183     " },   // Digital 48
      {-1,       -1,       -1,    -1,   -1,     -1,   nullptr }
    };

//#######################################################################
I2C_INTERFACE_C I2cDevices (BusI2C);
SYNTH_FRONT_C   SynthFront (FaderMapArray, KnobMapArray, SwitchMapArray, XlMapArray);

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
    if ( SystemError )
        {
        if ( counter0 % 25 )
            {
            digitalWrite (HEARTBEAT_PIN, LOW);  // LED off
            }
        else
            {
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

    printf ("\t>>> Start Settings config...\n");
    Settings.Begin ();    // System settings

    pinMode (HEARTBEAT_PIN, OUTPUT);
    pinMode (BEEP_PIN,      OUTPUT);
    digitalWrite (BEEP_PIN, LOW);           // Tone off
    digitalWrite (HEARTBEAT_PIN, LOW);      // LED off

    printf ("\t>>> Startup OTA...\n");
    UpdateOTA.Setup (Settings.GetSSID (), Settings.GetPasswd ());

    printf ("\t>>> Starting I2C & devices...\n");
    int errcnt = I2cDevices.Begin ();
    if ( errcnt > 0 )
        SystemError = true;

    if ( SystemFail )
        Serial << "*******  Synth interface is not operational *******" << endl << endl << endl;
    else
        {
        printf ("\t>>> System startup complete.\n");
        if ( SystemError )
            printf ("\t>## Not all synth interce registers are active.\n");

        printf ("\t>>> Starting display communications.\n");
        DisplayMessage.Begin (DISPLAY_I2C_ADDRESS, MSG_SDA, MSG_SCL);

        delay (1500);   // Give time for the graphics subsystem threads to start and Wifi to connect

        printf ("\t>>> Starting Synth...\n");
        SynthActive = true;

        // Setup initial state of synth
        SynthFront.Begin (START_OSC_ANALOG, START_MULT_DIGITAL, START_NOISE_DIGITAL, START_LFO_DIGITAL);

        SynthFront.Multiplex ()->SetOn(MULT_N::MULT_SOURCE::OSC, MULT_N::MULT_GROUP::ALL, MULT_N::MULT_OUTPUT::DIRECT);

        SynthFront.ResolveMapAllocation ();
        printf("\t>>> Synth ready.\n");
        }
    }

//#######################################################################
void loop (void)
    {
    static bool first = true;

    TimeDelta ();
    if ( TickTime () )
        TickState ();

    // Wifi connection manager
    if ( !UpdateOTA.WiFiStatus () )
        UpdateOTA.WaitWiFi ();

    if ( SynthActive )
        {
        SoftLFO.Loop (DeltaTimeMilli);     // Process sine wave for envelope generator modulation
        SynthFront.Loop ();
        if ( DisplayMessage.Loop () )
            SynthFront.ResolveMapAllocation ();
        }
    else if ( AnalogDiagEnabled )
        AnalogDiagnostics ();

    if ( first )
        {
        if ( RunTime > 5000000 )
            {
            first = false;
            SynthFront.ResetXL ();
            }
        }

    UpdateOTA.Loop ();
    Monitor.Loop ();
    }


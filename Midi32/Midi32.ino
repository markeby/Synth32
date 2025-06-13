//
// pinout of ESP32 DevKit found here:
// https://circuits4you.com/2018/12/31/esp32-devkit-esp32-wroom-gpio-pinout/
//
#include <Arduino.h>

#include "Settings.h"
#include "SerialMonitor.h"
#include "I2Cmessages.h"
#include "FrontEnd.h"
#include "UpdateOTA.h"

// SET_LOOP_TASK_STACK_SIZE(16 * 1024);  // 16KB

//#######################################################################
I2C_CLUSTERS_T  BusI2C[] = { 0, 1, 2, 3, 4, -1 };

I2C_LOCATION_T  DevicesI2C[] =
//    Cluster   Slice   Port   DtoA  AtoD    Dig     Name
    {
      { 4,        7,     0x20,     0,    0,     16, "Dig #0   - 15 " }, // Multiplexer
      { 2,        6,     0x21,     0,    0,     16, "Dig #16  - 31 " }, // Noise output
      { 2,        6,     0x38,     0,    0,      8, "Dig #32  - 39 " }, // LFO controls
      { 2,        6,     0x22,     0,    0,     16, "Dig #40  - 55 " }, // Modulation control
      { 2,        6,     0x48,     0,    4,      0, "A/D #56  - 59 " }, // LFO Static voltage A/D monitor (2.5V)
      { 0,        0,     0x60,     4,    0,      0, "D/A #60  - 63 " }, // unallocated
      { 0,        1,     0x64,     4,    0,      0, "D/A #64  - 67 " },
      { 0,        2,     0x60,     4,    0,      0, "D/A #68  - 71 " },
      { 0,        3,     0x60,     4,    0,      0, "D/A #72  - 75 " },
      { 0,        4,     0x60,     4,    0,      0, "D/A #76  - 79 " },
      { 0,        5,     0x60,     4,    0,      0, "D/A #80  - 83 " },
      { 0,        6,     0x60,     4,    0,      0, "D/A #84  - 87 " },
      { 0,        7,     0x60,     4,    0,      0, "D/A #88  - 91 " },
      { 1,        0,     0x60,     4,    0,      0, "D/A #92  - 95 " },
      { 1,        1,     0x60,     4,    0,      0, "D/A #96  - 99 " },
      { 1,        2,     0x60,     4,    0,      0, "D/A #100 - 103" }, // LFO analog
      { 1,        3,     0x60,     4,    0,      0, "D/A #104 - 107" },
      { 1,        4,     0x60,     4,    0,      0, "D/A #108 - 111" },
      { 1,        5,     0x60,     4,    0,      0, "D/A #112 - 115" },
      { 1,        6,     0x60,     4,    0,      0, "D/A #116 - 119" },
      { 1,        7,     0x60,     4,    0,      0, "D/A #120 - 123" },
      { 2,        0,     0x60,     4,    0,      0, "D/A #124 - 127" },
      { 2,        1,     0x60,     4,    0,      0, "D/A #128 - 131" },
      { 2,        2,     0x60,     4,    0,      0, "D/A #132 - 135" },
      { 2,        3,     0x60,     4,    0,      0, "D/A #136 - 139" },
      { 2,        4,     0x60,     4,    0,      0, "D/A #140 - 143" },
      { 2,        5,     0x60,     4,    0,      0, "D/A #144 - 147" },
      { 3,        0,     0x60,     4,    0,      0, "D/A #148 - 151" }, // Osc #0
      { 3,        0,     0x61,     4,    0,      0, "D/A #152 - 155" },
      { 3,        0,     0x62,     4,    0,      0, "D/A #156 - 159" }, // Osc #1
      { 3,        0,     0x63,     4,    0,      0, "D/A #160 - 163" },
      { 3,        1,     0x60,     4,    0,      0, "D/A #164 - 167" }, // Osc #2
      { 3,        1,     0x61,     4,    0,      0, "D/A #168 - 171" },
      { 3,        1,     0x62,     4,    0,      0, "D/A #172 - 175" }, // Osc #3
      { 3,        1,     0x63,     4,    0,      0, "D/A #176 - 179" },
      { 3,        2,     0x60,     4,    0,      0, "D/A #180 - 183" }, // Osc #4
      { 3,        2,     0x61,     4,    0,      0, "D/A #184 - 187" },
      { 3,        2,     0x62,     4,    0,      0, "D/A #188 - 191" }, // Osc #5
      { 3,        2,     0x63,     4,    0,      0, "D/A #192 - 195" },
      { 3,        3,     0x60,     4,    0,      0, "D/A #196 - 199" }, // Osc #6
      { 3,        3,     0x61,     4,    0,      0, "D/A #200 - 203" },
      { 3,        3,     0x62,     4,    0,      0, "D/A #204 - 207" }, // Osc #7
      { 3,        3,     0x63,     4,    0,      0, "D/A #208 - 211" },
      {-1,       -1,       -1,    -1,   -1,     -1,  nullptr }
    };

//#################################################
//  Synth I2C interface starting indexes
//#################################################
#define START_MULT_DIGITAL      0
#define START_NOISE_DIGITAL     16
#define START_LFO_DIGITAL       32
#define START_MOD_MUX           40
#define START_LFO_AD_OFFSET     48
#define START_A_D               56
#define START_LFO_ANALOG        100
#define START_OSC_ANALOG        148

//#######################################################################
I2C_INTERFACE_C I2cDevices (BusI2C, DevicesI2C);
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
    if ( SystemFail )
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

    pinMode      (HEARTBEAT_PIN, OUTPUT);
    pinMode      (BEEP_PIN, OUTPUT);
    digitalWrite (BEEP_PIN, LOW);           // Tone off
    digitalWrite (HEARTBEAT_PIN, LOW);      // LED off

    printf ("\t>>> Startup OTA...\n");
    UpdateOTA.Setup (Settings.GetSSID (), Settings.GetPasswd ());

    printf ("\t>>> Starting I2C & devices...\n");

    // First, validate for good I2C I/O bus
    int cnt = I2cDevices.Begin ();
    if ( cnt != 0 )
        {
        SystemError = true;
        if ( cnt < 0 )
            {
            SystemFail = true;
            return;
            }
        }

    if ( SystemFail )
        printf ("*******  Synth interface is not operational *******\n\n\n");
    else
        {
        printf ("\t>>> System startup complete.\n");

        printf ("\t>>> Starting display communications.\n");
        DisplayMessage.Begin (DISPLAY_I2C_ADDRESS, MSG_SDA, MSG_SCL);

        delay (1000);   // Give time for the graphics subsystem threads to start and Wifi to connect

        printf ("\t>>> Starting Synth...\n");

        // Setup initial state of synth
        SynthFront.Begin (START_OSC_ANALOG, START_MULT_DIGITAL, START_NOISE_DIGITAL, START_LFO_ANALOG, START_LFO_DIGITAL, START_MOD_MUX, START_A_D);
        delay (1000);
        SynthActive = true;
        printf ("\t>>> Synth ready.\n");
        if ( SystemError )
            printf ("\n\t>## Not all synth interface registers are active.\n\n");
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
        SoftLFO.Loop    (DeltaTimeMilli);     // Process sine wave for envelope generator modulation
        SynthFront.Loop ();
        if ( DisplayMessage.Loop () )
            {
            printf ("\n\t>>> Display reset requested\n");
            SynthFront.ResolveMapAllocation ();
            }
        I2cDevices.Loop ();

        if ( first )
            {
            if ( RunTime > 5000000 )
                {
                SynthFront.ResetUSB ();
                first = false;
                }
            }
        }

    UpdateOTA.Loop ();
    Monitor.Loop   ();
    }


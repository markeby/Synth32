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

// Enables power on to wait for single key hit to continue.
//#define POWERON_TESTING

// SET_LOOP_TASK_STACK_SIZE(16 * 1024);  // 16KB

//#######################################################################
I2C_CLUSTERS_T  BusI2C[] = { 0, 1, 7, -1 };

I2C_LOCATION_T  DevicesI2C[] =
//    Cluster   Slice   Port   DtoA  AtoD    Dig     Name
    {
      { 1,        7,     0x38,     0,    0,      8, "Dig 0   - 7"   }, // Test position for new MUXmixer
      { 1,        7,     0x60,     4,    0,      0, "D/A 8   - 11"  },
      { 1,        6,     0x21,     0,    0,     16, "Dig 12  - 27"  }, // Noise output
      { 1,        5,     0x38,     0,    0,      8, "Dig 28  - 35"  }, // LFO controls
      { 7,        5,     0x60,     4,    0,      0, "D/A 36  - 39"  }, // LFO Analog controls
      { 7,        6,     0x60,     4,    0,      0, "D/A 40  - 43"  },
      { 7,        7,     0x60,     4,    0,      0, "D/A 44  - 47"  },
      { 1,        4,     0x22,     0,    0,     16, "Dig 48  - 63"  }, // Modulation MUX
      { 1,        4,     0x48,     0,    4,      0, "A/D 64  - 67"  }, // LFO Static voltage A/D monitor (2.5V)
      { 0,        0,     0x60,     4,    0,      0, "D/A 68  - 71"  }, // Osc #0
      { 0,        0,     0x61,     4,    0,      0, "D/A 72  - 75"  },
      { 0,        0,     0x62,     4,    0,      0, "D/A 76  - 79"  }, // Osc #1
      { 0,        0,     0x63,     4,    0,      0, "D/A 80  - 83"  },
      { 0,        4,     0x60,     4,    0,      0, "D/A 84  - 87"  }, // Filter #0 & #1
      { 0,        4,     0x38,     0,    0,      8, "Dig 88  - 95"  },
      { 0,        1,     0x60,     4,    0,      0, "D/A 96  - 99"  }, // Osc #2
      { 0,        1,     0x61,     4,    0,      0, "D/A 100 - 103" },
      { 0,        1,     0x62,     4,    0,      0, "D/A 104 - 107" }, // Osc #3
      { 0,        1,     0x63,     4,    0,      0, "D/A 108 - 111" },
      { 0,        5,     0x60,     4,    0,      0, "D/A 112 - 115" }, // Filter #2 & #3
      { 0,        5,     0x38,     0,    0,      8, "Dig 116 - 123" },
      { 0,        2,     0x60,     4,    0,      0, "D/A 124 - 127" }, // Osc #4
      { 0,        2,     0x61,     4,    0,      0, "D/A 128 - 131" },
      { 0,        2,     0x62,     4,    0,      0, "D/A 132 - 135" }, // Osc #5
      { 0,        2,     0x63,     4,    0,      0, "D/A 136 - 139" },
      { 0,        6,     0x60,     4,    0,      0, "D/A 140 - 143" }, // Filter #4 & #5
      { 0,        6,     0x38,     0,    0,      8, "Dig 144 - 151" },
      { 0,        3,     0x60,     4,    0,      0, "D/A 152 - 155" }, // Osc #6
      { 0,        3,     0x61,     4,    0,      0, "D/A 156 - 159" },
      { 0,        3,     0x62,     4,    0,      0, "D/A 160 - 163" }, // Osc #7
      { 0,        3,     0x63,     4,    0,      0, "D/A 164 - 167" },
      { 0,        7,     0x60,     4,    0,      0, "D/A 168 - 171" }, // Filter #6 & #7
      { 0,        7,     0x38,     0,    0,      8, "Dig 172 - 179" },
      { 7,        0,     0x60,     4,    0,      0, "D/A 180 - 183" }, // Unallocated D/A
      { 7,        1,     0x60,     4,    0,      0, "D/A 184 - 187" },
      { 7,        2,     0x60,     4,    0,      0, "D/A 188 - 191" },
      { 7,        3,     0x60,     4,    0,      0, "D/A 192 - 195" },
      { 7,        4,     0x60,     4,    0,      0, "D/A 196 - 199" },
      {-1,       -1,       -1,    -1,   -1,     -1,  nullptr }
    };

//#################################################
//  Synth I2C interface starting indexes
//#################################################
#define START_MIXER             0
#define START_NOISE_DIGITAL     12
#define START_LFO_CONTROL       28
#define START_MOD_MUX           48
#define START_A_D               64
#define START_VOICE_CONTROL     68

//#######################################################################
MONITOR_C       Monitor;
I2C_INTERFACE_C I2cDevices (BusI2C, DevicesI2C);
SYNTH_FRONT_C   SynthFront (FaderMidiMapArray, KnobMidiMapArray, SwitchMidiMapArray, XL_MidiMapArray);

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
bool       DebugSynth           = false;
bool       DebugDisp            = false;

static uint64_t startTime       = 0;

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

    BootDebug ();  // Pause here so that init of serial port in the monitor class can complete on power up
    printf ("\n\t>>> Startup of Midi32 %s %s\n", __DATE__, __TIME__);
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
            }
        }

    if ( SystemFail )
        printf ("\n\n*******  Synth interface is not operational *******\n\n\n");
    else
        {
        printf ("\t>>> System startup complete.\n");

        printf ("\t>>> Starting display communications.\n");
        DisplayMessage.Begin (DISPLAY_I2C_ADDRESS, MSG_SDA, MSG_SCL);

        delay (1000);   // Give time for the graphics subsystem threads to start and Wifi to connect

        printf ("\t>>> Starting Synth...\n");

        // Setup initial state of synth
        SynthFront.Begin (START_VOICE_CONTROL, START_MIXER, START_NOISE_DIGITAL, START_LFO_CONTROL, START_MOD_MUX, START_A_D);
        SynthActive = true;
        SynthFront.LoadDefaultConfig ();
        printf ("\t>>> Synth ready.\a\a\n");
        if ( SystemError )
            printf ("\n\t>## Not all synth interface registers are active.\n\n");
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

    Monitor.Loop   ();
    UpdateOTA.Loop ();

    if ( SynthActive )
        {
        SoftLFO.Loop (DeltaTimeMilli);      // Process sine wave for envelope generator level modulation
        SynthFront.Loop ();
        if ( DisplayMessage.Loop () )
            {
            printf ("\n\t>>> Display reset requested\n");
            SynthFront.ResolveMapAllocation ();
            }
        I2cDevices.Loop ();
        }
    }


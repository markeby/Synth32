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
I2C_LOCATION_T  BusI2C[] =
//    Cluster   Slice   Channel   DtoA  AtoD    Dig     Name
    { { 0,        0,     0x60,     4,    0,      0,  "D/A #00, 01, 02, 03" },
      { 0,        1,     0x60,     4,    0,      0,  "D/A #04, 05, 06, 07" },
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
SYNTH_FRONT_C   SynthFront (START_OSC_ANALOG, FaderMapArray, KnobMapArray, SwitchMapArray);

bool       SystemError         = false;
bool       SystemFail          = false;
bool       SynthActive         = false;
float      DeltaTime           = 0;             // micro second interval.
int        DeltaMicro          = 0;
float      AverageDeltaTime    = 0;
uint64_t   RunTime             = 0;
bool       DebugMidi           = false;
bool       DebugDtoA           = false;
bool       DebugOsc            = false;
bool       DebugSynth          = true;

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
    static uint32_t counter0 = 1;

    if ( --counter0 == 0 )
        {
        digitalWrite (HEARTBEAT_PIN, HIGH);     // LED on
        counter0 = 20;
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
    if ( counter0 == 9 )
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
    UpdateOta.Setup (Settings.GetSSID (), Settings.GetPasswd ());

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
        SynthFront.Begin ();
        SynthFront.BeginNoise (START_NOISE_DIG);
        for ( int z = 0;  z < 5;  z++ )
            {
            SynthFront.SetOscSustainLevel (z, MAX_MVAL);
            SynthFront.OscChannelSelect(z, false);
            }
//        SynthFront.OscChannelSelect(0, true);
        SynthFront.OscChannelSelect(4, true);
        SynthFront.SetOscAttackTime (6);
        SynthFront.SetOscDecayTime (0);
        SynthFront.SetOscReleaseTime (35);
        SynthFront.SetOscSustainTime (0);
        SynthFront.SetOscMaxLevel (MAX_MVAL);
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
void AnalogDiagnostics (void)
    {
    static int lastdevice = -1;

    if ( AnalogDiagDevice != lastdevice )
        {
        if ( AnalogDiagDevice < 0 )
            AnalogDiagDevice = 0;
        if ( AnalogDiagDevice >= I2cDevices.NumAnalog () )
             AnalogDiagDevice = lastdevice;

        lastdevice = AnalogDiagDevice;
        I2cDevices.Zero ();                         // Clear all D/A
        I2cDevices.D2Analog (lastdevice, 4095);     // set next D/A

        printf("Analog diag:  Device %d  Board %d, Channel V%c\n", lastdevice, lastdevice / 4, (lastdevice % 4) + 'A');
        I2cDevices.UpdateDigital ();    // Update Digital output ports
        I2cDevices.UpdateAnalog ();     // Update D/A ports
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
            UpdateOta.Begin ();
        }

    if ( SynthActive )
        SynthFront.Loop ();
    else if ( AnalogDiagEnabled )
        AnalogDiagnostics ();
    TestSine ();
    Monitor.Loop ();
    }


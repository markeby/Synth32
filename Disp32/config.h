// config.h
//
// Project settings here (defines, numbers, etc.)

#pragma once

#include <Streaming.h>
#include "../Common/SynthCommon.h"

//#####################################
// Usefull constants
//#####################################
#define MICRO_TO_MILLI(x) ((x) * 0.001)
#define MILLI_TO_MICRO(x) ((x) * 1000)

//#####################################
// In case I get confused with Python
//#####################################
#define False false
#define True  true

//#####################################
//   debug controls
//#####################################
extern bool DebugInterface;
extern bool DebugGraphics;

//#################################################
//    Serial 1 Midi32->Disp32 communications
//#################################################
#define RXDM1    15  //12
#define TXDM1    16  //13

//#################################################
//   Global system variables
//#################################################
extern float    DeltaTime;
extern uint64_t RunTime;
extern float    AverageDeltaTime;
extern int      DeltaMicro;


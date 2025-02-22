//#######################################################################
// Module:     Multiplexer.h
// Descrption: Audio multiplex controls
// Creator:    markeby
// Date:       10/31/2024
//#######################################################################
#pragma once

#include "Config.h"

//#######################################################################
namespace MULT_N
{
enum class MULT_SOURCE
    {
    OSC = 0,
    FILTER3STATE,
    };

enum class MULT_GROUP
    {
    ALL = 0,
    ONE,
    TWO,
    THREE,
    FOUR,
    };

enum class MULT_OUTPUT
    {
    NONE = 0,
    DIRECT,
    FILTER3STATE,
    };

#define SIZE_OSC_ALL    16
#define SIZE_OSC_OUTPUT 8
#define GROUP_COUNT_OSC 2

}// end namespace MULT_N

//#######################################################################
class   MULTIPLEX_C
    {
private:
    union
    {
    uint16_t    OscAll[SIZE_OSC_ALL];
    struct
        {
        uint16_t    OscDirectDev[SIZE_OSC_OUTPUT];
        uint16_t    OscFilterDev[SIZE_OSC_OUTPUT];
        };
    };

    void Update       (uint16_t* pdev, uint16_t cnt, bool state);

public:
         MULTIPLEX_C (int first_device);
    void SetOn       (MULT_N::MULT_SOURCE source, MULT_N::MULT_GROUP group, MULT_N::MULT_OUTPUT output);
    void Clear       (void);
    };



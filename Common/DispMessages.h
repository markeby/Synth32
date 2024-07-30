//#######################################################################
// Module:     DispMessages.h
// Descrption: Display command messages
// Creator:    markeby
// Date:       7/23/2024
//#######################################################################
#pragma once

namespace DISP_MESSAGE_N
{
//######################################
//  Display device I2C addresss
//######################################
#define DISPLAY_I2C_ADDRESS     0x50

//######################################
//  Message length
//######################################
#define MESSAGE_LENGTH_CNTL     2
#define MESSAGE_LENGTH_PAGE     3
#define MESSAGE_LENGTH_OSC      5

#define RESET_TRIGGER_TIME  30000

//######################################
//  Control bytes
//######################################
enum class CMD_C: uint8_t
    {
    UPDATE  = 0xB0,
    PAGE    = 0xF0,
    PAUSE   = 0x00,
    GO      = 0xFF,
    };

//######################################
//  Control bytes text
//######################################
#ifdef ALLOCATE_DISP_TEXT
char* ClassCMD[] =
    {
    "UPDATE",
    "PAGE",
    "PAUSE",
    "GO",
    "NONE",
    "NONE",
    "NONE",
    };
#elif ALLOCATE_DISP_TEXT_REF
extern char* ClassCMD[];
#endif

//######################################
// Envelope selection bytes
//######################################
enum class ADSR_C: uint8_t
    {
    SINE = 0,
    TRIANGLE,
    SAWTOOTH,
    PULSE,
    SQUARE,
    NOISE,
    ALL
    };

//######################################
// Envelope selection text
//######################################
#ifdef ALLOCATE_DISP_TEXT
char* ClassADSR[] =
    {
    "SINE",
    "TRIANGLE",
    "SAWTOOTH",
    "PULSE",
    "SQUARE",
    "ALL",
    "NONE",
    "NONE",
    "NONE",
    "NONE",
    };
#elif ALLOCATE_DISP_TEXT_REF
extern char* ClassADSR[];
#endif

//######################################
// Function/Effects bytes
//######################################
enum class EFFECT_C: uint8_t
    {
    SELECTED = 0,
    BASE_VOL,
    MAX_LEVEL,
    ATTACK_TIME,
    DECAY_TIME,
    SUSTAIN_TIME,
    RELEASE_TIME,
    SUSTAIN_LEVEL,
    SAWTOOTH_DIRECTION,
    };

//######################################
// Function/Effects bytes text
//######################################
#ifdef ALLOCATE_DISP_TEXT
char* ClassEFFECT[] =
    {
    "SELECTED",
    "BASE_VOL",
    "MAX_LEVEL",
    "ATTACK_TIME",
    "DECAY_TIME",
    "SUSTAIN_TIME",
    "RELEASE_TIME",
    "SUSTAIN_LEVEL",
    "SAWTOOTH_DIRECTION",
    "NONE",
    "NONE",
    "NONE",
    "NONE",
    };
#elif ALLOCATE_DISP_TEXT_REF
extern char* ClassEFFECT[];
#endif

//######################################
// Function/Page select bytes
//######################################
enum class PAGE_C: uint8_t
    {
    PAGE_OSC = 0,
    PAGE_FILTER,
    PAGE_TUNING,
    NONE,
    };

//######################################
// Function/Effects bytes text
//######################################
#ifdef ALLOCATE_DISP_TEXT
char* ClassPAGE[] =
    {
    "OSCILLATOR",
    "FILTER",
    "TUNING",
    "NONE",
    "NONE",
    "NONE",
    "NONE",
    };
#elif ALLOCATE_DISP_TEXT_REF
extern char* ClassEFFECT[];
#endif

}


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
#define MESSAGE_LENGTH_CNT  2
#define MESSAGE_LENGTH_VCA  5

//######################################
//  Control bytes
//######################################
enum class CMD_C: uint8_t
    {
    CTRL_VCA    = 0xB0,
    CRTL_PAGE   = 0xF0,
    PAUSE       = 0x00,
    UPDATE      = 0xFF,
    };

//######################################
//  Control bytes text
//######################################
#ifdef ALLOCATE_DISP_TEXT
char* ClassCMD[] =
    {
    "CONTROL",
    "CRTL_PAGE",
    "PAUSE",
    "UPDATE",
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

}


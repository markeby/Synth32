//#######################################################################
// Module:     DispMessages.h
// Descrption: Display command messages
// Creator:    markeby
// Date:       7/23/2024
//#######################################################################
#pragma once

namespace DISP_MESSAGE_N
    {
    //###########################################
    //  Display device I2C addresss
    //###########################################
    #define DISPLAY_I2C_ADDRESS     0x50
    #define RESET_TRIGGER_TIME      30000

    //###########################################
    //  Message packets
    //###########################################

    //###########################################
    //           Update packet
    #define MESSAGE_LENGTH_UPDATE 5
    //           =============
    //            index             16 bit value
    //    CMD_C - chan - EFFECT_C - high - low
    //###########################################

    //###########################################
    //        Page show packet
    #define MESSAGE_LENGTH_PAGE 3
    //        ================
    //  PAGE_SHOW - 0 - PAGE_C
    //###########################################

    //###########################################
    //        Pause/Go packets
    #define MESSAGE_LENGTH_CNTL 2
    //        =============
    //  CMD_C - 0
    //###########################################

    //###########################################
    //  Control bytes
    //###########################################
    enum class CMD_C: uint8_t
        {
        UPDATE_PAGE_OSC      = 0xB0,
        UPDATE_PAGE_MOD,
        UPDATE_PAGE_FILTER,
        UPDATE_PAGE_TUNING,
        PAGE_SHOW            = 0x20,
        PAUSE                = 0x00,
        GO                   = 0xFF,
        };

    //###########################################
    // Function/Page select bytes
    //###########################################
    enum class PAGE_C: uint8_t
        {
        PAGE_OSC = 0,
        PAGE_MOD,
        PAGE_FILTER,
        PAGE_TUNING,
        NONE,
        PAGE_ADVANCE = 255
        };

    //###########################################
    // Function/Effects bytes text
    //###########################################
    #ifdef ALLOCATE_DISP_MESSAGES
    char* PageText[] =
        {
        "OSCILLATOR",
        "MODULATION",
        "FILTER",
        "TUNING",
        "NONE",
        "NONE",
        "NONE",
        "NONE",
        };
    #else
    extern char* PageText[];
    #endif

    //###########################################
    // Envelope selection bytes
    //###########################################
    enum class CHANNEL_C: uint8_t
        {
        SINE = 0,
        TRIANGLE,
        SAWTOOTH,
        PULSE,
        SQUARE,
        NOISE,
        ALL,
        HARDWARE_LFO,
        SOFTWARE_LFO,
        };

    //###########################################
    // Envelope selection text
    //###########################################
    #ifdef ALLOCATE_DISP_MESSAGES
    char* ChannelText[] =
        {
        "SINE",
        "TRIANGLE",
        "SAWTOOTH",
        "PULSE",
        "SQUARE",
        "ALL",
        "HARDWARE_LFO",
        "SOFTWARE_LFO",
        "NONE",
        "NONE",
        "NONE",
        "NONE",
        };
    #else
    extern char* ChannelText[];
    #endif

    //###########################################
    // Function/Effects bytes
    //###########################################
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
        PULSE_WIDTH,
        FREQ_LFO,
        };

    //###########################################
    // Function/Effects bytes text
    //###########################################
    #ifdef ALLOCATE_DISP_MESSAGES
    char* EffectText[] =
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
        "PULSE_WIDTH",
        "FREQ_COURSE",
        "FREQ_FINE",
        "NONE",
        "NONE",
        "NONE",
        "NONE",
        };
    #else
    extern char* EffectText[];
    #endif
    }


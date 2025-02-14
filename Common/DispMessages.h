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
    //  Control bytes
    //###########################################
    enum class CMD_C: byte
        {
        UPDATE_PAGE_OSC0      = 0xB0,
        UPDATE_PAGE_OSC1,
        UPDATE_PAGE_OSC2,
        UPDATE_PAGE_MOD,
        UPDATE_PAGE_FILTER,
        UPDATE_PAGE_TUNING,
        UPDATE_PAGE_MAP,
        PAGE_SHOW            = 0x20,
        RESET                = 0xFF,
        };

    //###########################################
    // Function/Page select bytes
    //###########################################
    enum class PAGE_C: byte
        {
        PAGE_OSC0 = 0,
        PAGE_OSC1,
        PAGE_OSC2,
        PAGE_MOD,
        PAGE_FILTER,
        PAGE_MIDI_MAP,
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
        "0 OSCILLATOR",
        "1 OSCILLATOR",
        "2 OSCILLATOR",
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
    enum class VOICE_C: byte
        {
        SINE = 0,
        TRIANGLE,
        SAWTOOTH,
        PULSE,
        NOISE,
        ALL,
        HARDWARE_LFO,
        SOFTWARE_LFO,
        };

    //###########################################
    // Envelope selection text
    //###########################################
    #ifdef ALLOCATE_DISP_MESSAGES
    char* VoiceText[] =
        {
        "SINE",
        "TRIANGLE",
        "RAMP",
        "PULSE",
        "NOISE",
        "ALL",
        "HARDWARE_LFO",
        "SOFTWARE_LFO",
        "NONE",
        "NONE",
        "NONE",
        "NONE",
        };
    #else
    extern char* VoiceText[];
    #endif

    //###########################################
    // Function/Effects bytes
    //###########################################
    enum class EFFECT_C: byte
        {
        SELECTED = 0,
        BASE_VOL,
        MAX_LEVEL,
        ATTACK_TIME,
        DECAY_TIME,
        RELEASE_TIME,
        SUSTAIN_LEVEL,
        SAWTOOTH_DIRECTION,
        PULSE_WIDTH,
        LFO_FREQ,
        DESELECTED,
        NOTE,
        NOISE,
        MAP_VOICE,
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
        "RELEASE_TIME",
        "SUSTAIN_LEVEL",
        "SAWTOOTH_DIRECTION",
        "PULSE_WIDTH",
        "LFO_FREQ",
        "DESELECTED",
        "NOTE",
        "NOISE",
        "NONE",
        "NONE",
        "NONE",
        };
    #else
    extern char* EffectText[];
    #endif

    }   // namespace DISP_MESSAGE_N


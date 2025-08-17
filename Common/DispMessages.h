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
    #define MESSAGE_LENGTH_UPDATE 6
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
        RESET = 0,
        PAGE_SHOW,
        SET_PAGE,
        UPDATE_PAGE_VOICE,
        UPDATE_PAGE_FILTER,
        UPDATE_PAGE_MOD,
        UPDATE_PAGE_MAP,
        UPDATE_PAGE_TUNING,
        UPDATE_PAGE_LOAD_SAVE,
        };

    //###########################################
    // Control bytes text
    //###########################################
    #ifdef ALLOCATE_DISP_MESSAGES
    char* CommandText[] =
        {
        "RESET",
        "PAGE_SHOW",
        "SET_PAGE_VOICE",
        "UPDATE_PAGE_VOICE",
        "UPDATE_PAGE_FILTER",
        "UPDATE_PAGE_MOD",
        "UPDATE_PAGE_MAP",
        "UPDATE_PAGE_TUNING",
        "NONE",
        "NONE",
        "NONE",
        };
    #else
    extern char* PageText[];
    #endif

    //###########################################
    // Page select bytes
    //###########################################
    enum class PAGE_C: byte
        {
        PAGE_OSC = 0,
        PAGE_FLT,
        PAGE_MOD,
        PAGE_MIDI_MAP,
        PAGE_CALIBRATION,
        PAGE_TUNING,
        PAGE_LOAD_SAVE,
        NONE,
        };

    //###########################################
    // Page select bytes text
    //###########################################
    #ifdef ALLOCATE_DISP_MESSAGES
    char* PageText[] =
        {
        "PAGE_OSC",
        "PAGE_FLT",
        "PAGE_MOD",
        "PAGE_MIDI_MAP",
        "PAGE_CALIBRATION",
        "PAGE TUNING",
        "PAGE LOAD/SAVE",
        "NONE",
        };
    #else
    extern char* PageText[];
    #endif

    //###########################################
    // Envelope selection bytes
    //###########################################
    enum class VOICE_OPT_C: byte
        {
        SINE = 0,
        TRIANGLE,
        RAMP,
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
    char* VoiceOptText[] =
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
    extern char* VoiceOptText[];
    #endif

    //###########################################
    // Function/Effects bytes
    //###########################################
    enum class EFFECT_C: byte
        {
        SELECTED = 0,
        BASE_LEVEL,
        MAX_LEVEL,
        ATTACK_TIME,
        DECAY_TIME,
        RELEASE_TIME,
        SUSTAIN_LEVEL,
        RAMP_DIRECTION,
        PULSE_WIDTH,
        LFO_FREQ,
        DESELECTED,
        NOTE,
        NOISE,
        MAP_VOICE,
        MESSAGE,
        VALUE,
        ALTERNATE,
        CONTROL,
        };

    //###########################################
    // Function/Effects bytes text
    //###########################################
    #ifdef ALLOCATE_DISP_MESSAGES
    char* EffectText[] =
        {
        "SELECTED",
        "BASE_LEVEL",
        "MAX_LEVEL",
        "ATTACK_TIME",
        "DECAY_TIME",
        "RELEASE_TIME",
        "SUSTAIN_LEVEL",
        "RAMP_DIRECTION",
        "PULSE_WIDTH",
        "LFO_FREQ",
        "DESELECTED",
        "NOTE",
        "NOISE",
        "MAP_VOICE",
        "MESSAGE",
        "VALUE",
        "ALTERNATE",
        "CONTROL",
        };
    #else
    extern char* EffectText[];
    #endif

    }   // namespace DISP_MESSAGE_N


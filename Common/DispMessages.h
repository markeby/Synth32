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
        SET_PAGE_VOICE,
        UPDATE_PAGE_VOICE,
        UPDATE_PAGE_MOD,
        UPDATE_PAGE_FILTER,
        UPDATE_PAGE_MAP,
        UPDATE_PAGE_TUNING,
        };

    //###########################################
    // Function/Effects bytes text
    //###########################################
    #ifdef ALLOCATE_DISP_MESSAGES
    char* CommandText[] =
        {
        "RESET",
        "PAGE_SHOW",
        "SET_PAGE_VOICE",
        "UPDATE_PAGE_VOICE",
        "UPDATE_PAGE_MOD",
        "UPDATE_PAGE_FILTER",
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
    // Function/Page select bytes
    //###########################################
    enum class PAGE_C: byte
        {
        PAGE_OSC0 = 0,
        PAGE_OSC1,
        PAGE_OSC2,
        PAGE_OSC3,
        PAGE_MOD,
        PAGE_FILTER,
        PAGE_MIDI_MAP,
        PAGE_TUNING,
        PAGE_ADVANCE,
        NONE,
        };

    //###########################################
    // Function/Effects bytes text
    //###########################################
    #ifdef ALLOCATE_DISP_MESSAGES
    char* PageText[] =
        {
        "PAGE_ADVANCE",
        "PAGE_OSC0",
        "PAGE_OSC1",
        "PAGE_OSC2",
        "PAGE_OSC3",
        "PAGE_MOD",
        "PAGE_FILTER",
        "PAGE_MIDI_MAP",
        "PAGE_TUNING",
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
        "MAP_VOICE",
        "NONE",
        "NONE",
        };
    #else
    extern char* EffectText[];
    #endif

    }   // namespace DISP_MESSAGE_N


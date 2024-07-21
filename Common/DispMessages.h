//#######################################################################
// Module:     DispMessages.h
// Descrption: Interface Synth32 => Disp32
// Creator:    markeby
// Date:       8/25/2023
//#######################################################################
#pragma once

namespace DISP_MESSAGE_N
    {
    enum class SHAPE_C: uint8_t {
        SINE = 0,
        TRIANGLE,
        SAWTOOTH,
        PULSE,
        SQUARE,
        ALL
        };

    enum class CMD_C: uint8_t {
        RENDER  = 0x20,
        CONTROL = 0xB0,
        RESET   = 0xFF,
        };

    enum class EFFECT_C: uint8_t {
      // Oscillator and VCA channels
        SELECTED = 0,
        BASE_VOL,
        LIMIT_VOL,
        ATTACK_TIME,
        DECAY_TIME,
        SUSTAIN_TIME,
        RELEASE_TIME,
        SUSTAIN_VOL,
        SAWTOOTH_REVERSE,
      // RENDER
        RENDER_ADSR,
        INIT,
        LAST
        };
    }


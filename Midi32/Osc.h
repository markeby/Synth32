//#######################################################################
// Module:     Osc.h
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       3/23/2023
//#######################################################################
#pragma once

#include "config.h"

#define A440                ((4 * NOTES_PER_OCTAVE) + 9)                // A440 = A4

namespace OSC_N
{


//#######################################################################
enum class SHAPE {
    SINE = 0,
    TRIANGLE,
    SQUARE,
    SAWTOOTH,
    PULSE,
    };

enum class STATE {
    IDLE = 0,
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE
    };

enum class D_A_OFF {
    DIR = 0,
    SQUARE,
    SINE,
    PULSE,
    SAWTOOTH,
    TRIANGLE,
    WIDTH,
    EXPO
    };


//#######################################################################
class   SYNTH_OSC_C
    {
private:
    uint16_t OctaveArray[FULL_KEYS];

    int     Valid;
    int     Active;
    int     Number;
    uint8_t CurrentNote;
    bool    TriggerUp;
    bool    TriggerDown;
    uint8_t OscChannel;
    uint8_t PwmChannel;

    typedef struct
        {
        uint8_t     Channel;            // I2C device index
        uint16_t    CurrentLevel;       // Current setting 12 bit D/A
        uint16_t    LimitLevel;         // Maximum value for channel as 12 bit D/A
        float       AttackTime;         // Attack time in mSec.
        float       DecayTime;          // Decay time to sustatin level in mSec.
        uint16_t    DacayTargetLevel;   // Target for end of decay stage
        uint16_t    SustainLevel;       // Sustain level in channel as 12 bit D/A
        float       SustainTime;        // How long to hold the sustain level (-1 = hold with key down)
        float       ReleaseTime;        // How long to devel 0 in mSec.

        bool        Active;             // Mixer channel is active
        bool        Change;             // Mixer in change state
        float       Timer;              // Timer loaded with state time and descrimented
        STATE       State;              // Current state of this mixer channel
        String      Name;
        } MIXER_T;

    MIXER_T     Mix[OSC_MIXER_COUNT];

    void  ClearState (void);

public:
         SYNTH_OSC_C     (void);
    void Begin           (int num, uint8_t first_device);
    bool Loop            (void);
    void SetTuning       (void);
    void NoteSet         (uint8_t key, uint8_t velocity);
    void NoteClear       ();
    void Clear           (void);
    void SetAttack       (uint8_t wave, float time);
    void SetDecay        (uint8_t wave, float time);
    void SetRelease      (uint8_t wave, float time);
    void SetSustainLevel (uint8_t wave, float level_percent);
    void SetSustainTime  (uint8_t wave, float time);
    void SetMaxLevel     (uint8_t wave, float level_percent);
    };
}



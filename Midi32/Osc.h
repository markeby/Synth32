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
    SAWTOOTH,
    PULSE,
    SQUARE,
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
    SINE,
    SQUARE,
    PULSE,
    SAWTOOTH,
    TRIANGLE,
    WIDTH,
    EXPO
    };

#define TUNING_WAVES_SHAPE      (SHAPE::SAWTOOTH)

}// end namespace OSC_N

//#######################################################################
class   SYNTH_OSC_C
    {
private:
    uint16_t OctaveArray[FULL_KEYS];

    int     Valid;
    int     Active;
    int     Number;
    byte    CurrentNote;
    bool    TriggerUp;
    bool    TriggerDown;
    byte    OscChannel;
    byte    PwmChannel;

    typedef struct
        {
        byte            Channel;            // I2C device index
        uint16_t        CurrentLevel;       // Current setting 12 bit D/A
        uint16_t        LimitLevel;         // Maximum value for channel as 12 bit D/A
        float           AttackTime;         // Attack time in mSec.
        float           DecayTime;          // Decay time to sustatin level in mSec.
        uint16_t        DacayTargetLevel;   // Target for end of decay stage
        uint16_t        SustainLevel;       // Sustain level in channel as 12 bit D/A
        float           SustainTime;        // How long to hold the sustain level (-1 = hold with key down)
        float           ReleaseTime;        // How long to devel 0 in mSec.

        bool            Active;             // Mixer channel is active
        bool            Change;             // Mixer in change state
        float           Timer;              // Timer loaded with state time and descrimented
        OSC_N::STATE    State;              // Current state of this mixer channel
        String      Name;
        } MIXER_T;

    MIXER_T     Mix[OSC_MIXER_COUNT];
    byte        SawtoothDirChannel;

    void  ClearState (void);

public:
         SYNTH_OSC_C     (void);
    void Begin           (int num, uint8_t first_device);
    bool Loop            (void);
    void SetTuning       (void);
    void NoteSet         (byte key, uint8_t velocity);
    void NoteClear       ();
    void Clear           (void);
    void SetSawReverse   (bool data);
    void SetAttackTime   (byte wave, float time);
    void SetDecayTime    (byte wave, float time);
    void SetSustainTime  (byte wave, float time);
    void SetReleaseTime  (byte wave, float time);
    void SetSustainLevel (byte wave, float level_percent);
    void SetMaxLevel     (byte wave, float level_percent);
    };



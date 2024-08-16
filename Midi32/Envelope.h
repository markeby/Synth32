//#######################################################################
// Module:     Envelope.h
// Descrption: Envelope processor
// Creator:    markeby
// Date:       6/25/2024
//#######################################################################
#pragma once
#include <deque>
#include "config.h"

//#######################################################################
enum class ESTATE {
    IDLE = 0,
    START,
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE
    };

//#######################################################################
class ENVELOPE_C
    {
private:
    // State change
    int         Active;
    bool        TriggerEnd;

    // Control state
    uint8_t&    UseCount;       // increment started and decriment as idle
    ESTATE      State;          // Current state of this mixer channel
    float       Timer;          // Timer loaded with state time and descrimented
    float       TargetTime;     // Timer is incrimented until this time is exceeded
    float       Current;        // Current level zero to one
    float       Target;
    float       Peak;           // Fraction of one (percent)
    float       Sustain;        // Sustain level up to one
    float       ReleaseSt;      // Start of release volume level
    bool        Updated;        // Flag indicating update output
    bool        PeakLevel;      // Flag indicating sustain and peak are the same
    bool        UseSoftLFO;     // Flag to enable sofware LFO

    int16_t     DiffLevel;      // Difference to new level

    float       AttackTime;     // Attack time in uSec.
    float       DecayTime;      // Decay time to sustatin level in uSec.
    float       SustainTime;    // How long to hold the sustain level (-1 = hold while active)
    float       ReleaseTime;    // How long to end back at base level in uSec.
    float       Multiplier;     // Final output adjustmet

    // Fixed parameters
    String      Name;
    uint8_t     Index;
    uint16_t    DeviceChannel;
    float       Floor;
    float       Diff;


public:
             ENVELOPE_C     (uint8_t index, String name, uint16_t device, uint8_t& usecount);
            ~ENVELOPE_C     (void)      {}
    void     Clear          (void);
    void     SetRange       (int16_t floor, int16_t ceiling);
    void     Process        (float deltaTime);
    void     Update         (void);
    void     Start          (void);
    void     End            (void);
    void     SetTime        (ESTATE state, float time);
    void     SetLevel       (ESTATE state, float percent);

    inline void     OutputMultiplier (float val)        { Multiplier = val; }           // Initialize adjustment to output value
    inline void     SetSoftLFO       (bool state)       { UseSoftLFO = state; }         // Enable software sine wave application
    inline uint16_t GetChannel       (void)             { return (DeviceChannel); }     // Return D/A channel number
    };  // end ENVELOPE_C

//#######################################################################
class ENVELOPE_GENERATOR_C
    {
private:
    std::deque<ENVELOPE_C>  Envelopes;

public:
                ENVELOPE_GENERATOR_C    (void);
                ~ENVELOPE_GENERATOR_C   (void)   {}
    ENVELOPE_C*  NewADSR                (uint8_t index, String name, uint16_t device, uint8_t& usecount);
    void         Loop                   (void);
    };


//#######################################################################
// Module:     Osc.h
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       3/23/2023
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

    // Current state
    ESTATE      State;          // Current state of this mixer channel
    float       Timer;          // Timer loaded with state time and descrimented
    float       TargetTime;     // Timer is incrimented until this time is exceeded
    uint16_t    CurrentLevel;   // Current setting 12 bit D/A
    uint16_t    StartLevel;     // start level for current state
    uint16_t    TargetLevel;    // Target for current state

    // State control settings
    uint16_t    BaseLevel;      // start and end levels as 12 bit D/A
    uint16_t    PeakLevel;      // Attacl value for channel as 12 bit D/A
    uint16_t    SustainLevel;   // Sustain level in channel as 12 bit D/A
    float       AttackTime;     // Attack time in uSec.
    float       DecayTime;      // Decay time to sustatin level in uSec.
    float       SustainTime;    // How long to hold the sustain level (-1 = hold while active)
    float       ReleaseTime;    // How long to end back at base level in uSec.

    // Fixed parameters
    String          Name;
    byte            DeviceChannel;

    void  ClearState (void)
        {
        Active        = false;
        TriggerEnd    = false;
        State         = ESTATE::IDLE;
        CurrentLevel  = BaseLevel;
        }

public:
         ENVELOPE_C         (String name, int16_t device);
        ~ENVELOPE_C         (void)   {}
    void Process            (float deltaTime);
    void Start              (void);
    void End                (void);
    void SetTime            (ESTATE state, float time);
    void SetLevel           (ESTATE state, float percent);
    void SetLevel           (ESTATE state, uint16_t davalue);
    };  // end ENVELOPE_C

class ENVELOPE_GENERATOR_C
    {
private:
    std::deque<ENVELOPE_C>  Envelopes;

public:
                ENVELOPE_GENERATOR_C    (void);
                ~ENVELOPE_GENERATOR_C   (void)   {}
    ENVELOPE_C&  NewADSR                (String name, uint16_t device);
    void         Loop                   (void);
    };

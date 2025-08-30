//#######################################################################
// Module:     Envelope.h
// Descrption: Envelope processor
// Creator:    markeby
// Date:       6/25/2024
//#######################################################################
#pragma once
#include <deque>
#include "Config.h"

//#######################################################################
enum class ESTATE
    {
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

    // Runtime state
    byte&       UseCount;       // increment started and decriment as idle
    ESTATE      State;          // Current state of this mixer channel

    bool        Updated;        // Flag indicating update output
    bool        PeakLevel;      // Flag indicating sustain and peak are the same
    bool        UseSoftLFO;     // Flag to enable sofware LFO

    // User supplied inputs
    float       Top;            // Fraction of one (percent).
    float       Bottom;         // Fraction of one (percent).
    float       SetSustain;     // The settin of sustain level up to one.
    float       AttackTime;     // Attack time in uSec.
    float       DecayTime;      // Decay time to sustatin level in uSec.
    float       ReleaseTime;    // How long to end back at base level in uSec.

    // runtime calculations
    float       Delta;          // Distance for the current state.
    float       Sustain;        // The usable Sustain level up to one
    bool        NoDecay;        // Decay time set so low that there is no decay.  Sustain serves no purpose then.
    float       Timer;          // Timer loaded with state time
    float       TargetTime;     // Timer is incrimented until this time is exceeded
    float       Current;        // Current level zero to one
    float       Target;

    // Fixed parameters at initialization
    String      Name;
    byte        Index;
    uint16_t    DevicePortIO;

public:
            ENVELOPE_C      (uint8_t index, String name, uint16_t device, uint8_t& usecount);
    void    Clear           (void);
    void    Process         (float deltaTime);
    void    SetCurrent      (float data);
    void    SetOverride     (uint32_t data);
    void    Update          (void);
    void    Start           (void);
    void    End             (void);
    void    SetTime         (ESTATE state, float time);
    float   GetTime         (ESTATE state);
    void    SetLevel        (ESTATE state, float percent);
    float   GetLevel        (ESTATE state);
    void    SetSoftLFO      (bool sel);

    inline uint16_t GetPortIO (void)                 // Return D/A channel number
        { return (DevicePortIO); }

    inline int IsActive (void)
        { return (Active); }
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


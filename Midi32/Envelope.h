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

enum class DAMPER : byte
    {
    OFF = 0,
    NORMAL,
    INVERT,
    MAX
    };

//#######################################################################
class ENVELOPE_C
    {
private:
    // State change
    int         _Active;
    bool        _TriggerEnd;

    // Runtime state
    byte&       _UseCount;      // increment started and decriment as idle
    ESTATE      _State;         // Current state of this mixer channel

    bool        _Muted;         // Do not respond to Start directive
    bool        _Updated;       // Flag indicating update output
    bool        _PeakLevel;     // Flag indicating sustain and peak are the same

    // User supplied inputs
    bool        _DualUse;       // Dual usage flag  (false = VCA,  true = VCF,other)
    bool        _UseSoftLFO;    // Flag to enable sofware LFO
    float       _ScaleLFO;      // Scale reduction multiplier for LFO
    DAMPER      _DamperMode;    // Mode to utilize string damper
    float       _Top;           // Fraction of one (percent).
    float       _Bottom;        // Fraction of one (percent).
    float       _SetSustain;    // The settin of sustain level up to one.
    float       _AttackTime;    // Attack time in uSec.
    float       _DecayTime;     // Decay time to sustatin level in uSec.
    float       _ReleaseTime;   // How long to end back at base level in uSec.
    float       _Expression;    // Final volume multiplier
    bool        _Damper;        // state of damper pedal


    // runtime calculations
    float       _Delta;         // Distance for the current state.
    float       _Sustain;       // The usable Sustain level up to one
    bool        _NoDecay;       // Decay time set so low that there is no decay.  Sustain serves no purpose then.
    float       _Timer;         // Timer loaded with state time
    float       _TargetTime;    // Timer is incrimented until this time is exceeded
    float       _Current;       // Current level zero to one
    float       _Target;

    // Fixed parameters at initialization
    String      _Name;
    byte        _Index;
    uint16_t    _DevicePortIO;

public:
                ENVELOPE_C      (uint8_t index, String name, uint16_t device, uint8_t& usecount);
    void        Clear           (void);
    void        Mute            (bool state);
    void        Expression      (float level)       { _Expression = level; }
    void        Damper          (bool state)        { _Damper = state; }
    void        Process         (float deltaTime);
    void        SetCurrent      (float data);
    void        SetOverride     (uint16_t data);
    void        Update          (void);
    void        Start           (void);
    void        Start           (bool modstate)     { _UseSoftLFO = modstate; _ScaleLFO = 0.2; Start (); }
    void        End             (void);
    void        SetTime         (ESTATE state, float time);
    float       GetTime         (ESTATE state);
    void        SetLevel        (ESTATE state, float percent);
    float       GetLevel        (ESTATE state);
    void        SetSoftLFO      (bool sel);
    void        SetDamperMode   (DAMPER mode)   { _DamperMode = mode; }
    void        SetDualUse      (bool sel)      { _DualUse = sel; }
    uint16_t    GetPortIO       (void)          { return (_DevicePortIO); }  // Return D/A channel number

    int IsActive (void)
        { return (_Active); }
    };  // end ENVELOPE_C

//#######################################################################
class ENV_GENERATOR_C
    {
private:
    std::deque<ENVELOPE_C>  _Envelopes;

public:
                ENV_GENERATOR_C     (void);
    ENVELOPE_C*  NewADSR            (uint8_t index, String name, uint16_t device, uint8_t& usecount);
    void         Loop               (void);
    };


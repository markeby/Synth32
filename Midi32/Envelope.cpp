//#######################################################################
// Module:     Osc.cpp
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include "config.h"
#include "Envelope.h"

using namespace std;

#define DEBUG(args...) {if (DebugSynth) { printf ("[ENV-%d]{%s} - ",Index,Name);printf(args);printf("\n");}}
#define EDEBUG(args...) {if (DebugSynth) { printf ("[ENV=%d]{%s} - %s - ",Index,Name,StateLabel[(int)State]);printf(args);printf("\n");}}

String StateLabel[] = { "IDLE", "START", "ATTACK", "DECAY", "SUSTAIN", "RELEASE" };
#define TIME_THRESHOLD  10.0

//#######################################################################
ENVELOPE_GENERATOR_C::ENVELOPE_GENERATOR_C ()
    {
    }

//#######################################################################
ENVELOPE_C*  ENVELOPE_GENERATOR_C::NewADSR (ETYPE etype, byte index, String name, uint16_t device, byte& usecount)
    {
    ENVELOPE_C adsl (etype, index, name, device, usecount);
    Envelopes.push_back (adsl);
    return (&(Envelopes.back ()));
    }

//#######################################################################
void  ENVELOPE_GENERATOR_C::Loop ()
    {
    for ( deque<ENVELOPE_C>::iterator it = Envelopes.begin ();  it != Envelopes.end();  ++it )
        {
        it->Process (DeltaTime);
        it->Update ();
        }
    I2cDevices.UpdateDigital ();
    I2cDevices.UpdateAnalog  ();     // Update D/A ports
    }
//#######################################################################
ENVELOPE_C::ENVELOPE_C (ETYPE etype, byte index, String name, int16_t device, byte& usecount) : UseCount (usecount)
    {
    Name            = name;
    DeviceChannel   = device;
    Type            = etype;
    Index           = index;
    CurrentLevel    = 0;
    BaseLevel       = 0;
    PeakLevel       = 0;
    SustainLevel    = 0;
    AttackTime      = 0;
    DecayTime       = 0;
    SustainTime     = 0;
    ReleaseTime     = 0;
    ClearState  ();
    }

//#######################################################################
byte ENVELOPE_C::GetChannel ()
    {
    return (DeviceChannel);
    }

//#######################################################################
void ENVELOPE_C::SetTime (ESTATE state, float time)
    {
    switch (state )
        {
        case ESTATE::ATTACK:
            AttackTime = time;
            break;
        case ESTATE::DECAY:
            DecayTime = time;
            break;
        case ESTATE::SUSTAIN:
            SustainTime = time;
            break;
        case ESTATE::RELEASE:
            ReleaseTime = time;
            break;
        }
    DEBUG ("%s - Time setting > %f uSec", StateLabel[(int)state], time );
    }

//#######################################################################
void ENVELOPE_C::SetLevel (ESTATE state, float percent)
    {
    uint16_t da = percent * DA_RANGE;
    if ( da > MAX_DA )
        da = MAX_DA;

    switch ( state )
        {
        case ESTATE::START:
            if ( Type == ETYPE::VCA )
                return;
            BaseLevel = da;
            break;
        case ESTATE::ATTACK:
            PeakLevel = da;
            break;
        case ESTATE::DECAY:
            SustainLevel = da;
            break;
        case ESTATE::SUSTAIN:
            SustainLevel = da;
            break;
        case ESTATE::RELEASE:
            BaseLevel = da;
            break;
        }
    DEBUG ("%s - level setting > %d", StateLabel[(int)state], da );
    }

//#######################################################################
void ENVELOPE_C::SetLevel (ESTATE state, uint16_t davalue)
    {
    if ( davalue > MAX_DA )
        davalue = MAX_DA;

    switch ( state )
        {
        case ESTATE::START:
            BaseLevel = davalue;
            break;
        case ESTATE::ATTACK:
            PeakLevel = davalue;
            break;
        case ESTATE::DECAY:
            SustainLevel = davalue;
            break;
        case ESTATE::SUSTAIN:
            SustainLevel = davalue;
            break;
        case ESTATE::RELEASE:
            BaseLevel = davalue;
            break;
        }

    DEBUG ("%s - discrete level setting > %d", StateLabel[(int)state], davalue );
    }

//#######################################################################
void ENVELOPE_C::Start ()
    {
    if ( Active )
        return;
    if ( (Type == ETYPE::VCA) && (PeakLevel == 0) )
        return;
    Active = true;
    State = ESTATE::START;
    UseCount++;
    }

//#######################################################################
void ENVELOPE_C::End ()
    {
     if ( !Active )
         return;
     TriggerEnd = true;
    }

//#######################################################################
void ENVELOPE_C::ClearState ()
    {
    Active        = false;
    TriggerEnd    = false;
    State         = ESTATE::IDLE;
    CurrentLevel  = BaseLevel;
    if ( UseCount )     UseCount--;
    }

//#######################################################################
void ENVELOPE_C::Update ()
    {
    I2cDevices.D2Analog (DeviceChannel, CurrentLevel);
    }

//#######################################################################
//#######################################################################
void ENVELOPE_C::Process (float deltaTime)
    {
    if ( !Active )                      // if we ain't doing it then we don't need to run this.
        return;

    //***************************************
    //  Fix any levels exceeding paramters
    //***************************************
    if ( BaseLevel < PeakLevel )        // sanity checking
        {
        if ( CurrentLevel > PeakLevel )
            CurrentLevel = PeakLevel;
        if ( CurrentLevel < BaseLevel )
            CurrentLevel = BaseLevel;
        }
    else
        {
        if ( CurrentLevel > BaseLevel )
            CurrentLevel = BaseLevel;
        if ( CurrentLevel < PeakLevel )
            CurrentLevel = PeakLevel;
        }

    //***************************************
    //  Beginning of the end
    //***************************************
    if ( TriggerEnd && (State != ESTATE::RELEASE) )
        {
        State = ESTATE::RELEASE;
        StartLevel = CurrentLevel;
        Timer = ReleaseTime;
        EDEBUG ("Terminating");
        return;
        }

    switch ( State )
        {
        //***************************************
        //  Start envelope
        //***************************************
        case ESTATE::START:
            {
            CurrentLevel = BaseLevel;
            Timer        = 0.0;
            TargetTime   = AttackTime - TIME_THRESHOLD;
            State        = ESTATE::ATTACK;
            EDEBUG ("Starting");
            return;
            }
        //***************************************
        //  ATTACK
        //***************************************
        case ESTATE::ATTACK:
            {
            if ( Timer < TargetTime )
                {
                Timer  += deltaTime;
                CurrentLevel  = (Timer / AttackTime) * (PeakLevel - BaseLevel);
                EDEBUG ("Timer > %f uSec at level %d\n", Timer, CurrentLevel);
                return;
                }
            CurrentLevel = PeakLevel;
            Timer        = 0.0;
            State        = ESTATE::DECAY;
            TargetTime   = DecayTime - TIME_THRESHOLD;
            EDEBUG ("Start > %f  uSec from level %d to %d\n", DecayTime, CurrentLevel, SustainLevel);
            return;
            }

        //***************************************
        //  DECAY
        //***************************************
        case ESTATE::DECAY:
            {
            if ( Timer > 10 )
                {
                Timer -= deltaTime;
                CurrentLevel = (Timer / DecayTime) * (PeakLevel - SustainLevel);
                EDEBUG ("Timer > %f uSec at level %d\n", Timer, CurrentLevel);
                return;
                }
            CurrentLevel = SustainLevel;
            Timer        = 0.0;
            TargetTime   = SustainTime - TIME_THRESHOLD;
            State        = ESTATE::SUSTAIN;
            if ( DebugSynth )
                {
                if ( SustainTime < 0 )
                    {
                    EDEBUG ("Staying at level %d\n", CurrentLevel);
                    }
                else
                    {
                    EDEBUG ("%f uSec at level %d\n", Timer, CurrentLevel);
                    }
                }
            return;
            }
        //***************************************
        //  SUSTAIN
        //***************************************
        case ESTATE::SUSTAIN:
            {
            if ( Timer <= 0 )
                return;
            if ( Timer > 10 )
                {
                Timer -= deltaTime;
                EDEBUG ("Timer > %f uSec at level %d\n", Timer, CurrentLevel);
                return;
                }
            StartLevel = CurrentLevel;
            Timer      = 0.0;
            TargetTime = ReleaseTime - TIME_THRESHOLD;
            State      = ESTATE::RELEASE;
            EDEBUG ("Start > %f\n", Timer);
            return;
            }
        //***************************************
        //  RELEASE
        //***************************************
        case ESTATE::RELEASE:
            {
            TriggerEnd = false;
            if ( Timer > 10)
                {
                Timer -= deltaTime;
                CurrentLevel = (Timer / ReleaseTime) * (StartLevel - BaseLevel);
                EDEBUG ("Timer > %f uSec at level %d\n", Timer, CurrentLevel);
                return;
                }
            ClearState ();          // We got to here so this envelope process in finished.
            return;
            }
        }
    //***************************************
    //  This should never happen
    //***************************************
    EDEBUG ("DANGER! DANGER!We should have never gotten here during environment processing!");
    ClearState ();
    }



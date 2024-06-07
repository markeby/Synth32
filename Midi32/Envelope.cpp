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

#include "Debug.h"
static const char* Label = "ENV";
#define DBG(args...)  {if (DebugSynth){DebugMsgF(Label,Index,Name,StateLabel[(int)State],args); } }

char* StateLabel[] = { "IDLE", "START", "ATTACK", "DECAY", "SUSTAIN", "RELEASE" };
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
        switch ( it->WhatType () )
            {
            case ETYPE::VCA:
                it->ProcessVCA (DeltaTimeMilli);
                break;
            case ETYPE::VCF:
                it->ProcessVCF (DeltaTimeMilli);
                break;
            case ETYPE::VCO:
                break;
            }
        it->Update ();
        }
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
uint16_t ENVELOPE_C::GetChannel ()
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
    DBG ("%s - Time setting > %f uSec", StateLabel[(int)state], time );
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
    DBG ("setting > %d", da );
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

    DBG ("discrete level setting > %d", davalue );
    }

//#######################################################################
void ENVELOPE_C::Start ()
    {
    if ( Active || (PeakLevel == 0) )
        return;
    Active = true;
    State = ESTATE::START;
    UseCount++;
    DBG ("starting %d", Index);
    }

//#######################################################################
void ENVELOPE_C::End ()
    {
    if ( !Active )
        return;
    TriggerEnd = true;
    State = ESTATE::IDLE;
    }

//#######################################################################
void ENVELOPE_C::ClearState ()
    {
    if ( Active && UseCount )   UseCount--;
    Active        = false;
    TriggerEnd    = false;
    State         = ESTATE::IDLE;
    CurrentLevel  = BaseLevel;
    DBG ("clearing %d", Index);
    Update ();
    }

//#######################################################################
void ENVELOPE_C::Clear ()
    {
    ClearState ();
    }

//#######################################################################
void ENVELOPE_C::Update ()
    {
    I2cDevices.D2Analog (DeviceChannel, CurrentLevel);
    }

//#######################################################################
//#######################################################################
void ENVELOPE_C::ProcessVCA (float deltaTime)
    {
    if ( !Active )                      // if we ain't doing it then we don't need to run this.
        return;

    //***************************************
    //  Fix  level exceeding paramter
    //***************************************
    if ( CurrentLevel > PeakLevel )
        CurrentLevel = PeakLevel;

    //***************************************
    //  Beginning of the end
    //***************************************
    if ( TriggerEnd && (State != ESTATE::RELEASE) )
        {
        State = ESTATE::RELEASE;
        StartLevel = CurrentLevel;
        Timer = ReleaseTime;
        DBG ("Terminating");
        return;
        }

    switch ( State )
        {
        //***************************************
        //  Start envelope
        //***************************************
        case ESTATE::START:
            {
            CurrentLevel = 0;
            Timer        = 0.0;
            TargetTime   = AttackTime - TIME_THRESHOLD;
            State        = ESTATE::ATTACK;
            DBG ("Start > %f  uSec from level %d to %d", TargetTime, CurrentLevel, PeakLevel);
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
                CurrentLevel  = (Timer / AttackTime) * PeakLevel;
                DBG ("Timer > %f uSec at level %d", Timer, CurrentLevel);
                return;
                }
            CurrentLevel = PeakLevel;
            Timer        = DecayTime - TIME_THRESHOLD;;
            State        = ESTATE::DECAY;
            DiffLevel    = PeakLevel - SustainLevel;
            TargetTime   = 0.0;
            DBG ("Start > %f  uSec from level %d to %d", Timer, CurrentLevel, SustainLevel);
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
                CurrentLevel = SustainLevel + ((Timer / DecayTime) * DiffLevel);
                DBG ("Timer > %f uSec at level %d", Timer, CurrentLevel);
                return;
                }
            CurrentLevel = SustainLevel;
            Timer        = SustainTime - TIME_THRESHOLD;
            State        = ESTATE::SUSTAIN;
            if ( DebugSynth )
                {
                if ( SustainTime < 0 )
                    {
                    DBG ("Staying at level %d", CurrentLevel);
                    }
                else
                    {
                    DBG ("%f uSec at level %d", Timer, CurrentLevel);
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
            if ( Timer > TIME_THRESHOLD )
                {
                Timer -= deltaTime;
                DBG ("Timer > %f uSec at level %d", Timer, CurrentLevel);
                return;
                }
            StartLevel = CurrentLevel;
            Timer      = ReleaseTime - TIME_THRESHOLD;
            State      = ESTATE::RELEASE;
            DBG ("Start > %f  uSec from level %d to 0", Timer, CurrentLevel);
            return;
            }
        //***************************************
        //  RELEASE
        //***************************************
        case ESTATE::RELEASE:
            {
            TriggerEnd = false;
            if ( Timer > 20)
                {
                Timer -= deltaTime;
                CurrentLevel = (Timer / ReleaseTime) * (StartLevel - BaseLevel);
                DBG ("Timer > %f uSec at level %d", Timer, CurrentLevel);
                return;
                }
            ClearState ();          // We got to here so this envelope process in finished.
            return;
            }
        }
    //***************************************
    //  This should never happen
    //***************************************
    DBG ("DANGER! DANGER!We should have never gotten here during environment processing!");
    ClearState ();
    }

//#######################################################################
//#######################################################################
void ENVELOPE_C::ProcessVCF (float deltaTime)
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
        DBG ("Terminating");
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
            DBG ("Starting");
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
                DBG ("Timer > %f uSec at level %d", Timer, CurrentLevel);
                return;
                }
            CurrentLevel = PeakLevel;
            Timer        = DecayTime;
            State        = ESTATE::DECAY;
            TargetTime   = DecayTime - TIME_THRESHOLD;
            DBG ("Start > %f  uSec from level %d to %d", DecayTime, CurrentLevel, SustainLevel);
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
                DBG ("Timer > %f uSec at level %d", Timer, CurrentLevel);
                return;
                }
            CurrentLevel = SustainLevel;
            TargetTime   = SustainTime - TIME_THRESHOLD;
            Timer        = TargetTime;
            State        = ESTATE::SUSTAIN;
            if ( DebugSynth )
                {
                if ( SustainTime < 0 )
                    {
                    DBG ("Staying at level %d", CurrentLevel);
                    }
                else
                    {
                    DBG ("%f uSec at level %d", Timer, CurrentLevel);
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
            if ( Timer > TIME_THRESHOLD )
                {
                Timer -= deltaTime;
                DBG ("Timer > %f uSec at level %d\, Timer, CurrentLevel");
                return;
                }
            StartLevel = CurrentLevel;
            TargetTime = ReleaseTime - TIME_THRESHOLD;
            Timer      = TargetTime;
            State      = ESTATE::RELEASE;
            DBG ("Start > %f, Timer");
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
                DBG ("Timer > %f uSec at level %d", Timer, CurrentLevel);
                return;
                }
            ClearState ();          // We got to here so this envelope process in finished.
            return;
            }
        }
    //***************************************
    //  This should never happen
    //***************************************
    DBG ("DANGER! DANGER!We should have never gotten here during environment processing!");
    ClearState ();
    }


//#######################################################################
// Module:     Envelope.cpp
// Descrption: Envelope processor
// Creator:    markeby
// Date:       6/25/2024
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
ENVELOPE_C* ENVELOPE_GENERATOR_C::NewADSR (byte index, String name, uint16_t device, byte& usecount)
    {
    ENVELOPE_C adsl (index, name, device, usecount);
    Envelopes.push_back (adsl);
    return (&(Envelopes.back ()));
    }

//#######################################################################
void ENVELOPE_GENERATOR_C::Loop ()
    {
    for ( deque<ENVELOPE_C>::iterator it = Envelopes.begin ();  it != Envelopes.end();  ++it )
        {
        it->Process (DeltaTimeMilli);
        it->Update ();
        }
    }

//#######################################################################
void ENVELOPE_C::SetRange (int16_t floor, int16_t ceiling)
    {
    Floor = floor;
    Diff  = ceiling - floor;
    }

//#######################################################################
ENVELOPE_C::ENVELOPE_C (byte index, String name, uint16_t device, byte& usecount) : UseCount(usecount)
    {
    Name            = name;
    DeviceChannel   = device;
    Index           = index;
    Current         = 0;
    Peak            = 0;
    Sustain         = 0;
    AttackTime      = 0;
    DecayTime       = 0;
    SustainTime     = 0;
    ReleaseTime     = 0;
    SetRange (0, DA_MAX);        // Defualt to normal D/A converter settings
    Clear ();
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
    switch ( state )
        {
        case ESTATE::START:
            break;
        case ESTATE::ATTACK:
            Peak = percent;
            break;
        case ESTATE::DECAY:
            Sustain = percent;
            break;
        case ESTATE::SUSTAIN:
            Sustain = percent;
            break;
        case ESTATE::RELEASE:
            break;
        }
    DBG ("setting %s > %f", StateLabel[(int)state], percent );
    }

//#######################################################################
void ENVELOPE_C::Start ()
    {
    if ( Active || (Peak == 0.0) )
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
void ENVELOPE_C::Clear ()
    {
    if ( Active && UseCount )   UseCount--;
    Active        = false;
    TriggerEnd    = false;
    State         = ESTATE::IDLE;
    Current       = 0.0;
    Updated       = true;
    DBG ("clearing %d", Index);
    Update ();
    }

//#######################################################################
void ENVELOPE_C::Update ()
    {
    if ( Updated )
        {
        if ( Current > Peak )
            Current = Peak;
        int16_t z = abs (Floor + (Diff * Current));
        I2cDevices.D2Analog (DeviceChannel, z);
        Updated = false;
        }
    }

//#######################################################################
//#######################################################################
void ENVELOPE_C::Process (float deltaTime)
    {
    if ( !Active )                      // if we ain't doing it then we don't need to run this.
        return;

    //***************************************
    //  Beginning of the end
    //***************************************
    if ( TriggerEnd && (State != ESTATE::RELEASE) )
        {
        State = ESTATE::RELEASE;
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
            Current     = 0.0;
            Timer       = 0.0;
            TargetTime  = AttackTime - TIME_THRESHOLD;
            State       = ESTATE::ATTACK;
            DBG ("Start attact to %f uSec from level %f to %f", TargetTime, Current, Peak);
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
                Current  = (Timer / TargetTime) * Peak;
                Updated = true;
                DBG ("Timer > %f uSec at level %f", Timer, Current);
                return;
                }
            Current     = Peak;
            Updated     = true;
            Timer       = DecayTime - TIME_THRESHOLD;;
            State       = ESTATE::DECAY;
            Target      = Peak - Sustain;
            TargetTime  = 0.0;
            DBG ("Start > %f  uSec from level %f to %f", Timer, Current, Sustain);
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
                Current = Sustain + ((Timer / DecayTime) * Target);
                Updated = true;
                DBG ("Timer > %f uSec at level %f", Timer, Current);
                return;
                }
            Current = Sustain;
            Updated = true;
            Timer   = SustainTime - TIME_THRESHOLD;
            State   = ESTATE::SUSTAIN;
            if ( DebugSynth )
                {
                if ( SustainTime < 0 )
                    { DBG ("Staying at level %f", Current); }
                else
                    { DBG ("%f uSec at level %f", Timer, Current); }
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
                DBG ("Timer > %f uSec at level %f", Timer, Current);
                return;
                }
            Timer      = ReleaseTime - TIME_THRESHOLD;
            State      = ESTATE::RELEASE;
            DBG ("Start > %f  uSec from level %f to 0", Timer, Current);
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
                Timer  -= deltaTime;
                Current = (Timer / ReleaseTime) * Sustain;
                Updated = true;
                DBG ("Timer > %f uSec at level %f", Timer, Current);
                return;
                }
            Clear ();          // We got to here so this envelope process in finished.
            return;
            }
        }
    //***************************************
    //  This should never happen
    //***************************************
    DBG ("DANGER! DANGER!We should have never gotten here during environment processing!");
    Clear ();
    }

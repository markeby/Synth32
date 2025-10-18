//#######################################################################
// Module:     Envelope.cpp
// Descrption: Envelope processor
// Creator:    markeby
// Date:       6/25/2024
//#######################################################################
#include <Arduino.h>
#include "Envelope.h"
#include "SoftLFO.h"

using namespace std;

#include "Debug.h"

#ifdef DEBUG_SYNTH
static const char* Label = "ENV";
#define DBG(args...)  {if (DebugSynth){DebugMsgF(Label,Index,Name,StateLabel[(int)State],args); } }
#else
#define DBG(args...)
#endif

char* StateLabel[] = { "IDLE", "START", "ATTACK", "DECAY", "SUSTAIN", "RELEASE" };
#define TIME_THRESHOLD  0.0

//#######################################################################
ENV_GENERATOR_C::ENV_GENERATOR_C ()
    {
    }

//#######################################################################
ENVELOPE_C* ENV_GENERATOR_C::NewADSR (uint8_t index, String name, uint16_t device, uint8_t& usecount)
    {
    ENVELOPE_C adsl (index, name, device, usecount);
    Envelopes.push_back (adsl);
    return (&(Envelopes.back ()));
    }

//#######################################################################
void ENV_GENERATOR_C::Loop ()
    {
    for ( deque<ENVELOPE_C>::iterator it = Envelopes.begin();  it != Envelopes.end();  ++it )
        {
        if ( it->IsActive () )                      // if we ain't doing it then we don't need to run this.
            {
            it->Process (DeltaTimeMilli);
            it->Update ();
            }
        }
    }

//#######################################################################
//#######################################################################
ENVELOPE_C::ENVELOPE_C (uint8_t index, String name, uint16_t device, uint8_t& usecount) : UseCount(usecount)
    {
    Name          = name;
    DevicePortIO  = device;
    Index         = index;
    DualUse       = false;
    Current       = 0;
    Top           = 0;
    Bottom        = 0;
    SetSustain    = 0;
    AttackTime    = 0;
    DecayTime     = 0;
    ReleaseTime   = 0;
    Active        = 0;
    UseSoftLFO    = false;
    Damper        = false;
    Clear ();
    }

//#######################################################################
void ENVELOPE_C::Mute (bool state)
    {
    Muted = state;
    Clear ();
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
        case ESTATE::RELEASE:
            ReleaseTime = time;
            break;
        }
    DBG ("%s - Time setting > %f mSec", StateLabel[(int)state], time );
    }

//#######################################################################
float ENVELOPE_C::GetTime (ESTATE state)
    {
    float val = 0.0;

    switch (state )
        {
        case ESTATE::ATTACK:
            val = AttackTime;
            break;
        case ESTATE::DECAY:
            val = DecayTime;
            break;
        case ESTATE::RELEASE:
            val = ReleaseTime;
            break;
        }
    return (val);
    }

//#######################################################################
void ENVELOPE_C::SetLevel (ESTATE state, float percent)
    {
    String str;

    switch ( state )
        {
        case ESTATE::START:
            str = "BASE";
            Bottom = percent;
            break;
        case ESTATE::ATTACK:
            str = "MAXIMUM";
            Top = percent;
            break;
        case ESTATE::DECAY:
        case ESTATE::SUSTAIN:
            str = "SUSTAIN LEVEL";
            SetSustain = percent;
            break;
        case ESTATE::RELEASE:
            break;
        }
    DBG ("Setting %s > %f", str.c_str (), percent );
    }

//#######################################################################
float ENVELOPE_C::GetLevel (ESTATE state)
    {
    float val = 0.0;

    switch ( state )
        {
        case ESTATE::START:
            val = Bottom;
            break;
        case ESTATE::ATTACK:
            val = Top;
            break;
        case ESTATE::DECAY:
            val = SetSustain;
            break;
        case ESTATE::SUSTAIN:
            val = SetSustain;
            break;
        case ESTATE::RELEASE:
            break;
        }
    return (val);
    }

//#######################################################################
void ENVELOPE_C::SetSoftLFO (bool sel)
    {
    UseSoftLFO = sel;
    DBG ("Toggle %s > %s", Name, (( sel ) ? "ON" : "Off") );
    }


//#######################################################################
void ENVELOPE_C::Start ()
    {
    if ( Active || (Top == 0.0 || Muted ) )
        return;
    Active = true;
    State = ESTATE::START;
    UseCount++;
    DBG ("Starting");
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
    Current       = Bottom;
    Updated       = true;
    DBG ("clearing");
    Update ();
    }

//#######################################################################
void ENVELOPE_C::SetCurrent (float data)
    {
    Current = data;
    short z = (short)(data * (float)DA_MAX);
    I2cDevices.D2Analog (DevicePortIO, z);
    }

//#######################################################################
void ENVELOPE_C::SetOverride (uint32_t data)
    {
    I2cDevices.D2Analog (DevicePortIO, data);
    }

//#######################################################################
void ENVELOPE_C::Update ()
    {
    float output;

    if ( Updated )
        {
        output = Current;
        if ( UseSoftLFO )
            output -= output * SoftLFO.GetSin ();

        int16_t z = (int16_t)((float)DA_MAX * output);
        I2cDevices.D2Analog (DevicePortIO, z);
        Updated = false;
        }
    }

//#######################################################################
//#######################################################################
void ENVELOPE_C::Process (float deltaTime)
    {
    if ( UseSoftLFO )
        Updated = true;

    //***************************************
    //  Beginning of the end
    //***************************************
    if ( TriggerEnd && (State != ESTATE::RELEASE) )
        {
        State   = ESTATE::RELEASE;
        Timer   = ReleaseTime;
        Delta   = Current - Bottom;
        DBG ("%f mSec from level %f to %f", ReleaseTime, Current, Bottom);
        return;
        }

    switch ( State )
        {
        //***************************************
        //  Start envelope
        //***************************************
        case ESTATE::START:
            {
            Current = Bottom;
            Sustain = SetSustain;               // update runtime sustain with sustain as user set
            NoDecay = false;
            if ( DecayTime < 8.0 )
                NoDecay = true;
            else
                {
                if ( Bottom < Top )             // if envelope is going to be running forwards...
                    {
                    // Make sure sustain doesn't exceed top or bottom in the currect direction
//                    if ( (Sustain < Bottom) || (Sustain > Top) )
//                        NoDecay = true;         // This case makes sustain a pointless parameter
                    }
                else
                    {
//                    if ( (Sustain > Bottom) || (Sustain < Top) )
//                        NoDecay = true;         // This case makes sustain a pointless parameter
                     }
                }
            Timer       = 0.0;
            Delta       = Top - Bottom ;
            PeakLevel   = false;
            TargetTime  = AttackTime - TIME_THRESHOLD;
            State       = ESTATE::ATTACK;
            DBG ("Start > %f mSec from level %f to %f", AttackTime, Current, Top);
            return;
            }
        //***************************************
        //  ATTACK
        //***************************************
        case ESTATE::ATTACK:
            {
            Timer += deltaTime;
            if ( Timer < TargetTime )
                {
                Current  = Bottom + ((Timer / TargetTime) * Delta);
                Updated = true;
                DBG ("Timer > %f mSec at level %f", Timer, Current);
                return;
                }
            Current     = Top;
            Updated     = true;
            if ( NoDecay )
                {
                Timer   = 0.0;
                State = ESTATE::SUSTAIN;
                DBG ("Hold at level %f", Current);
                }
            else
                {
                Timer      = DecayTime - TIME_THRESHOLD;;
                State      = ESTATE::DECAY;
                Delta      = Top - Sustain;
                TargetTime = 0.0;
                DBG ("%f mSec from level %f to %f", DecayTime, Current, Sustain);
                }
            return;
            }

        //***************************************
        //  DECAY
        //***************************************
        case ESTATE::DECAY:
            {
            Timer -= deltaTime;
            if ( Timer > 10 )
                {
                Current = Sustain + ((Timer / DecayTime) * Delta);
                Updated = true;
                DBG ("Timer > %f mSec at level %f", Timer, Current);
                return;
                }
            Current = Sustain;
            Updated = true;
            Timer   = 0.0;
            State   = ESTATE::SUSTAIN;

            if ( Sustain >= Top )
                PeakLevel = true;

            DBG ("sustained at level %f", Current);
            return;
            }
        //***************************************
        //  SUSTAIN
        //***************************************
        case ESTATE::SUSTAIN:
            {
            if ( PeakLevel && (Current != Top) && !DualUse )
                {
                Current = Top;
                Updated = true;
                }
            return;
            }
        //***************************************
        //  RELEASE
        //***************************************
        case ESTATE::RELEASE:
            {
            TriggerEnd = false;
            Timer  -= deltaTime;
            if ( Timer > 20)
                {
                Current = Bottom + ((Timer / ReleaseTime) * Delta);
                Updated = true;
                DBG ("Timer > %f mSec at level %f", Timer, Current);
                return;
                }
            Clear ();          // We got to here so this envelope process in finished.
            return;
            }
        }
    //***************************************
    //  This should never happen
    //***************************************
    DBG ("DANGER! DANGER! We should have never gotten here during envelope processing!");
    Clear ();
    }


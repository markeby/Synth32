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
ENVELOPE_GENERATOR_C::ENVELOPE_GENERATOR_C ()
    {
    }

//#######################################################################
ENVELOPE_C* ENVELOPE_GENERATOR_C::NewADSR (uint8_t index, String name, uint16_t device, uint8_t& usecount)
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
    this->Floor = floor;
    this->Diff  = ceiling - floor;
    }

//#######################################################################
ENVELOPE_C::ENVELOPE_C (uint8_t index, String name, uint16_t device, uint8_t& usecount) : UseCount(usecount)
    {
    this->Name          = name;
    this->DevicePortIO = device;
    this->Index         = index;
    this->Current       = 0;
    this->Peak          = 0;
    this->Sustain       = 0;
    this->AttackTime    = 0;
    this->DecayTime     = 0;
    this->ReleaseTime   = 0;
    this->UseSoftLFO    = false;
    this->SetRange (0, DA_MAX);        // Defualt to normal D/A converter settings
    this->Clear    ();
    }

//#######################################################################
void ENVELOPE_C::SetTime (ESTATE state, float time)
    {
    switch (state )
        {
        case ESTATE::ATTACK:
            this->AttackTime = time;
            break;
        case ESTATE::DECAY:
            this->DecayTime = time;
            break;
        case ESTATE::RELEASE:
            this->ReleaseTime = time;
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
            val = this->AttackTime;
            break;
        case ESTATE::DECAY:
            val = this->DecayTime;
            break;
        case ESTATE::RELEASE:
            val = this->ReleaseTime;
            break;
        }
    return (val);
    }

//#######################################################################
void ENVELOPE_C::SetLevel (ESTATE state, float percent)
    {
    switch ( state )
        {
        case ESTATE::START:
            break;
        case ESTATE::ATTACK:
            this->Peak = percent;
            break;
        case ESTATE::DECAY:
            this->Sustain = percent;
            break;
        case ESTATE::SUSTAIN:
            this->Sustain = percent;
            break;
        case ESTATE::RELEASE:
            break;
        }
    DBG ("Setting %s > %f", StateLabel[(int)state], percent );
    }

//#######################################################################
float ENVELOPE_C::GetLevel (ESTATE state)
    {
    float val = 0.0;

    switch ( state )
        {
        case ESTATE::START:
            break;
        case ESTATE::ATTACK:
            val = this->Peak;
            break;
        case ESTATE::DECAY:
            val = this->Sustain;
            break;
        case ESTATE::SUSTAIN:
            val = this->Sustain;
            break;
        case ESTATE::RELEASE:
            break;
        }
    return (val);
    }

//#######################################################################
void ENVELOPE_C::SetSoftLFO (bool sel)
    {
    this->UseSoftLFO = sel;
    DBG ("Toggle %s > %s", this->Name, (( sel ) ? "ON" : "Off") );
    }


//#######################################################################
void ENVELOPE_C::Start ()
    {
    if ( this->Active || (this->Peak == 0.0) )
        return;
    this->Active = true;
    this->State = ESTATE::START;
    this->UseCount++;
    DBG ("Starting %d", this->Index);
    }

//#######################################################################
void ENVELOPE_C::End ()
    {
    if ( !this->Active )
        return;
    this->TriggerEnd = true;
    this->State = ESTATE::IDLE;
    }

//#######################################################################
void ENVELOPE_C::Clear ()
    {
    if ( this->Active && this->UseCount )   this->UseCount--;
    this->Active        = false;
    this->TriggerEnd    = false;
    this->State         = ESTATE::IDLE;
    this->Current       = 0.0;
    this->Updated       = true;
    DBG ("clearing %d", Index);
    this->Update ();
    }

//#######################################################################
void ENVELOPE_C::Update ()
    {
    float output;

    if ( this->Updated )
        {
        if ( this->Current > this->Peak )
            this->Current = this->Peak;

//        output = this->Current * this->Multiplier;
        output = this->Current;
        if ( this->UseSoftLFO )
            output -= output * SoftLFO.GetSin ();

        int16_t z = abs(this->Floor + (this->Diff * output));
        I2cDevices.D2Analog (this->DevicePortIO, z);
        this->Updated = false;
        }
    }

//#######################################################################
//#######################################################################
void ENVELOPE_C::Process (float deltaTime)
    {
    if ( !this->Active )                      // if we ain't doing it then we don't need to run this.
        return;

    if ( this->UseSoftLFO )
        this->Updated = true;

    //***************************************
    //  Beginning of the end
    //***************************************
    if ( this->TriggerEnd && (this->State != ESTATE::RELEASE) )
        {
        this->State     = ESTATE::RELEASE;
        this->Timer     = this->ReleaseTime;
        this->ReleaseSt = this->Current;
        DBG ("Terminating");
        return;
        }

    switch ( this->State )
        {
        //***************************************
        //  Start envelope
        //***************************************
        case ESTATE::START:
            {
            this->Current     = 0.0;
            this->Timer       = 0.0;
            this->PeakLevel   = false;
            this->TargetTime  = this->AttackTime - TIME_THRESHOLD;
            this->State       = ESTATE::ATTACK;
            DBG ("Start > %f mSec from level %f to %f", TargetTime, Current, Peak);
            return;
            }
        //***************************************
        //  ATTACK
        //***************************************
        case ESTATE::ATTACK:
            {
            this->Timer += deltaTime;
            if ( this->Timer < this->TargetTime )
                {
                this->Current  = (this->Timer / this->TargetTime) * this->Peak;
                this->Updated = true;
                DBG ("Timer > %f mSec at level %f", this->Timer, this->Current);
                return;
                }
            this->Current     = this->Peak;
            this->Updated     = true;
            if ( this->DecayTime < 8.0 )
                {
                this->Timer   = 0.0;
                this->State = ESTATE::SUSTAIN;
                }
            else
                {
                this->Timer       = this->DecayTime - TIME_THRESHOLD;;
                this->State       = ESTATE::DECAY;
                this->Target      = this->Peak - this->Sustain;
                this->TargetTime  = 0.0;
                }
            DBG ("Start > %f  mSec from level %f to %f", Timer, Current, Sustain);
            return;
            }

        //***************************************
        //  DECAY
        //***************************************
        case ESTATE::DECAY:
            {
            this->Timer -= deltaTime;
            if ( this->Timer > 10 )
                {
                this->Current = this->Sustain + ((this->Timer / this->DecayTime) * this->Target);
                this->Updated = true;
                DBG ("Timer > %f mSec at level %f", this->Timer, this->Current);
                return;
                }
            this->Current = this->Sustain;
            this->Updated = true;
            this->Timer   = 0.0;
            this->State   = ESTATE::SUSTAIN;
            if ( this->Sustain >= this->Peak )
                this->PeakLevel = true;

            DBG ("sustained at level %f", this->Current);
            return;
            }
        //***************************************
        //  SUSTAIN
        //***************************************
        case ESTATE::SUSTAIN:
            {
            if ( this->PeakLevel && (this->Current != this->Peak) )
                {
                this->Current = this->Peak;
                this->Updated = true;
                }
            return;
            }
        //***************************************
        //  RELEASE
        //***************************************
        case ESTATE::RELEASE:
            {
            this->TriggerEnd = false;
            this->Timer  -= deltaTime;
            if ( Timer > 20)
                {
                this->Current = (this->Timer / this->ReleaseTime) * this->ReleaseSt;
                this->Updated = true;
                DBG ("Timer > %f mSec at level %f", this->Timer, this->Current);
                return;
                }
            Clear ();          // We got to here so this envelope process in finished.
            return;
            }
        }
    //***************************************
    //  This should never happen
    //***************************************
    DBG ("DANGER! DANGER!We should have never gotten here during envelope processing!");
    Clear ();
    }


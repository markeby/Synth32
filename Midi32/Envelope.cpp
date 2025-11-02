//#######################################################################
// Module:     Envelope.cpp
// Descrption: Envelope processor
// Creator:    markeby
// Date:       6/25/2024
//#######################################################################
#include <Arduino.h>

#include "FrontEnd.h"
#include "Envelope.h"
#include "SoftLFO.h"

using namespace std;

#include "Debug.h"

#ifdef DEBUG_SYNTH
static const char* Label = "ENV";
#define DBG(args...)  {if (DebugSynth){DebugMsgF(Label,_Index,_Name,stateLabel[(int)_State],args); } }
#else
#define DBG(args...)
#endif

static char* stateLabel[] = { "IDLE", "START", "ATTACK", "DECAY", "SUSTAIN", "RELEASE" };
#define TIME_THRESHOLD  0.0

//#######################################################################
ENV_GENERATOR_C::ENV_GENERATOR_C ()
    {
    }

//#######################################################################
ENVELOPE_C* ENV_GENERATOR_C::NewADSR (uint8_t index, String name, uint16_t device, uint8_t& usecount)
    {
    ENVELOPE_C adsl (index, name, device, usecount);
    _Envelopes.push_back (adsl);
    return (&(_Envelopes.back ()));
    }

//#######################################################################
void ENV_GENERATOR_C::Loop ()
    {
    for ( deque<ENVELOPE_C>::iterator it = _Envelopes.begin();  it != _Envelopes.end();  ++it )
        {
        if ( it->IsActive () )                      // if we ain't active then we don't need to run this.
            {
            it->Process (DeltaTimeMilli);
            it->Update ();
            }
        }
    }

//#######################################################################
//#######################################################################
ENVELOPE_C::ENVELOPE_C (uint8_t index, String name, uint16_t device, uint8_t& usecount) : _UseCount(usecount)
    {
    _Name         = name;
    _DevicePortIO = device;
    _Index        = index;
    _DualUse      = false;
    _Current      = 0;
    _Top          = 0;
    _Bottom       = 0;
    _SetSustain   = 0;
    _AttackTime   = 0;
    _DecayTime    = 0;
    _ReleaseTime  = 0;
    _Active       = 0;
    _UseSoftLFO   = false;
    _DamperMode   = DAMPER::OFF;
    _Expression   = 1.0;
    Clear ();
    }

//#######################################################################
void ENVELOPE_C::Mute (bool state)
    {
    _Muted = state;
    Clear ();
    }


//#######################################################################
void ENVELOPE_C::SetTime (ESTATE state, float time)
    {
    switch (state )
        {
        case ESTATE::ATTACK:
            _AttackTime = time;
            break;
        case ESTATE::DECAY:
            _DecayTime = time;
            break;
        case ESTATE::RELEASE:
            _ReleaseTime = time;
            break;
        }
    DBG ("%s - Time setting > %f mSec", stateLabel[(int)state], time );
    }

//#######################################################################
float ENVELOPE_C::GetTime (ESTATE state)
    {
    float val = 0.0;

    switch (state )
        {
        case ESTATE::ATTACK:
            val = _AttackTime;
            break;
        case ESTATE::DECAY:
            val = _DecayTime;
            break;
        case ESTATE::RELEASE:
            val = _ReleaseTime;
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
            _Bottom = percent;
            break;
        case ESTATE::ATTACK:
            str = "MAXIMUM";
            _Top = percent;
            break;
        case ESTATE::DECAY:
        case ESTATE::SUSTAIN:
            str = "SUSTAIN LEVEL";
            _SetSustain = percent;
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
            val = _Bottom;
            break;
        case ESTATE::ATTACK:
            val = _Top;
            break;
        case ESTATE::DECAY:
            val = _SetSustain;
            break;
        case ESTATE::SUSTAIN:
            val = _SetSustain;
            break;
        case ESTATE::RELEASE:
            break;
        }
    return (val);
    }

//#######################################################################
void ENVELOPE_C::SetSoftLFO (bool sel)
    {
    _UseSoftLFO = sel;
    DBG ("Toggle %s > %s", _Name, (( sel ) ? "ON" : "Off") );
    }


//#######################################################################
void ENVELOPE_C::Start ()
    {
    if ( _Active || (_Top == 0.0 || _Muted ) )
        return;
    _Active = true;
    _State = ESTATE::START;
    _UseCount++;
    DBG ("Starting");
    }

//#######################################################################
void ENVELOPE_C::End ()
    {
    if ( !_Active )
        return;
    _TriggerEnd = true;
    _State = ESTATE::IDLE;
    }

//#######################################################################
void ENVELOPE_C::Clear ()
    {
    if ( _Active && _UseCount )     _UseCount--;
    _Active        = false;
    _TriggerEnd    = false;
    _State         = ESTATE::IDLE;
    _Current       = _Bottom;
    _Updated       = true;
    DBG ("clearing");
    Update ();
    }

//#######################################################################
void ENVELOPE_C::SetCurrent (float data)
    {
    _Current = data;
    short z = (short)(data * (float)DA_MAX);
    I2cDevices.D2Analog (_DevicePortIO, z);
    }

//#######################################################################
void ENVELOPE_C::SetOverride (uint32_t data)
    {
    I2cDevices.D2Analog (_DevicePortIO, data);
    }

//#######################################################################
void ENVELOPE_C::Update ()
    {
    float output;

    if ( _Updated )
        {
        output = _Current;
        if ( _UseSoftLFO )
            {
            output += output * SoftLFO.GetTri ();
            if ( output > 1.0 )
                output = 1.0;
            if ( output < 0.0 )
                output = 0.0;
            }
        int16_t z = (int16_t)((float)DA_MAX * output * _Expression);    //Calculate final D to A with output level and expression level
        I2cDevices.D2Analog (_DevicePortIO, z);
        _Updated = false;
        }
    }

//#######################################################################
//#######################################################################
void ENVELOPE_C::Process (float deltaTime)
    {
    if ( _UseSoftLFO )
        _Updated = true;

    //***************************************
    //  Beginning of the end
    //***************************************
    if ( _TriggerEnd && (_State != ESTATE::RELEASE) )
        {
        _State   = ESTATE::RELEASE;
        _Timer   = _ReleaseTime;
        _Delta   = _Current - _Bottom;
        DBG ("%f mSec from level %f to %f", _ReleaseTime, _Current, _Bottom);
        return;
        }

    switch ( _State )
        {
        //***************************************
        //  Start envelope
        //***************************************
        case ESTATE::START:
            {
            _Current = _Bottom;
            _Sustain = _SetSustain;             // update runtime sustain with sustain as user set
            _NoDecay = false;
            if ( _DecayTime < 8.0 )
                _NoDecay = true;

            _Timer       = 0.0;
            _Delta       = _Top - _Bottom ;
            _PeakLevel   = false;
            _TargetTime  = _AttackTime - TIME_THRESHOLD;
            _State       = ESTATE::ATTACK;
            DBG ("Start > %f mSec from level %f to %f", _AttackTime, _Current, _Top);
            return;
            }
        //***************************************
        //  ATTACK
        //***************************************
        case ESTATE::ATTACK:
            {
            _Timer += deltaTime;
            if ( _Timer < _TargetTime )
                {
                _Current  = _Bottom + ((_Timer / _TargetTime) * _Delta);
                _Updated = true;
                DBG ("Timer > %f mSec at level %f", _Timer, _Current);
                return;
                }
            _Current     = _Top;
            _Updated     = true;
            if ( _NoDecay )
                {
                _Timer   = 0.0;
                _State = ESTATE::SUSTAIN;
                DBG ("Hold at level %f", _Current);
                }
            else
                {
                _Timer      = _DecayTime - TIME_THRESHOLD;;
                _State      = ESTATE::DECAY;
                _Delta      = _Top - _Sustain;
                _TargetTime = 0.0;
                DBG ("%f mSec from level %f to %f", _DecayTime, _Current, _Sustain);
                }
            return;
            }

        //***************************************
        //  DECAY
        //***************************************
        case ESTATE::DECAY:
            {
            _Timer -= deltaTime;
            if ( _Timer > 10 )
                {
                _Current = _Sustain + ((_Timer / _DecayTime) * _Delta);
                _Updated = true;
                DBG ("Timer > %f mSec at level %f", _Timer, _Current);
                return;
                }
            _Current = _Sustain;
            _Updated = true;
            _Timer   = 0.0;
            _State   = ESTATE::SUSTAIN;

            if ( _Sustain >= _Top )
                _PeakLevel = true;

            DBG ("sustained at level %f", _Current);
            return;
            }
        //***************************************
        //  SUSTAIN
        //***************************************
        case ESTATE::SUSTAIN:
            {
            if ( _PeakLevel && (_Current != _Top) && !_DualUse )
                {
                _Current = _Top;
                _Updated = true;
                }
            return;
            }
        //***************************************
        //  RELEASE
        //***************************************
        case ESTATE::RELEASE:
            {
            _TriggerEnd = false;
            _Timer  -= deltaTime;
            if ( _Timer > 20)
                {
                _Current = _Bottom + ((_Timer / _ReleaseTime) * _Delta);
                _Updated = true;
                DBG ("Timer > %f mSec at level %f", _Timer, _Current);

                // Process string damper
                bool damper = false;
                switch ( _DamperMode )
                    {
                    default:
                        break;
                    case DAMPER::NORMAL:
                        if ( _Damper )      damper = false;
                        else                damper = true;
                        break;
                    case DAMPER::INVERT:
                        if ( _Damper )      damper = true;
                        else                damper = false;
                        break;
                    }
                if ( !damper )
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


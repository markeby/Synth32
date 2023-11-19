//#######################################################################
// Module:     Osc.cpp
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include "config.h"
#include "Osc.h"

using namespace OSC_N;

#define CONST_MULT      (DA_RANGE / FULL_KEYS)
static  const char*     MixerNames[] = { "sine", "triangle", "square", "saw", "pulse" };

//#######################################################################
SYNTH_OSC_C::SYNTH_OSC_C ()
    {
    Valid = false;
    }

//#######################################################################
void SYNTH_OSC_C::Begin (int num, uint8_t first_device)
    {
    Number = num;
    // D/A configuration
    OscChannel = first_device + uint8_t(D_A_OFF::EXPO);
    PwmChannel = first_device + uint8_t(D_A_OFF::WIDTH);
    Mix[int(SHAPE::TRIANGLE)].Channel = first_device + uint8_t(D_A_OFF::TRIANGLE);
    Mix[int(SHAPE::SAWTOOTH)].Channel = first_device + uint8_t(D_A_OFF::SAWTOOTH);
    Mix[int(SHAPE::PULSE)].Channel    = first_device + uint8_t(D_A_OFF::PULSE);
    Mix[int(SHAPE::SINE)].Channel     = first_device + uint8_t(D_A_OFF::SINE);
    Mix[int(SHAPE::SQUARE)].Channel   = first_device + uint8_t(D_A_OFF::SQUARE);

    // Initialize mixer
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        MIXER_T& m = Mix[z];

        m.Name             = MixerNames[z];
        m.CurrentLevel     = 0;
        m.LimitLevel       = 0;
        m.AttackTime       = 0;
        m.DecayTime        = 0;
        m.DacayTargetLevel = 0;
        m.SustainLevel     = 0;
        m.SustainTime      = 0;
        m.ReleaseTime      = 0;
        m.Change           = false;
        m.Active           = false;
        m.State            = STATE::IDLE;
        }
    // Configure keyboard MIDI frequencies
    memset (OctaveArray, 0, sizeof (OctaveArray));
    for ( int z = 0, m = 0;  z < FULL_KEYS; z++, m++ )
        {
        OctaveArray[z] = (uint16_t)((float)m * CONST_MULT);     // These DtoA are 9 x 1v / octave-
        if ( OctaveArray[z] > MAXDA )
            OctaveArray[z] = (uint16_t)((float)MAXDA * CONST_MULT);
        }

    if ( I2cDevices.IsChannelValid (first_device) && I2cDevices.IsChannelValid (first_device + 7) )
        {
        ClearState ();
        printf("\t  >> VCO %d started for device %d\n", num, first_device);
        Valid = true;
        }
    else
        printf("\t  ** VCO %d NO USABLE D/A CHANNELS FROM DEVICE %d\n", num, first_device);
    }

//#######################################################################
void SYNTH_OSC_C::ClearState ()
    {
    TriggerUp = false;
    TriggerDown = false;

    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++)
        {
        MIXER_T& m = Mix[z];
        m.Active   = false;
        m.State    = STATE::IDLE;
        I2cDevices.D2Analog(m.Channel, 0);
        }
    }

//#######################################################################
void SYNTH_OSC_C::Clear ()
    {
    ClearState ();
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_OSC_C::SetTuning ()
    {
    I2cDevices.D2Analog (Mix[(int)SHAPE::SQUARE].Channel, MAXDA);
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    }

//#######################################################################
void SYNTH_OSC_C::NoteSet (uint8_t note, uint8_t velocity)
    {
    CurrentNote = note;
    if ( DebugOsc )
        printf("[VCO %d] Key > %d D/A > %d\n", Number, note, OctaveArray[note]);

    ClearState ();
    I2cDevices.D2Analog (OscChannel, OctaveArray[note]);
    TriggerDown = true;
    }

//#######################################################################
void SYNTH_OSC_C::NoteClear ()
    {
    TriggerUp = true;
    }

//#######################################################################
void SYNTH_OSC_C::SetAttackTime (uint8_t wave, float time)
    {
    Mix[wave].AttackTime = time;
    if ( DebugOsc )
        printf("[VCO %d] %s Attack > %f\n", Number, Mix[wave].Name, time );
    }

//#######################################################################
void SYNTH_OSC_C::SetDecayTime (uint8_t wave, float time)
    {
    Mix[wave].DecayTime = time;
    if ( DebugOsc )
        printf("[VCO %d] %s Decay > %f\n", Number, Mix[wave].Name, time );
    }

//#######################################################################
void SYNTH_OSC_C::SetReleaseTime (uint8_t wave, float time)
    {
    Mix[wave].ReleaseTime = time;
    if ( DebugOsc )
        printf("[VCO %d] %s Release > %f\n", Number, Mix[wave].Name, time );
    }

//#######################################################################
void SYNTH_OSC_C::SetSustainLevel (uint8_t wave, float level_percent)
    {
    Mix[wave].SustainLevel = (int)((level_percent * 0.01) * (float)MAXDA);
    if ( DebugOsc )
        printf("[VCO %d] %s Sustain level > %d\n", Number, Mix[wave].Name, Mix[wave].SustainLevel);
    }

//#######################################################################
void SYNTH_OSC_C::SetSustainTime (uint8_t wave, float time)
    {
    Mix[wave].SustainTime = time;
    if ( DebugOsc )
        printf("[VCO %d] %s Sustain > %f\n", Number, Mix[wave].Name, time );
    }

//#######################################################################
void SYNTH_OSC_C::SetMaxLevel (uint8_t wave, float level_percent)
    {
    Mix[wave].LimitLevel = (int)((level_percent * 0.01) * (float)MAXDA);
    if ( DebugOsc )
        printf("[VCO %d] %s Level limit > %d\n", Number, Mix[wave].Name, Mix[wave].LimitLevel );
    }

//#######################################################################
//#######################################################################
bool SYNTH_OSC_C::Loop ()
    {
    float deltaTime = DeltaTime;

    //**************************************
    //***** Process initial key press ******
    //**************************************
    if ( TriggerDown )
        {
        for ( int z = 0;  z < OSC_MIXER_COUNT;  z++)
            {
            MIXER_T& m = Mix[z];
            if ( m.LimitLevel > 0 )
                {
                m.Active    = true;
                m.State     = STATE::ATTACK;
                m.Timer     = m.AttackTime;
                m.Change    = True;
                if ( DebugOsc )
                    printf ("[VCO %d] %s ATTACK  start > %f mSec to level %d\n", Number,  m.Name, m.AttackTime, m.LimitLevel);
                deltaTime   = 0;
                Active++;
                }
            }
        }

    if ( Active == 0 )
        return true;

    //***************************************
    //***** Loop through mixer channels *****
    //***************************************
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++)
        {
        MIXER_T& m = Mix[z];
        if ( !m.Active )
            continue;

        if ( DebugOsc & m.Change )
            printf ("[VCO %d] %s ", Number,  m.Name);

        //*******************************
        //**********   ATTACK  **********
        //*******************************
        if ( m.State == STATE::ATTACK )
            {
            if ( !TriggerUp )       // key is still down
                {
                if ( m.Timer > 10 )
                    {
                    m.Timer  -= deltaTime;
                    m.CurrentLevel  = ((m.AttackTime - m.Timer) / m.AttackTime) * m.LimitLevel;
                    if ( DebugOsc )
                        printf ("ATTACK  timer > %f mSec at level %d\n", m.Timer, m.CurrentLevel);

                    continue;
                    }
                }
            m.State        = STATE::DECAY;
            m.Timer        = m.DecayTime;
            m.DacayTargetLevel = (m.SustainLevel > m.LimitLevel ) ? m.LimitLevel : m.SustainLevel;
            m.CurrentLevel = m.LimitLevel;
            if ( DebugOsc )
               printf ("DECAY   start > %f  mSec from level %d to level %d\n", m.DecayTime, m.CurrentLevel, m.DacayTargetLevel);
            }

        //******************************
        //**********  DECAY  ***********
        //******************************
        if ( m.State == STATE::DECAY )
            {
            if ( !TriggerUp )       // key is still down
                {
                if ( m.Timer > 10 )
                    {
                    m.Timer       -= deltaTime;
                    m.CurrentLevel  = m.DacayTargetLevel + (m.Timer / m.DecayTime) * (m.LimitLevel - m.DacayTargetLevel);
                    if ( DebugOsc )
                        printf ("DECAY   timer > %f mSec at level %d\n", m.Timer, m.CurrentLevel);
                    continue;
                    }
                }
            m.State        = STATE::SUSTAIN;
            m.Timer        = m.SustainTime;
            m.CurrentLevel = m.DacayTargetLevel;
            if ( m.SustainTime > 0 )
                {
                if ( DebugOsc )
                    printf ("SUSTAIN > %f mSec al level %d\n", m.Timer, m.CurrentLevel);
                }
            else
                {
                m.Change = false;
                if ( DebugOsc )
                    printf ("SUSTAIN > key down at level %d\n", m.CurrentLevel);
                }
            }

        //*******************************
        //**********  SUSTAIN  **********
        //*******************************
        if ( m.State == STATE::SUSTAIN )
            {
            if ( !TriggerUp )       // key is still down
                {
                if ( m.Timer < 0 )
                    continue;
                if ( m.Timer > 10 )
                    {
                    m.Timer -= deltaTime;
                    if ( DebugOsc )
                        printf ("SUSTAIN timer > %f mSec at level %d\n", m.Timer, m.CurrentLevel);
                    continue;
                    }
                }
            m.State  = STATE::RELEASE;
            m.Timer  = m.ReleaseTime;
            m.Change = true;
            if ( DebugOsc )
                printf ("[VCO %d] %s RELEASE start > %f\n", Number,  m.Name, m.Timer);
            continue;
            }

        //*******************************
        //**********  RELEASE  **********
        //*******************************
        if ( m.State == STATE::RELEASE )
            {
            if ( m.Timer > 10)
                {
                m.Timer -= deltaTime;
                m.CurrentLevel = ((float)m.Timer / (float)m.ReleaseTime) * m.DacayTargetLevel;
                if ( DebugOsc )
                    printf ("RELEASE timer > %f mSec at level %d\n", m.Timer, m.CurrentLevel);

                continue;
                }
            m.CurrentLevel = 0;
            m.Timer        = 0;
            m.State        = STATE::IDLE;
            }
        }


    //*******************************************
    //**********  feed D/A converters  **********
    //*******************************************
    int active = 0;
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++)
        {
        MIXER_T& m = Mix[z];
        if ( m.Active)
            {
            I2cDevices.D2Analog(m.Channel, m.CurrentLevel);

            if ( m.State == STATE::IDLE )
                m.Active = false;
            else
                active++;
            }
        }

    TriggerDown = false;            // If this was a key down, we don't need it anymore.
    TriggerUp = false;              // If this was a key up, we don't need it anymore.
    Active = active;                // Detect if Osc is still active
    I2cDevices.UpdateAnalog ();     // Update D/A ports
    return (Active == 0);           // return true when oscillator is done
    }


//#######################################################################
// Module:     LFOosc.h
// Descrption: Oscillator controls
// Creator:    markeby
// Date:       9/18/2024
//#######################################################################
#pragma once
#include "Config.h"

//#######################################################################
enum class LFO_SHAPE {
    SINE = 0,
    RAMP,
    PULSE,
    };

enum class LFO_D_A_OFF {
    BEND = 0,
    WIDTH,
    FREQ,
    RAMP,
    PULSE,
    SINE
    };

#define MAX_FREQ_DEV    32

//#######################################################################
class   SYNTH_LFO_C
    {
private:
    bool    Valid;
    int     Active;
    int     Number;
    byte    OscPortIO;
    byte    PwmPortIO;
    byte    BendPortIO;
    short   Offset;
    byte    SlopePortO;
    byte    HardResetPortIO;
    byte    Midi;
    byte    InUse;
    bool    UpdateNeded;
    short   CurrentFreqCoarse;
    short   CurrentFreqFine;
    short   CurrentFreq;
    float   CurrentWidth;
    byte    CurrentLevel;
    bool    RampSlope;
    bool    ResetOn;
    bool    ModLevelAlt;

    typedef struct
        {
        uint8_t     Port;           // I2C device index
        uint16_t    CurrentLevel;   // Current setting 12 bit D/A
        uint16_t    MaximumLevel;   // No higher than this
        bool        Select;
        String      Name;
        } WAVE_LEVEL_T;

    WAVE_LEVEL_T    Level[SOURCE_CNT_LFO];

    void ClearState         (void);
    void SetInternalLevel   (uint8_t ch, uint8_t data);
    void ProcessFreq        (void);
    void OutputFreqIO       (void);

public:
         SYNTH_LFO_C        (void);
    void Begin              (int num, uint8_t first_device, uint8_t lfo_digital);
    void Clear              (void);
    void SetPulseWidth      (short value);
    void SetLevel           (uint8_t ch, uint8_t data);
    void SetWave            (short ch, bool state);
    void SetLevelMidi       (byte mchan, uint8_t data);
    void PitchBend          (short value);
    void SetRampDir         (bool state);
    void HardReset          (byte mchan);
    void Loop               (void);

    void PitchBend (byte mchan, short value)
        {
        if ( mchan == this->Midi )
            this->PitchBend (value);
        }

    void  SetMidi (byte data)
        { Midi = data; }

    byte  GetMidi (void)
        { return (Midi); }

    void  SetOffset  (short val)
        { this->Offset = val;  }

    short GetOffset  (void)
        { return (this->Offset); }

    bool GetWave (short ch)
        { return (this->Level[ch].Select); }

    bool GetRampDir (void)
        { return (this->RampSlope); }

    void SetFreqCoarse (short value)
        { this->CurrentFreqCoarse = value; this->ProcessFreq (); }

    void SetFreqFine (short value)
        { this->CurrentFreqFine = value; this->ProcessFreq (); }

    short GetFreq (void)
        { return (this->CurrentFreq); }

    void SetFreq (short value)
        { this->CurrentFreq = value; this->OutputFreqIO (); }

    void SetModLevelAlt (bool state)
        { this->ModLevelAlt = state; }

    bool GetModLevelAlt (void)
        { return (this->ModLevelAlt); }
    };



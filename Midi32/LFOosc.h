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
    bool    _Valid;
    int     _Active;
    int     _Number;
    byte    _OscPortIO;
    byte    _PwmPortIO;
    byte    _BendPortIO;
    short   _Offset;
    byte    _SlopePortO;
    byte    _HardResetPortIO;
    byte    _Midi;
    byte    _InUse;
    bool    _UpdateNeded;
    short   _CurrentFreqCoarse;
    short   _CurrentFreqFine;
    short   _CurrentFreq;
    float   _CurrentWidth;
    byte    _CurrentLevel;
    bool    _RampSlope;
    bool    _ResetOn;
    bool    _ModLevelAlt;

    typedef struct
        {
        uint8_t     Port;           // I2C device index
        uint16_t    CurrentLevel;   // Current setting 12 bit D/A
        uint16_t    MaximumLevel;   // No higher than this
        bool        Select;
        String      Name;
        } WAVE_LEVEL_T;

    WAVE_LEVEL_T    _Level[SOURCE_CNT_LFO];

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
    void ResetControl       (void);
    void PitchBend          (short value);
    void SetRampDir         (bool state);
    void HardReset          (byte mchan);
    void Loop               (void);

    void PitchBend (byte mchan, short value)
        {
        if ( mchan == _Midi )
            PitchBend (value);
        }

    void  SetMidi (byte data)
        { _Midi = data; }

    byte  GetMidi (void)
        { return (_Midi); }

    void  SetOffset  (short val)
        { _Offset = val;  }

    short GetOffset  (void)
        { return (_Offset); }

    bool GetWave (short ch)
        { return (_Level[ch].Select); }

    bool GetRampDir (void)
        { return (_RampSlope); }

    void SetFreqCoarse (short value)
        { _CurrentFreqCoarse = value; ProcessFreq (); }

    void SetFreqFine (short value)
        { _CurrentFreqFine = value; ProcessFreq (); }

    short GetFreq (void)
        { return (_CurrentFreq); }

    void SetFreq (short value)
        { _CurrentFreq = value; OutputFreqIO (); }

    void SetModLevelAlt (bool state)
        { _ModLevelAlt = state; }

    bool GetModLevelAlt (void)
        { return (_ModLevelAlt); }
    };



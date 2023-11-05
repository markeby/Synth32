//#######################################################################
// Module:     SynthFront.h
// Descrption: Interface to the synth channels
// Creator:    markeby
// Date:       8/25/2023
//#######################################################################
#pragma once

#define CHAN_COUNT 6    // number of synth oscillaotor/filter/control channels

namespace SYNTH_FRONT
    {
    void  KeyDown (byte channel, byte key, byte velocity);
    void  KeyUp (byte channel, byte key, byte velocity);
    } // end namespace SYNTH_FRONT

//#################################################
//    Synthesizer front end class
//#################################################
class   SYNTH_FRONT_C
    {
private:
    uint8_t             DownKey;
    uint8_t             DownVelocity;
    bool                DownTrigger;
    uint8_t             UpKey;
    uint8_t             UpVelocity;
    bool                UpTrigger;
    uint16_t            LastOp;
    bool                SelectWaveShapeVCO[OSC_MIXER_COUNT];
    int                 InTuning;
    int                 SetTuning;
    int                 SetDeviceIndex;

    MIDI_VALUE_MAP*     FaderMap;
    MIDI_VALUE_MAP*     KnobMap;
    MIDI_SWITCH_MAP*    SwitchMap;
    MIDI_VALUE_MAP*     PitchMap;

    String Selected (void);

public:
    void  SendControl       (void);
          SYNTH_FRONT_C     (int first_device, MIDI_VALUE_MAP* fader_map, MIDI_VALUE_MAP* knob_map, MIDI_VALUE_MAP* pitch_map, MIDI_SWITCH_MAP* switch_map);
    void  Begin             (void);
    void  Loop              (void);
    void  Controller        (byte type, byte chan, byte value);
    void  PitchBend         (byte chan, int value);
    void  ShortMsg          (uint8_t* data);
    void  ChannelSelect     (uint8_t chan, bool state);
    void  SelectWaveLFO     (uint8_t ch, uint8_t state);
    void  FreqSelectLFO     (uint8_t ch, float val);
    void  PitchBend         (float val);
    void  LFOrange          (bool up);
    void  SetLevelLFO       (float data);
    void  SetMaxLevel       (uint8_t wave, float data);
    void  SetAttack         ( float data);
    void  SetDecay          (float data);
    void  SetSustainLevel   (uint8_t ch, float data);
    void  SetSustainTime    (float data);
    void  SetRelease        (float data);


    //#######################################################################
    inline void  KeyDown (byte chan, byte key, byte velocity)
        {
        DownKey      = key;
        DownVelocity = velocity;
        DownTrigger  = true;
        }

    //#######################################################################
    inline void  KeyUp (byte chan, byte key, byte velocity)
        {
        UpKey      = key;
        UpVelocity = velocity;
        UpTrigger  = true;
        }

    //#######################################################################
    inline void StartTuning (int setting)
        {
        SetTuning = setting;
        }

    };

//#################################################

extern SYNTH_FRONT_C SynthFront;        // Synth front end class


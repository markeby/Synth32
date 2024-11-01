//#######################################################################
// Module:     I@Cmessages.h
// Descrption:  Synthesizer display message
// Creator:    markeby
// Date:       7/21/2024
//#######################################################################
#pragma once
#include "config.h"
#include "../Common/DispMessages.h"

//#################################################
//    Synthesizer display message class
//#################################################
class I2C_MESSAGE_C
    {
private:
    bool        Ready;
    bool        Paused;
    bool        ResetState;
    bool        Lock;
    uint64_t    ResetStart;

    uint8_t     DisplayAddress;

    void  SendComplete  (byte length);
    void  SendUpdate (DISP_MESSAGE_N::CMD_C page, uint8_t channel, DISP_MESSAGE_N::EFFECT_C effect, uint16_t value);

public:
          I2C_MESSAGE_C     (void);
    void  Begin             (uint8_t display, uint8_t sda, uint8_t scl);
    void  Pause             (bool state);
    void  Page              (DISP_MESSAGE_N::PAGE_C page);

    //#################################################
    inline bool Loop (void)
        {
        bool z = digitalRead (RESET_STROBE_IO);
        if ( z != this->ResetState )
            {
            this->ResetState = z;
            if ( z )
                {
                if ( (RunTime - this->ResetStart) < (RESET_TRIGGER_TIME + 50) )
                    return (true);
                else
                    return (false);
                return (false);
                }
            else
                this->ResetStart = RunTime;
            }
        }

    //#################################################
    inline void PageTuning (void)
        {
        this->Page (DISP_MESSAGE_N::PAGE_C::PAGE_TUNING);
        this->Lock = true;
        }

    //#################################################
    inline void TuningNote (uint8_t value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_TUNING, 0, DISP_MESSAGE_N::EFFECT_C::NOTE, value);
        this->Lock = true;
        }

    //#################################################
    inline void TuningLevel (uint8_t channel, uint8_t value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_TUNING, channel, DISP_MESSAGE_N::EFFECT_C::MAX_LEVEL, value);
        this->Lock = true;
        }

    //#################################################
    inline void PageAdvance (void)
        {
        this->Page (DISP_MESSAGE_N::PAGE_C::PAGE_ADVANCE);
        }

    //#################################################
    inline void SendUpdateOsc (uint8_t channel, DISP_MESSAGE_N::EFFECT_C effect, uint16_t value)
        {
        if ( !Lock )
            this->SendUpdate(DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_OSC, channel, effect, value);
        }

    //#################################################
    inline void SendUpdateMod (uint8_t channel, DISP_MESSAGE_N::EFFECT_C effect, uint16_t value)
        {
        if ( !Lock )
            this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_MOD, channel, effect, value);
        }

    //#################################################
    inline void SendUpdateFilter (uint8_t channel, DISP_MESSAGE_N::EFFECT_C effect, uint16_t value)
        {
        if ( !Lock )
            this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_FILTER, channel, effect, value);
        }

    //#################################################
    inline void OscSelected (uint8_t channel, bool select)
        {
        this->SendUpdateOsc (channel, DISP_MESSAGE_N::EFFECT_C::SELECTED, (( select ) ? 1 : 0));
        }

    //#################################################
    inline void OscAttackTime (uint8_t channel, uint16_t value)
        {
        this->SendUpdateOsc (channel, DISP_MESSAGE_N::EFFECT_C::ATTACK_TIME, value);
        }

    //#################################################
    inline void OscMaxLevel (uint8_t channel, uint16_t value)
        {
        this->SendUpdateOsc (channel, DISP_MESSAGE_N::EFFECT_C::MAX_LEVEL, value);
        }

    //#################################################
    inline void OscDecayTime (uint8_t channel, uint16_t value)
        {
        this->SendUpdateOsc (channel, DISP_MESSAGE_N::EFFECT_C::DECAY_TIME, value);
        }

    //#################################################
    inline void OscSustainTime (uint8_t channel, uint16_t value)
        {
        this->SendUpdateOsc (channel, DISP_MESSAGE_N::EFFECT_C::SUSTAIN_TIME, value);
        }

    //#################################################
    inline void OscReleaseTime (uint8_t channel, uint16_t value)
        {
        this->SendUpdateOsc (channel, DISP_MESSAGE_N::EFFECT_C::RELEASE_TIME, value);
        }

    //#################################################
    inline void OscSustainLevel (uint8_t channel, uint16_t value)
        {
        this->SendUpdateOsc (channel, DISP_MESSAGE_N::EFFECT_C::SUSTAIN_LEVEL, value);
        }

    //#################################################
    inline void OscSelectADSR (uint8_t channel, bool select)
        {
        this->SendUpdateOsc (channel, DISP_MESSAGE_N::EFFECT_C::SELECTED, (( select ) ? 1 : 0));
        }

    //#################################################
    inline void OscSawtoothDirection (bool select)
        {
        this->SendUpdateOsc ((uint8_t)(DISP_MESSAGE_N::CHANNEL_C::SAWTOOTH), DISP_MESSAGE_N::EFFECT_C::SAWTOOTH_DIRECTION, (( select ) ? 1 : 0));
        }

    //#################################################
    inline void OscPulseWidth (uint8_t width)
        {
        this->SendUpdateOsc ((uint8_t)(DISP_MESSAGE_N::CHANNEL_C::PULSE), DISP_MESSAGE_N::EFFECT_C::PULSE_WIDTH, width);
        }

    //#################################################
    inline void LFOSelected (uint8_t channel, bool select)
        {
        this->SendUpdateMod (channel, DISP_MESSAGE_N::EFFECT_C::SELECTED, (( select ) ? 1 : 0));
        }

    //#################################################
    inline void HardwareFreqLFO (uint8_t value)
        {
        this->SendUpdateMod ((uint8_t)(DISP_MESSAGE_N::CHANNEL_C::HARDWARE_LFO), DISP_MESSAGE_N::EFFECT_C::FREQ_LFO, value);
        }

    //#################################################
    inline void SoftwareFreqLFO (uint8_t value)
        {
        this->SendUpdateMod ((uint8_t)(DISP_MESSAGE_N::CHANNEL_C::SOFTWARE_LFO), DISP_MESSAGE_N::EFFECT_C::FREQ_LFO, value);
        }
    };

extern I2C_MESSAGE_C DisplayMessage;


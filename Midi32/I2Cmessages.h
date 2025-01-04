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

    byte        DisplayAddress;

    void  SendComplete  (byte length);
    void  SendUpdate (DISP_MESSAGE_N::CMD_C page, byte channel, DISP_MESSAGE_N::EFFECT_C effect, uint16_t value);

public:
          I2C_MESSAGE_C     (void);
    void  Begin             (byte display, byte sda, byte scl);
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
    inline void PageOsc (byte value)
        {
        this->Page ((DISP_MESSAGE_N::PAGE_C)value);
        }

    //#################################################
    inline void TuningNote (byte value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_TUNING, 0, DISP_MESSAGE_N::EFFECT_C::NOTE, value);
        this->Lock = true;
        }

    //#################################################
    inline void TuningLevel (byte channel, byte value)
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
    inline void SendUpdateOsc (byte zone, byte channel, DISP_MESSAGE_N::EFFECT_C effect, uint16_t value)
        {
        if ( !Lock )
            {
            DISP_MESSAGE_N::CMD_C page;
            switch ( zone )
                {
                case 0:
                    page = DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_OSC0;
                    break;
                case 1:
                    page = DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_OSC1;
                    break;
                case 2:
                    page = DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_OSC2;
                    break;
                default:
                    return;
                }
            this->SendUpdate (page, channel, effect, value);
            }
        }

    //#################################################
    inline void SendUpdateMod (byte channel, DISP_MESSAGE_N::EFFECT_C effect, uint16_t value)
        {
        if ( !Lock )
            this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_MOD, channel, effect, value);
        }

    //#################################################
    inline void OscSelected (byte zone, byte channel, bool select)
        {
        this->SendUpdateOsc (zone, channel, DISP_MESSAGE_N::EFFECT_C::SELECTED, (( select ) ? 1 : 0));
        }

    //#################################################
    inline void OscAttackTime (byte zone, byte channel, uint16_t value)
        {
        this->SendUpdateOsc (zone, channel, DISP_MESSAGE_N::EFFECT_C::ATTACK_TIME, value);
        }

    //#################################################
    inline void OscMaxLevel (byte zone, byte channel, uint16_t value)
        {
        this->SendUpdateOsc (zone, channel, DISP_MESSAGE_N::EFFECT_C::MAX_LEVEL, value);
        }

    //#################################################
    inline void OscDecayTime (byte zone, byte channel, uint16_t value)
        {
        this->SendUpdateOsc (zone, channel, DISP_MESSAGE_N::EFFECT_C::DECAY_TIME, value);
        }

    //#################################################
    inline void OscReleaseTime (byte zone, byte channel, uint16_t value)
        {
        this->SendUpdateOsc (zone, channel, DISP_MESSAGE_N::EFFECT_C::RELEASE_TIME, value);
        }

    //#################################################
    inline void OscSustainLevel (byte zone, byte channel, uint16_t value)
        {
        this->SendUpdateOsc (zone, channel, DISP_MESSAGE_N::EFFECT_C::SUSTAIN_LEVEL, value);
        }

    //#################################################
    inline void OscSelectADSR (byte zone, byte channel, bool select)
        {
        this->SendUpdateOsc (zone, channel, DISP_MESSAGE_N::EFFECT_C::SELECTED, (( select ) ? 1 : 0));
        }

    //#################################################
    inline void OscSawtoothDirection (byte zone, bool select)
        {
        this->SendUpdateOsc (zone, (byte)(DISP_MESSAGE_N::CHANNEL_C::SAWTOOTH), DISP_MESSAGE_N::EFFECT_C::SAWTOOTH_DIRECTION, (( select ) ? 1 : 0));
        }

    //#################################################
    inline void OscPulseWidth (byte zone, byte width)
        {
        this->SendUpdateOsc (zone, (byte)(DISP_MESSAGE_N::CHANNEL_C::PULSE), DISP_MESSAGE_N::EFFECT_C::PULSE_WIDTH, width);
        }

    //#################################################
    inline void OscNoise (byte zone, byte color, bool state)
        {
        this->SendUpdateOsc (zone, (byte)(DISP_MESSAGE_N::CHANNEL_C::NOISE), DISP_MESSAGE_N::EFFECT_C::NOISE, color | ( (state) ? 0x80 : 0x00 ));
        }

    //#################################################
    inline void LfoSoftwareFreq (byte value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::CHANNEL_C::SOFTWARE_LFO), DISP_MESSAGE_N::EFFECT_C::LFO_FREQ, value / 32);
        }

    //#################################################
    inline void LfoSoftwareSelect (byte value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::CHANNEL_C::SOFTWARE_LFO), DISP_MESSAGE_N::EFFECT_C::SELECTED, value);
        }

    //#################################################
    inline void LfoHardwareFreq (byte value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::CHANNEL_C::HARDWARE_LFO), DISP_MESSAGE_N::EFFECT_C::LFO_FREQ, value / 32);
        }

    //#################################################
    inline void LfoHardwareLevel (byte value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::CHANNEL_C::HARDWARE_LFO), DISP_MESSAGE_N::EFFECT_C::MAX_LEVEL, value);
        }

    //#################################################
    inline void LfoHardwarePulseWidth (short value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::CHANNEL_C::HARDWARE_LFO), DISP_MESSAGE_N::EFFECT_C::PULSE_WIDTH, value / 32 );
        }

    //#################################################
    inline void LfoHardwareRampSlope (byte value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::CHANNEL_C::HARDWARE_LFO), DISP_MESSAGE_N::EFFECT_C::SAWTOOTH_DIRECTION, value);
        }

    //#################################################
    inline void LfoHardwareSelect (short value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::CHANNEL_C::HARDWARE_LFO), DISP_MESSAGE_N::EFFECT_C::SELECTED, value);
        }
    };

extern I2C_MESSAGE_C DisplayMessage;


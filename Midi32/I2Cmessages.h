//#######################################################################
// Module:     I@Cmessages.h
// Descrption:  Synthesizer display message
// Creator:    markeby
// Date:       7/21/2024
//#######################################################################
#pragma once
#include "Config.h"
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
    byte        CurrentVoicePage;

    byte        DisplayAddress;

    void  SendComplete  (byte length);
    void  SendUpdate (DISP_MESSAGE_N::CMD_C cmd, byte mapi, byte channel, DISP_MESSAGE_N::EFFECT_C effect, short value);

public:
          I2C_MESSAGE_C (void);
    void  Begin         (byte display, byte sda, byte scl);
    void  Page          (DISP_MESSAGE_N::PAGE_C page);
    void  Reset         (void);
    void  SetVoicePage  (byte page, byte midi);

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
        }

    //#################################################
    inline void TuningNote (byte value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_TUNING,
                          0,
                          0,
                          DISP_MESSAGE_N::EFFECT_C::NOTE,
                          value);
        }

    //#################################################
    inline void TuningSelect (byte ch, byte value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_TUNING,
                          0,
                          ch,
                          DISP_MESSAGE_N::EFFECT_C::SELECTED,
                          value);
        }

    //#################################################
    inline void TuningLevel (byte channel, byte value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_TUNING,
                          0,
                          channel,
                          DISP_MESSAGE_N::EFFECT_C::MAX_LEVEL,
                          value);
        }

    //#################################################
    inline void Unlock (void)
        {
        this->Lock = false;
        }

    //#################################################
    inline void PageAdvance (void)
        {
        this->Page (DISP_MESSAGE_N::PAGE_C::PAGE_ADVANCE);
        }

    //#################################################
    inline void SetCurrentVoicePage (byte index)
        {
        this->CurrentVoicePage = index; // identify which voice page
        }

    //#################################################
    inline void SelectVoicePage (byte index)
        {
        this->CurrentVoicePage = index;             // identify which voice page
        this->Page ((DISP_MESSAGE_N::PAGE_C)index); // Select voice page to show
        }

    //#################################################
    inline void SendMapVoiceMidi (byte channel, DISP_MESSAGE_N::EFFECT_C effect, uint16_t value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_MAP,
                          0,
                          channel,
                          effect,
                          value);
        }

    //#################################################
    inline void SendUpdateMod (byte channel, DISP_MESSAGE_N::EFFECT_C effect, uint16_t value)
        {
        if ( !Lock )
            this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_MOD,
                              0,
                              channel,
                              effect,
                              value);
        }

    //#################################################
    inline void SendUpdateOsc (byte mapi, byte channel, DISP_MESSAGE_N::EFFECT_C effect, uint16_t value)
        {
        if ( !this->Lock )
            {
            this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_VOICE,
                              mapi,
                              channel,
                              effect,
                              value);
            }
        }

    //#################################################
    inline void OscSelected (byte channel, bool sel)
        {
        this->SendUpdateOsc (this->CurrentVoicePage,
                             channel,
                             DISP_MESSAGE_N::EFFECT_C::SELECTED,
                             (( sel ) ? 1 : 0));
        }

    //#################################################
    inline void OscAttackTime (byte channel, uint16_t value)
        {
        this->SendUpdateOsc (this->CurrentVoicePage,
                             channel,
                             DISP_MESSAGE_N::EFFECT_C::ATTACK_TIME,
                             value);
        }

    //#################################################
    inline void OscMaxLevel (byte channel, uint16_t value)
        {
        this->SendUpdateOsc (this->CurrentVoicePage,
                             channel,
                             DISP_MESSAGE_N::EFFECT_C::MAX_LEVEL,
                             value);
        }

    //#################################################
    inline void OscDecayTime (byte channel, uint16_t value)
        {
        this->SendUpdateOsc (this->CurrentVoicePage,
                             channel,
                             DISP_MESSAGE_N::EFFECT_C::DECAY_TIME,
                             value);
        }

    //#################################################
    inline void OscReleaseTime (byte channel, uint16_t value)
        {
        this->SendUpdateOsc (this->CurrentVoicePage,
                             channel,
                             DISP_MESSAGE_N::EFFECT_C::RELEASE_TIME,
                             value);
        }

    //#################################################
    inline void OscSustainLevel (byte channel, uint16_t value)
        {
        this->SendUpdateOsc (this->CurrentVoicePage,
                             channel,
                             DISP_MESSAGE_N::EFFECT_C::SUSTAIN_LEVEL,
                             value);
        }

    //#################################################
    inline void OscSelectADSR (byte channel, bool select)
        {
        this->SendUpdateOsc (this->CurrentVoicePage,
                             channel,
                             DISP_MESSAGE_N::EFFECT_C::SELECTED,
                             (( select ) ? 1 : 0));
        }

    //#################################################
    inline void OscSawtoothDirection ( bool sel)
        {
        this->SendUpdateOsc (this->CurrentVoicePage,
                             (byte)(DISP_MESSAGE_N::VOICE_C::SAWTOOTH),
                             DISP_MESSAGE_N::EFFECT_C::SAWTOOTH_DIRECTION,
                             (( sel ) ? 1 : 0));
        }

    //#################################################
    inline void OscPulseWidth (byte width)
        {
        this->SendUpdateOsc (this->CurrentVoicePage,
                             (byte)(DISP_MESSAGE_N::VOICE_C::PULSE),
                             DISP_MESSAGE_N::EFFECT_C::PULSE_WIDTH,
                             width);
        }

    //#################################################
    inline void OscNoise (byte color, bool state)
        {
        this->SendUpdateOsc (this->CurrentVoicePage,
                             (byte)(DISP_MESSAGE_N::VOICE_C::NOISE),
                             DISP_MESSAGE_N::EFFECT_C::NOISE,
                             color | ( (state) ? 0x80 : 0x00 ));
        }

    //#################################################
    inline void LfoSoftFreq (short value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::VOICE_C::SOFTWARE_LFO),
                             DISP_MESSAGE_N::EFFECT_C::LFO_FREQ,
                             value);
        }

    //#################################################
    inline void LfoSoftSelect (byte ch, bool sel)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::VOICE_C::SOFTWARE_LFO),
                             ((sel) ? DISP_MESSAGE_N::EFFECT_C::SELECTED : DISP_MESSAGE_N::EFFECT_C::DESELECTED),
                             ch);
        }

    //#################################################
    inline void LfoHardFreq (short value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::VOICE_C::HARDWARE_LFO),
                             DISP_MESSAGE_N::EFFECT_C::LFO_FREQ,
                             value);
        }

    //#################################################
    inline void LfoHardLevel (short value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::VOICE_C::HARDWARE_LFO),
                             DISP_MESSAGE_N::EFFECT_C::MAX_LEVEL,
                             value);
        }

    //#################################################
    inline void LfoHardPulseWidth (short value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::VOICE_C::HARDWARE_LFO),
                             DISP_MESSAGE_N::EFFECT_C::PULSE_WIDTH,
                             value);
        }

    //#################################################
    inline void LfoHardRampSlope (byte value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::VOICE_C::HARDWARE_LFO),
                             DISP_MESSAGE_N::EFFECT_C::SAWTOOTH_DIRECTION,
                             value);
        }

    //#################################################
    inline void LfoHardSelect (byte ch, bool sel)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::VOICE_C::HARDWARE_LFO),
                             ((sel) ? DISP_MESSAGE_N::EFFECT_C::SELECTED : DISP_MESSAGE_N::EFFECT_C::DESELECTED),
                              ch);
        }
    };

extern I2C_MESSAGE_C DisplayMessage;


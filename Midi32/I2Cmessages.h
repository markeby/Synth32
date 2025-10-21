//#######################################################################
// Module:     I2Cmessages.h
// Descrption:  Synthesizer display message
// Creator:    markeby
// Date:       7/21/2024
//#######################################################################
#pragma once
#include "Config.h"
#include <DispMessages.h>

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
    byte        CurrentPage;
    byte        DisplayAddress;
    byte        SendBuffer[16];
    uint8_t     LastEndT;

    void  SendComplete  (byte length);
    void  SendUpdate    (DISP_MESSAGE_N::CMD_C cmd, byte mapi, byte channel, DISP_MESSAGE_N::EFFECT_C effect, short value);

public:
          I2C_MESSAGE_C (void);
    void  Begin         (byte display, byte sda, byte scl);
    void  Page          (byte page);
    void  Page          (DISP_MESSAGE_N::PAGE_C page)               { this->Page ((byte)page); }
    byte  Page          (void)                                      { return (this->CurrentPage); }
    void  Reset         (void);
    void  SetPage       (byte page, byte midi);
    void  SetPage       (DISP_MESSAGE_N::PAGE_C page, byte midi)    { this->SetPage ((byte)page,  midi); }

    //#################################################
    bool Loop (void)
        {
        bool z = digitalRead (RESET_STROBE_IO);
        if ( z != this->ResetState )
            {
            this->ResetState = z;
            if ( z )
                return (true);
            else
                return (false);
            }
        return (false);
        }

    //#################################################
    void PageCalibration (void)
        {
        this->Page (DISP_MESSAGE_N::PAGE_C::PAGE_CALIBRATION);
        }

    //#################################################
    void PageTuning (void)
        {
        this->Page (DISP_MESSAGE_N::PAGE_C::PAGE_TUNING);
        }

    //#################################################
    void TuningNote (byte value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_TUNING,
                          0, 0,
                          DISP_MESSAGE_N::EFFECT_C::NOTE,
                          value);
        }

    //#################################################
    void TuningDtoA (short value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_TUNING,
                          0, 0,
                          DISP_MESSAGE_N::EFFECT_C::VALUE,
                          value);
        }

    //#################################################
    void TuningSelect (byte ch)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_TUNING,
                          0, ch,
                          DISP_MESSAGE_N::EFFECT_C::SELECTED,
                          0);
        }

    //#################################################
    void TuningSelectSecond (byte ch)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_TUNING,
                          0, ch,
                          DISP_MESSAGE_N::EFFECT_C::ALTERNATE,
                          0);
        }

    //#################################################
    void TuningLevel (byte channel, byte value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_TUNING,
                          0, channel,
                          DISP_MESSAGE_N::EFFECT_C::MAX_LEVEL,
                          value);
        }

    //#################################################
    void TuningFilter (byte channel, byte value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_TUNING,
                          0, channel,
                          DISP_MESSAGE_N::EFFECT_C::FILTER,
                          value);
        }

    //#################################################
    void TuningControl (byte value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_TUNING,
                          0, 0,
                          DISP_MESSAGE_N::EFFECT_C::CONTROL,
                          value);
        }

    //#################################################
    void Unlock (void)
        {
        this->Lock = false;
        }

    //#################################################
    void SendMapVoiceMidi (byte channel, DISP_MESSAGE_N::EFFECT_C effect, uint16_t value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_MAP,
                          0, channel, effect, value);
        }

    //#################################################
    void SendUpdateMod (byte channel, byte index, DISP_MESSAGE_N::EFFECT_C effect, uint16_t value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_MOD,
                          index, channel, effect, value);
        }

    //#################################################
    //#################################################
    void SendUpdateOsc (byte mapi, byte channel, DISP_MESSAGE_N::EFFECT_C effect, uint16_t value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_VOICE,
                          mapi, channel, effect, value);
        }

    //#################################################
    void OscDamperMode (byte channel, byte mode)
        {
        this->SendUpdateOsc (0, channel,
                             DISP_MESSAGE_N::EFFECT_C::DAMPER,
                             mode);
        }

    //#################################################
    void OscMute (bool sel)
        {
        this->SendUpdateOsc (0, 0,
                             DISP_MESSAGE_N::EFFECT_C::MUTE,
                             (( sel ) ? 1 : 0));
        }

    //#################################################
    void OscSelectedLevel (byte channel, bool sel)
        {
        this->SendUpdateOsc (0, channel,
                             DISP_MESSAGE_N::EFFECT_C::SELECTED,
                             (( sel ) ? 1 : 0));
        }

    //#################################################
    void OscAttackTime (byte channel, uint16_t value)
        {
        this->SendUpdateOsc (0, channel,
                             DISP_MESSAGE_N::EFFECT_C::ATTACK_TIME,
                             value);
        }

    //#################################################
    void OscMaxLevel (byte channel, uint16_t value)
        {
        this->SendUpdateOsc (0, channel,
                             DISP_MESSAGE_N::EFFECT_C::MAX_LEVEL,
                             value);
        }

    //#################################################
    void OscDecayTime (byte channel, uint16_t value)
        {
        this->SendUpdateOsc (0, channel,
                             DISP_MESSAGE_N::EFFECT_C::DECAY_TIME,
                             value);
        }

    //#################################################
    void OscReleaseTime (byte channel, uint16_t value)
        {
        this->SendUpdateOsc (0, channel,
                             DISP_MESSAGE_N::EFFECT_C::RELEASE_TIME,
                             value);
        }

    //#################################################
    void OscSustainLevel (byte channel, uint16_t value)
        {
        this->SendUpdateOsc (0, channel,
                             DISP_MESSAGE_N::EFFECT_C::SUSTAIN_LEVEL,
                             value);
        }

    //#################################################
    void OscRampDirection (bool sel)
        {
        this->SendUpdateOsc (0, (byte)(DISP_MESSAGE_N::VOICE_OPT_C::RAMP),
                             DISP_MESSAGE_N::EFFECT_C::RAMP_DIRECTION,
                             (( sel ) ? 1 : 0));
        }

    //#################################################
    void OscPulseWidth (byte width)
        {
        this->SendUpdateOsc (0, (byte)(DISP_MESSAGE_N::VOICE_OPT_C::PULSE),
                             DISP_MESSAGE_N::EFFECT_C::PULSE_WIDTH,
                             width);
        }

    //#################################################
    //#################################################
    void SendUpdateFlt (byte mapi, byte channel, DISP_MESSAGE_N::EFFECT_C effect, uint16_t value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_FILTER,
                          mapi, channel, effect, value);
        }

    //#################################################
    void FltSelected (byte channel, bool sel)
        {
        this->SendUpdateOsc (0, channel,
                             DISP_MESSAGE_N::EFFECT_C::SELECTED,
                             (( sel ) ? 1 : 0));
        }

    //#################################################
    void FltAttackTime (byte channel, uint16_t value)
        {
        this->SendUpdateFlt (0, channel,
                             DISP_MESSAGE_N::EFFECT_C::ATTACK_TIME,
                             value);
        }

    //#################################################
    void FltDecayTime (byte channel, uint16_t value)
        {
        this->SendUpdateFlt (0, channel,
                             DISP_MESSAGE_N::EFFECT_C::DECAY_TIME,
                             value);
        }

    //#################################################
    void FltReleaseTime (byte channel, uint16_t value)
        {
        this->SendUpdateFlt (0, channel,
                             DISP_MESSAGE_N::EFFECT_C::RELEASE_TIME,
                             value);
        }

    //#################################################
    void FltStart (byte channel, uint16_t value)
        {
        this->SendUpdateFlt (0, channel,
                             DISP_MESSAGE_N::EFFECT_C::BASE_LEVEL,
                             value);
        }

    //#################################################
    void FltEnd (byte channel, uint16_t value)
        {
        this->SendUpdateFlt (0, channel,
                             DISP_MESSAGE_N::EFFECT_C::MAX_LEVEL,
                             value);
        }

    //#################################################
    void FltSustain (byte channel, uint16_t value)
        {
        this->SendUpdateFlt (0, channel,
                             DISP_MESSAGE_N::EFFECT_C::SUSTAIN_LEVEL,
                             value);
        }

    //#################################################
    void FltOut (byte fmap)
        {
        this->SendUpdateFlt (0, 0,
                             DISP_MESSAGE_N::EFFECT_C::MAP_VOICE,
                             fmap);
        }

    //#################################################
    void FltCtrl (byte fn, byte value)
        {
        this->SendUpdateFlt (0, fn,
                             DISP_MESSAGE_N::EFFECT_C::CONTROL,
                             value);
        }

    //#################################################
    //#################################################
    void LfoSoftFreq (short value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::VOICE_OPT_C::SOFTWARE_LFO),
                             0,
                             DISP_MESSAGE_N::EFFECT_C::LFO_FREQ,
                             value);
        }

    //#################################################
    void LfoSoftSelect (byte ch, bool sel)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::VOICE_OPT_C::SOFTWARE_LFO),
                             0,
                             ((sel) ? DISP_MESSAGE_N::EFFECT_C::SELECTED : DISP_MESSAGE_N::EFFECT_C::DESELECTED),
                             ch);
        }

    //#################################################
    void LfoHardFreq (byte index, short value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::VOICE_OPT_C::HARDWARE_LFO),
                             index,
                             DISP_MESSAGE_N::EFFECT_C::LFO_FREQ,
                             value);
        }

    //#################################################
    void LfoHardLevel (byte index, short value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::VOICE_OPT_C::HARDWARE_LFO),
                             index,
                             DISP_MESSAGE_N::EFFECT_C::MAX_LEVEL,
                             value);
        }

    //#################################################
    void LfoHardPulseWidth (byte index, short value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::VOICE_OPT_C::HARDWARE_LFO),
                             index,
                             DISP_MESSAGE_N::EFFECT_C::PULSE_WIDTH,
                             value);
        }

    //#################################################
    void LfoHardRampSlope (byte index, byte value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::VOICE_OPT_C::HARDWARE_LFO),
                             index,
                             DISP_MESSAGE_N::EFFECT_C::RAMP_DIRECTION,
                             value);
        }

    //#################################################
    void SetModLevelAlt (byte index, byte value)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::VOICE_OPT_C::HARDWARE_LFO),
                             index,
                             DISP_MESSAGE_N::EFFECT_C::ALTERNATE,
                             value);
        }

    //#################################################
    void LfoHardSelect (byte index, byte wave, bool sel)
        {
        this->SendUpdateMod ((byte)(DISP_MESSAGE_N::VOICE_OPT_C::HARDWARE_LFO),
                             index,
                             ((sel) ? DISP_MESSAGE_N::EFFECT_C::SELECTED : DISP_MESSAGE_N::EFFECT_C::DESELECTED),
                             wave);
        }
    //#################################################
    void SendLoadSave (uint16_t value)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_LOAD_SAVE,
                          0, 0,
                          DISP_MESSAGE_N::EFFECT_C::VALUE,
                          value);
        }
    //#################################################
    void LoadMessage (void)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_LOAD_SAVE,
                          0, 0,
                          DISP_MESSAGE_N::EFFECT_C::MESSAGE,
                          1);
        }
    //#################################################
    void SaveMessage (void)
        {
        this->SendUpdate (DISP_MESSAGE_N::CMD_C::UPDATE_PAGE_LOAD_SAVE,
                          0, 0,
                          DISP_MESSAGE_N::EFFECT_C::MESSAGE,
                          2);
        }
    };

extern I2C_MESSAGE_C DisplayMessage;


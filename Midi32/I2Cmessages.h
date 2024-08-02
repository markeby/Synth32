//#######################################################################
// Module:     I@Cmessages.h
// Descrption:  Synthesizer display message
// Creator:    markeby
// Date:       7/21/2024
//#######################################################################
#pragma once
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
    uint64_t    ResetStart;

    uint8_t     DisplayAddress;
    uint8_t     SendBuffer[16];

    void  SendComplete  (byte length);
    void  SendVCA (uint8_t channel, DISP_MESSAGE_N::EFFECT_C effect, uint16_t value);

public:
          I2C_MESSAGE_C     (void);
    void  Begin             (uint8_t display, uint8_t sda, uint8_t scl);
    void  Pause             (bool state);
    void  Page              (DISP_MESSAGE_N::PAGE_C page);
    void  Selected          (uint8_t channel, bool select);
    void  AttackTime        (uint8_t channel, uint16_t value);
    void  MaxLevel          (uint8_t channel, uint16_t value);
    void  DecayTime         (uint8_t channel, uint16_t value);
    void  SustainTime       (uint8_t channel, uint16_t value);
    void  SustainLevel      (uint8_t channel, uint16_t value);
    void  ReleaseTime       (uint8_t channel, uint16_t value);
    void  SelectADSR        (uint8_t channel, bool select);
    void  PulseWidth        (uint8_t width);
    void  SawtoothDirection (bool select);

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
    inline void PageOsc (void)
        {
        this->Page (DISP_MESSAGE_N::PAGE_C::PAGE_OSC);
        }

//#################################################
    inline void PageTuning (void)
        {
        this->Page (DISP_MESSAGE_N::PAGE_C::PAGE_TUNING);
        }

//#################################################
    inline void PageFilter (void)
        {
        this->Page (DISP_MESSAGE_N::PAGE_C::PAGE_FILTER);
        }

    };

extern I2C_MESSAGE_C DisplayMessage;


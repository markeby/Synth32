//#######################################################################
// Module:     Novation.h
// Descrption: Class for Novation devices
// Creator:    markeby
// Date:       7/7/2025
//#######################################################################
#pragma once
#include "Config.h"

//#######################################################################
#define NOVATION_LOOP_COUNT     1500

//#######################################################################
enum class XL_LED: byte
    {
    RED_FLASH       = 11,
    OFF             = 12,
    RED_LOW         = 13,
    RED             = 15,
    GREEN_LOW       = 28,
    AMBER_LOW       = 29,
    GREEN_FLASH     = 56,
    YELLOW_FLASH    = 58,
    AMBER_FLASH     = 59,
    GREEN           = 60,
    YELLOW          = 62,
    AMBER           = 63
    };

//#######################################################################
class NOVATION_XL_C
    {
private:
    short       CurrentMap;
    short       FlashCounter;
    XL_MIDI_MAP (*pMidiMap)[XL_MIDI_MAP_SIZE];
    byte        *pButtonState;
    bool        ButtonChange;
    bool        FlashState;
    short       Counter;

// located in FrontEnd.cpp
private:
    void    SendTo          (unsigned length, byte* buff);
    void    ResetAllLED     (byte index);
public:
    void    Begin           (XL_MIDI_MAP (*xl_map)[XL_MIDI_MAP_SIZE]);

// located in Novation.cpp
public:
            NOVATION_XL_C   (void);
    void    Loop            (void);
    void    TemplateReset   (byte index);
    void    TemplateRefresh (void)              { this->ButtonChange = true; }

    void    SelectTemplate  (byte index, byte* pbuttons=nullptr);
    void    SetColor        (byte index, byte led, XL_LED color);
    void    SetColor        (byte led, XL_LED color)                { this->SetColor (this->CurrentMap, led,  color); }
    void    ButtonColor     (byte led, XL_LED color)
        {
        if ( pButtonState == nullptr )
            return;
        pButtonState[led] = (byte)color;
        this->ButtonChange = true;
        }
    void    UpdateButtons   (void);
    short   GetCurrentMap   (void)                                  { return (this->CurrentMap); }
    };


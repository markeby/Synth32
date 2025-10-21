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
enum class XL: byte
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
    bool        ButtonChange;
    bool        FlashState;
    short       Counter;
    byte        ButtonState[XL_BUTTON_COUNT];

// located in FrontEnd.cpp
private:
    void    SendTo              (unsigned length, byte* buff);
    void    ResetAllLED         (byte index);
    void    TemplateReset       (byte index);
    void    SetColor            (byte index, byte led, XL color);

public:
            NOVATION_XL_C       (void);
    void    Begin               (void);
    void    Loop                (void);
    void    SelectTemplate      (byte index);
    void    TemplateRefresh     (void)                              { ButtonChange = true; }
    void    UpdateButtons       (void);
    short   GetCurrentMap       (void)                              { return (CurrentMap); }
    void    SetButtonState      (int index, byte value)             { ButtonState[index] = value; }
    void    SetButtonStateAG    (int index, bool state)             { SetButtonState (index, ( state ) ? (byte)XL::AMBER : (byte)XL::GREEN); }
    void    SetButtonStateRG    (int index, bool state)             { SetButtonState (index, ( state ) ? (byte)XL::RED : (byte)XL::GREEN); }
    void    SetColorTri         (int index, byte tri);
    };


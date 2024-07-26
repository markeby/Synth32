//#######################################################################
// Module:     Graphics.cpp
// Descrption: Graphics processing class
// Creator:    markeby
// Date:       7/23/2024
//#######################################################################

//   Graphics.h
//
//  Graphics for control displays
//
//  author: markeby
//
#pragma once

#include "../Common/SynthCommon.h"

class ESP_Panel;
class ADSR_WIDGET_C;

//############################################
class GRPH_C
    {
private:
    ESP_Panel* Panel;

    ADSR_WIDGET_C*     MeterADSR[OSC_MIXER_COUNT];

public:
            GRPH_C          (void);
    void    Begin           (void);
    void    Pause           (bool state);
    void    InitializePage1 (void);
    void    UpdatePageVCA   (byte ch, byte effect, short value);
    };

extern GRPH_C  Graphics;

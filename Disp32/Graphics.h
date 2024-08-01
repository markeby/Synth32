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

#include <lvgl.h>
#include "../Common/SynthCommon.h"

class ESP_Panel;
class TITLE_WIDGET_C;
class ADSR_WIDGET_C;
class LEVEL_WIDGET_C;

//############################################
class GRPH_C
    {
private:
    ESP_Panel*      Panel;

    byte            CurrentPage;

    // Oscillator control page
    lv_obj_t*       PageOsc;
    TITLE_WIDGET_C* TitleControl[OSC_MIXER_COUNT];
    ADSR_WIDGET_C*  MeterADSR[OSC_MIXER_COUNT];
    LEVEL_WIDGET_C* MaxLevel[OSC_MIXER_COUNT];
    LEVEL_WIDGET_C* SustainLevel[OSC_MIXER_COUNT];

    lv_obj_t*       Pages;
    lv_obj_t*       PageFilter;
    lv_obj_t*       PageTuning;

    const lv_font_t*   Tuningfont;
    lv_style_t         TuningStyle;
    lv_obj_t*          TuningTitle;

public:
            GRPH_C              (void);
    void    Begin               (void);
    void    Pause               (bool state);
    void    PageSelect          (byte page);
    void    InitPageOsc         (lv_obj_t* base);
    void    InitPageFilter      (lv_obj_t* base);
    void    InitPageTuning      (lv_obj_t* base);
    void    UpdatePage          (byte ch, byte effect, short value);
    };

extern GRPH_C  Graphics;

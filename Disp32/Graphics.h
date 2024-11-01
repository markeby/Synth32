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
#include "../Common/DispMessages.h"

class ESP_Panel;
class TITLE_WIDGET_C;
class ADSR_METER_WIDGET_C;
class LEVEL_WIDGET_C;
class SAWTOOTH_WIDGET_C;
class PULSE_WIDGET_C;
class LFO_METER_WIDGET_C;
class LFO_METER_WIDGET_C;
class NOTE_WIDGET_C;

//############################################
class GRPH_C
    {
private:
    // Display device
    ESP_Panel*  Panel;

    // Page objects
    lv_obj_t*               Pages;
    DISP_MESSAGE_N::PAGE_C  CurrentPage;
    lv_style_t              TitleStyle;

    // Oscillator control page
    lv_obj_t*               PageOsc;
    lv_obj_t*               TitleOsc;
    TITLE_WIDGET_C*         TitleControl[OSC_MIXER_COUNT];
    ADSR_METER_WIDGET_C*    MeterADSR[OSC_MIXER_COUNT];
    LEVEL_WIDGET_C*         MaxLevel[OSC_MIXER_COUNT];
    LEVEL_WIDGET_C*         SustainLevel[OSC_MIXER_COUNT];
    SAWTOOTH_WIDGET_C*      SawtoothDir;
    PULSE_WIDGET_C*         PulseWidth;

    // Modulation control page
    lv_obj_t*               PageMod;
    lv_obj_t*               TitleMod;
    TITLE_WIDGET_C*         TitleHardware;
    LFO_METER_WIDGET_C*     MeterHardware;

    TITLE_WIDGET_C*         TitleSoftware;
    LFO_METER_WIDGET_C*     MeterSoftware;

    // Filter control page
    lv_obj_t*               PageFilter;
    lv_obj_t*               TitleFlt;

    // Tunning page
    lv_obj_t*               PageTuning;
    const lv_font_t*        Tuningfont;
    lv_style_t              TuningStyle;
    lv_obj_t*               TuningTitle;
    NOTE_WIDGET_C*           Note;
    TITLE_WIDGET_C*         TitleTuning[OSC_MIXER_COUNT];
    LEVEL_WIDGET_C*         LevelTuning[OSC_MIXER_COUNT];

public:
            GRPH_C              (void);
    void    Begin               (void);
    void    Pause               (bool state);
    void    PageSelect          (DISP_MESSAGE_N::PAGE_C page);
    void    InitPageOsc         (lv_obj_t* base);
    void    InitPageMod         (lv_obj_t* base);
    void    InitPageFilter      (lv_obj_t* base);
    void    InitPageTuning      (lv_obj_t* base);
    void    UpdatePageMod       (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value);
    void    UpdatePageOsc       (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value);
    void    UpdatePageTuning    (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value);
    };

extern GRPH_C  Graphics;

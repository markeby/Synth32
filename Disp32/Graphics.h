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
class NOISE_WIDGET_C;

class PAGE_TITLE_C
    {
protected:
    lv_obj_t*       Title;
    lv_style_t      TitleStyle;
        PAGE_TITLE_C (lv_obj_t* base, const char* str);
    };

//############################################
//############################################
class PAGE_OSC_C : protected PAGE_TITLE_C
    {
private:
    TITLE_WIDGET_C*         TitleControl[OSC_MIXER_COUNT];
    ADSR_METER_WIDGET_C*    MeterADSR[OSC_MIXER_COUNT];
    LEVEL_WIDGET_C*         MaxLevel[OSC_MIXER_COUNT];
    LEVEL_WIDGET_C*         SustainLevel[OSC_MIXER_COUNT];
    SAWTOOTH_WIDGET_C*      SawtoothDir;
    PULSE_WIDGET_C*         PulseWidth;
    NOISE_WIDGET_C*         Noise;

public:
              PAGE_OSC_C (lv_obj_t* base, const char* str);
    void      UpdatePage (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value);
    };

//############################################
//############################################
class PAGE_MOD_C : protected PAGE_TITLE_C
    {
private:
    TITLE_WIDGET_C*         TitleHardware;
    LFO_METER_WIDGET_C*     MeterHardware;

    TITLE_WIDGET_C*         TitleSoftware;
    LFO_METER_WIDGET_C*     MeterSoftware;

public:
              PAGE_MOD_C (lv_obj_t* base);
    void      UpdatePage (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value);
    };

//############################################
//############################################
class PAGE_FILTER_C : protected PAGE_TITLE_C
    {
private:

public:
              PAGE_FILTER_C (lv_obj_t* base);
    void      UpdatePage (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value);
    };

//############################################
//############################################
class PAGE_TUNE_C
    {
private:
    const lv_font_t*        Tuningfont;
    lv_style_t              TuningStyle;
    lv_obj_t*               TuningTitle;
    NOTE_WIDGET_C*          Note;
    TITLE_WIDGET_C*         TitleTuning[OSC_MIXER_COUNT];
    LEVEL_WIDGET_C*         LevelTuning[OSC_MIXER_COUNT];

public:
              PAGE_TUNE_C (lv_obj_t* base);
    void      UpdatePage (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value);
    };

//############################################
//############################################
class GRPH_C
    {
private:
    // Display device
    ESP_Panel*  Panel;

    // Page objects
    lv_obj_t*               Pages;
    DISP_MESSAGE_N::PAGE_C  CurrentPage;

    lv_obj_t*               BasePageOsc0;
    lv_obj_t*               BasePageOsc1;
    lv_obj_t*               BasePageOsc2;
    lv_obj_t*               BasePageMod;
    lv_obj_t*               BasePageFilter;
    lv_obj_t*               BasePageTuning;

    PAGE_OSC_C*             PageOsc0;
    PAGE_OSC_C*             PageOsc1;
    PAGE_OSC_C*             PageOsc2;
    PAGE_MOD_C*             PageMod;
    PAGE_TUNE_C*            PageTune;
    PAGE_FILTER_C*          PageFilter;

public:
            GRPH_C              (void);
    void    Begin               (void);
    void    Pause               (bool state);
    void    PageSelect          (DISP_MESSAGE_N::PAGE_C page);
    void    UpdatePageMod       (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value) { PageMod->UpdatePage (ch, effect, value); }
    void    UpdatePageTuning    (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value) { PageTune->UpdatePage (ch, effect, value); }
    void    UpdatePageOsc0      (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value) { PageOsc0->UpdatePage (ch, effect, value); }
    void    UpdatePageOsc1      (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value) { PageOsc1->UpdatePage (ch, effect, value); }
    void    UpdatePageOsc2      (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value) { PageOsc2->UpdatePage (ch, effect, value); }
    };

extern GRPH_C  Graphics;

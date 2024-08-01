//#######################################################################
// Module:     Widgets.h
// Descrption: Widget models
// Creator:    markeby
// Date:       7/7/2024
//#######################################################################
#pragma once
#include <lvgl.h>

//############################################
class TITLE_WIDGET_C
    {
private:
    lv_obj_t*   Label;
    lv_style_t  Style;

public:
    TITLE_WIDGET_C (lv_obj_t* base, const char* s);
    };

//############################################
class ADSR_WIDGET_C
    {
private:
    lv_obj_t*   Meter;
    lv_obj_t*   Led;
    lv_obj_t*   MaxLevel;

    typedef struct
        {
        lv_meter_indicator_t*   Gauge;
        lv_obj_t*               Label;
        lv_obj_t*               Unit;
        lv_obj_t*               Value;
        lv_style_t              Style;
        } ADSR_ELEMENT_T;

    ADSR_ELEMENT_T  Attack;
    ADSR_ELEMENT_T  Decay;
    ADSR_ELEMENT_T  Sustain;
    ADSR_ELEMENT_T  Release;

    void    InfoLine        (lv_obj_t* base, ADSR_ELEMENT_T &element, const char* s, short y, uint32_t color);
public:
            ADSR_WIDGET_C   (lv_obj_t* base, const char* s, short x, short y);
    void    Volume          (lv_obj_t* base);
    void    Select          (bool sel);
    void    SetAttack       (int val);
    void    SetDecay        (int val);
    void    SetSustain      (int val);
    void    SetRelease      (int val);
    };

//############################################
class LEVEL_WIDGET_C
    {
private:
    lv_obj_t*   Slider;
    short       SliderSize;
    lv_style_t  StyleLabel;
    lv_style_t  StyleValue;
    lv_style_t  StyleMain;
    lv_style_t  StyleIndicator;
    lv_style_t  StyleKnob;
    lv_obj_t*   Label;
    lv_obj_t*   Unit;
    lv_obj_t*   Value;
    float       Multiplier;

public:
         LEVEL_WIDGET_C (lv_obj_t* base, const char* s, short x, short y, lv_palette_t p);
    void SetLevel (int val);
    };


//#######################################################################
// Module:     Widgets.h
// Descrption: Widget models
// Creator:    markeby
// Date:       7/7/2024
//#######################################################################
#pragma once
#include <lvgl.h>

//############################################
class ADSR_ELEMENT_C
    {
public:
    lv_meter_indicator_t*   Gauge;
    lv_obj_t*               Label;
    lv_obj_t*               Unit;
    lv_obj_t*               Value;
    lv_style_t              Style;
    };

//############################################
class ADSR_WIDGET_C
    {
private:
    lv_obj_t*           Meter;
    lv_obj_t*           Label;
    lv_style_t          LabelStyle;
    lv_obj_t*           Led;
    ADSR_ELEMENT_C      Attack;
    ADSR_ELEMENT_C      Decay;
    ADSR_ELEMENT_C      Sustain;
    ADSR_ELEMENT_C      Release;

public:
            ADSR_WIDGET_C   (lv_obj_t* base, const char* s, short x, short y);
//           ~ADSR_WIDGET_C   (void);
    void    Select          (bool sel);
    void    SetAttack       (int val);
    void    SetDecay        (int val);
    void    SetSustain      (int val);
    void    SetRelease      (int val);
    };


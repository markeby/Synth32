//#######################################################################
// Module:     Widgets.h
// Descrption: Widget models
// Creator:    markeby
// Date:       7/7/2024
//#######################################################################
#pragma once
#include <lvgl.h>

//############################################
class ADSR_WIDGET_C
    {
private:
    lv_obj_t*               Meter;
    lv_meter_indicator_t*   Attack;
    lv_meter_indicator_t*   Decay;
    lv_meter_indicator_t*   Sustain;
    lv_meter_indicator_t*   Release;

public:
            ADSR_WIDGET_C   (const char* s, int x, int y);
           ~ADSR_WIDGET_C   (void) {}
    void    SetAttack       (int val);
    void    SetDecay        (int val);
    void    SetSustain      (int val);
    void    SetRelease      (int val);
    };


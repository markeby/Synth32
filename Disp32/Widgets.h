//#######################################################################
// Module:     Widgets.h
// Descrption: Widget models
// Creator:    markeby
// Date:       7/7/2024
//#######################################################################
#pragma once
#include <lvgl.h>

//############################################
typedef struct MELEMENT_S
    {
    lv_meter_indicator_t*   Gauge;
    lv_obj_t*               Label;
    lv_obj_t*               Unit;
    lv_obj_t*               Value;
    lv_style_t              Style;
        MELEMENT_S () : Gauge(nullptr),  Label(nullptr), Unit(nullptr), Value(nullptr)
        {  }
    } METER_ELEMENT_S;

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
class ADSR_METER_WIDGET_C
    {
private:
    lv_obj_t*        Meter;
    lv_obj_t*        Led;
    METER_ELEMENT_S  Attack;
    METER_ELEMENT_S  Decay;
    METER_ELEMENT_S  Sustain;
    METER_ELEMENT_S  Release;

    void  InfoLine (lv_obj_t* base, METER_ELEMENT_S &element, const char* s, short y, uint32_t color);
public:
          ADSR_METER_WIDGET_C (lv_obj_t* base, short x, short y);
    void  Volume              (lv_obj_t* base);
    void  Select              (bool sel);
    void  SetAttack           (int val);
    void  SetDecay            (int val);
    void  SetSustain          (int val);
    void  SetRelease          (int val);
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
    void SetLevel       (int val);
    };

//############################################
class SAWTOOTH_WIDGET_C
    {
private:
    lv_obj_t*   SlopeFore;
    lv_style_t  StyleFore;
    lv_obj_t*   SlopeBack;
    lv_style_t  StyleBack;

public:
         SAWTOOTH_WIDGET_C (lv_obj_t* base,  lv_align_t align, short x, short y);
    void SetDir            (bool dir);
    };

//############################################
class PULSE_WIDGET_C
    {
private:
    #define     SQUARE_SIZE 10
    lv_point_t  Pulse[SQUARE_SIZE];
    lv_obj_t*   Wave;
    lv_style_t  Style;
private:
    lv_obj_t*   SlopeFore;
    lv_style_t  StyleFore;


public:
         PULSE_WIDGET_C (lv_obj_t* base,  lv_align_t align, short x, short y);
    void SetWidth       (short width);
    };

//############################################
class NOTE_WIDGET_C
    {
private:
    lv_style_t  StyleValue;
    lv_obj_t*   Value;

public:
         NOTE_WIDGET_C (lv_obj_t* base, short x, short y);
    void SetValue      (short val);
    };

//############################################
class LFO_METER_WIDGET_C
    {
private:
    lv_obj_t*       Meter;
    METER_ELEMENT_S MeterFreq;
    bool            SoftwareLFO;

    void InfoLine (lv_obj_t* base, METER_ELEMENT_S &element, const char* s, const char* su, short y, uint32_t color);

public:
         LFO_METER_WIDGET_C (lv_obj_t* base, short x, short y, bool software);
    void SetFreq            (short val);
    };

//############################################
class NOISE_WIDGET_C
    {
    private:
        lv_style_t  StyleBlue;
        lv_style_t  StyleWhite;
        lv_style_t  StylePink;
        lv_style_t  StyleRed;
        lv_obj_t*   Blue;
        lv_obj_t*   White;
        lv_obj_t*   Pink;
        lv_obj_t*   Red;

    public:
             NOISE_WIDGET_C (lv_obj_t* base,  lv_align_t align, short x, short y);
        void Set            (short val, bool state);
    };


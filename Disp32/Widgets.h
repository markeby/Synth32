//#######################################################################
// Module:     Widgets.h
// Descrption: Widget models
// Creator:    markeby
// Date:       7/7/2024
//#######################################################################
#pragma once
#include <lvgl.h>

extern lv_style_t  GlobalKeyStyle;

//############################################
void KeyLabel (lv_obj_t* base, const char* s, short x, short y);

//############################################
class TEXT_INFO_C
    {
private:
    lv_style_t  StyleLabel;
    lv_style_t  StyleValue;
    lv_obj_t*   Label;
    lv_obj_t*   Unit;
    lv_obj_t*   Value;
    bool        Valid;

public:
          TEXT_INFO_C   (void);
    void  BeginText     (lv_obj_t* base, const char* s, const char* su, short y, uint32_t color);

    void BeginText (lv_obj_t* base, const char* s, const char* su, short y)     { BeginText (base, s, su, y, 0xD0D0D0);  }
    void SetValid (bool state)                                                  { Valid = state; }
    bool GetValid (void)                                                        { return (Valid); }
    void SetLabel (const char* s)                                               { lv_label_set_text (Value, s); }
    void SetLabelColor (uint32_t color)                                         { lv_style_set_text_color (&StyleLabel, lv_color_hex (color)); }
    void SetValueColor (uint32_t color)                                         { lv_style_set_text_color (&StyleValue, lv_color_hex (color)); }
    void TextFloat (float f)                                                    { lv_label_set_text_fmt (Value, "%.3f Hz", f); }
    void TextInt (int d)                                                        { lv_label_set_text_fmt (Value, "%d", d); }

    };

//############################################
class LFO_METER_WIDGET_C
    {
private:
    lv_obj_t*               Meter;
    lv_meter_indicator_t*   Gauge;
    lv_obj_t*               Led;
    TEXT_INFO_C             MeterFreq;
    bool                    SoftwareLFO;

public:
         LFO_METER_WIDGET_C (lv_obj_t* base, short x, short y, bool software, const char* estr);
    void SetFreq            (short val);
    void Select             (bool sel);
    };

//############################################
class TITLE_WIDGET_C
    {
private:
    lv_obj_t*   Label;
    lv_style_t  Style;

public:
            TITLE_WIDGET_C      (lv_obj_t* base, const char* s);
            TITLE_WIDGET_C      (lv_obj_t* base, const char* s, int yoff);
    void    Begin               (lv_obj_t* base, const char* s, int yoff);
    };

//############################################
class ADSR_METER_WIDGET_C
    {
private:
    lv_obj_t*               Meter;
    lv_obj_t*               Led;
    lv_meter_indicator_t*   GaugeAttack;
    TEXT_INFO_C             Attack;
    lv_meter_indicator_t*   GaugeDecay;
    TEXT_INFO_C             Decay;
    lv_meter_indicator_t*   GaugeRelease;
    TEXT_INFO_C             Release;
    bool                    LedRed;
    byte                    LedDamper;

public:
          ADSR_METER_WIDGET_C (lv_obj_t* base, short x, short y);
    void  Volume              (lv_obj_t* base);
    void  Damper              (byte mode);
    void  Mute                (bool sel);
    void  SetAttack           (int val);
    void  SetDecay            (int val);
    void  SetRelease          (int val);

    void  Select              (int val)
        { Damper (val); }
    };

//############################################
class LEVEL_WIDGET_C
    {
private:
    lv_obj_t*       Panel;
    lv_obj_t*       Fader;
    short           SliderSize;
    lv_style_t      StyleLabel;
    lv_style_t      StyleValue;
    lv_style_t      StyleMain;
    lv_style_t      StyleIndicator;
    lv_style_t      StyleKnob;
    lv_obj_t*       Label;
    lv_obj_t*       Unit;
    lv_obj_t*       Value;
    float           Multiplier;
    lv_palette_t    Palette;

public:
         LEVEL_WIDGET_C (lv_obj_t* base, const char* title, const char* ks, short x, short y, lv_palette_t p);
    void Active         (bool state);
    void SetLevel       (int val);
    };

//############################################
class RAMP_WIDGET_C
    {
private:
    lv_style_t  StyleFore;
    lv_style_t  StyleBack;
    lv_obj_t*   SlopeFore;
    lv_obj_t*   SlopeBack;

public:
         RAMP_WIDGET_C (lv_obj_t* base, const char* s, lv_align_t align, short x, short y);
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

public:
         PULSE_WIDGET_C (lv_obj_t* base, const char* s, lv_align_t align, short x, short y);
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
    void SetValue      (short val);         // range 0 - 4095
    };

//############################################
class TUNE_OSC_WIDGET_C
    {
    private:
        lv_style_t  Style;
        lv_obj_t*   Osc[VOICE_COUNT];

    public:
             TUNE_OSC_WIDGET_C (lv_obj_t* base, short x, short y);
        void Set               (short chan);
        void Set2              (short chan);
    };

//############################################
class TUNE_FLT_WIDGET_C
    {
    private:
        lv_obj_t*   Panel;
        lv_style_t  Style;
        lv_obj_t*   Flt[5];

    public:
             TUNE_FLT_WIDGET_C (lv_obj_t* base, short x, short y);
        void Set               (byte sel);
    };

//############################################
class VALUE_WIDGET_C
    {
    private:
        lv_style_t  Style;
        lv_style_t  StyleValue;
        lv_obj_t*   Value;
        String      PreStr;

    public:
             VALUE_WIDGET_C (lv_obj_t* base, short x, short y, const char* str);
        void Set            (short val);
    };

//############################################
class SELECT_WIDGET_C
    {
    private:
        lv_obj_t*   Button;
        lv_obj_t*   Label;
        lv_obj_t*   Roller;
        lv_style_t  StyleSelect;
        uint16_t    Current;

    public:
             SELECT_WIDGET_C (lv_obj_t* base, const char* s, short x, short y, short width, const char* items, short offset);
        void Select          (bool state);
        void Set             (short val);
    };



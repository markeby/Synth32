//#######################################################################
// Module:     Widgets.cpp
// Descrption: Widget models
// Creator:    markeby
// Date:       7/7/2024
//#######################################################################
#include <Arduino.h>
#include <ESP_Panel_Library.h>
#include <lvgl.h>

#include "lvgl_port_v8.h"
#include "config.h"
#include "Widgets.h"

//#######################################################################
//#######################################################################
void KeyLabel (lv_obj_t* base, const char* s, short x, short y)
    {
    lv_obj_t* keys = lv_label_create (base);
    lv_obj_align      (keys, LV_ALIGN_DEFAULT, x, y);
    lv_label_set_text (keys, s);
    lv_obj_add_style  (keys, &GlobalKeyStyle, 0);
    }

//#######################################################################
//#######################################################################
    TEXT_INFO_C::TEXT_INFO_C ()
    {
    Label = nullptr;
    Unit  = nullptr;
    Value = nullptr;
    Valid = false;
    lv_style_init          (&StyleLabel);
    lv_style_set_text_font (&StyleLabel, &lv_font_montserrat_14);
    lv_style_init          (&StyleValue);
    lv_style_set_text_font (&StyleValue, &lv_font_montserrat_14);
    }

//#######################################################################
void TEXT_INFO_C::BeginText (lv_obj_t* base, const char* s, const char* su, short y, uint32_t color)
    {
    short x = 2;

    Label = lv_label_create (base);
    lv_obj_align            (Label, LV_ALIGN_TOP_MID, -42, y);
    lv_label_set_text       (Label, s);
    lv_obj_add_style        (Label, &StyleLabel, 0);
    lv_style_set_text_color (&StyleLabel, lv_color_hex (color));

    Value = lv_label_create (base);
    lv_obj_align            (Value, LV_ALIGN_TOP_MID, 5, y);
    lv_obj_add_style        (Value, &StyleValue, 0);
    lv_style_set_text_color (&StyleValue, lv_color_hex (color));

    if ( strlen (su) > 0 )
        {
        Unit = lv_label_create (base);
        lv_obj_align           (Unit, LV_ALIGN_TOP_MID, 45, y);
        lv_label_set_text      (Unit, su);
        lv_obj_add_style       (Unit, &StyleValue, 0);
        }
    }

//#######################################################################
//#######################################################################
    TITLE_WIDGET_C::TITLE_WIDGET_C (lv_obj_t* base, const char* s, int yoff)
    {
    Begin (base, s, yoff);
    }

//#######################################################################
    TITLE_WIDGET_C::TITLE_WIDGET_C (lv_obj_t* base, const char* s)
    {
    Begin (base, s, 0);
    }

//#######################################################################
void TITLE_WIDGET_C::Begin (lv_obj_t* base, const char* s, int yoff)
    {
    lv_style_init             (&_Style);
    lv_style_set_text_font    (&_Style, &lv_font_montserrat_18);
    _Label  = lv_label_create (base);
    lv_obj_align              (_Label, LV_ALIGN_TOP_MID, 3, yoff);
    lv_label_set_text         (_Label, s);
    lv_obj_add_style          (_Label, &_Style, 0);
    }


//#######################################################################
//#######################################################################
SUBTITLE_WIDGET_C::SUBTITLE_WIDGET_C (lv_obj_t* base, const char* s, int xoff)
    {
    lv_style_init             (&_Style);
    lv_style_set_text_font    (&_Style, &lv_font_montserrat_20);
    _Label  = lv_label_create (base);
    lv_obj_align              (_Label, LV_ALIGN_TOP_LEFT, xoff, 12);
    lv_label_set_text         (_Label, s);
    lv_obj_add_style          (_Label, &_Style, 0);
    }


//#######################################################################
//#######################################################################
    SELECT_WIDGET_C::SELECT_WIDGET_C (lv_obj_t* base, const char* s, short x, short y, short width, const char* items, short offset)
    {
    lv_obj_t* panel = lv_obj_create (base);
    lv_obj_set_size             (panel, width, 106);
    lv_obj_set_pos              (panel, x, y);
    lv_obj_set_style_pad_top    (panel, 0, 0);
    lv_obj_set_style_pad_bottom (panel, 0, 0);
    lv_obj_set_style_pad_left   (panel, 2, 0);
    lv_obj_set_style_pad_right  (panel, 2, 0);

    lv_style_init (&StyleSelect);
    lv_style_set_text_font (&StyleSelect, &lv_font_montserrat_14);

    Roller = lv_roller_create       (panel);
    lv_roller_set_options           (Roller, items, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count (Roller, 3);
    lv_obj_add_style                (Roller, &StyleSelect, LV_PART_SELECTED);
    lv_obj_align                    (Roller, LV_ALIGN_LEFT_MID, 0, 0);
    Current = 0;
    lv_roller_set_selected (Roller, Current, LV_ANIM_ON);

    Button = lv_btn_create (panel);
    lv_obj_align           (Button, LV_ALIGN_CENTER, offset, 0);
    lv_obj_add_flag        (Button, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height      (Button, LV_SIZE_CONTENT);

    Label = lv_label_create (Button);
    lv_label_set_text       (Label, s);
    lv_obj_center           (Label);
    }

//#######################################################################
void SELECT_WIDGET_C::Select (bool state)
    {
    if ( state )
        lv_obj_add_state (Button, LV_STATE_CHECKED);
    else
        lv_obj_clear_state (Button, LV_STATE_CHECKED);
    }

//#######################################################################
void SELECT_WIDGET_C::Set (short val)
    {
    Current = val;
    lv_roller_set_selected (Roller, Current, LV_ANIM_ON);
    }

//#######################################################################
//#######################################################################
    LFO_METER_WIDGET_C::LFO_METER_WIDGET_C (lv_obj_t* base, short x, short y, bool software, const char* estr)
    {
    SoftwareLFO = software;
    Meter = lv_meter_create (base);
    lv_obj_align (Meter, LV_ALIGN_TOP_LEFT, x, y);
    lv_obj_set_size (Meter, 140, 140);

    lv_obj_remove_style (Meter, NULL, LV_PART_INDICATOR);       // Remove the circle from the middle

    lv_meter_scale_t* scale = lv_meter_add_scale (Meter);
    lv_meter_set_scale_ticks (Meter, scale, 6, 2, 30, lv_color_hex3 (0x444));

    Gauge = lv_meter_add_arc (Meter, scale, 6, lv_palette_main (LV_PALETTE_RED), 2);
    Led  = lv_led_create (Meter);
    lv_obj_align (Led, LV_ALIGN_CENTER, 0, 0);

    y = 160;
    MeterFreq.BeginText (base, estr, "", y, 0x0000F0);
    // Initial positions
    SetFreq (0);
    Select (false);
    }

//#######################################################################
void LFO_METER_WIDGET_C::SetFreq (short val)
    {
    float zf;

    if ( SoftwareLFO )
        {
        zf = val * 0.014652;
        if ( zf < 0.015 )   zf = 0.015;
        }
    else
        {
        zf = val * 0.014652;
        if ( zf < 0.015 )   zf = 0.015;
        }
    MeterFreq.TextFloat (zf);
    lv_meter_set_indicator_end_value (Meter, Gauge, zf * 1.6666);
    }

//#######################################################################
void LFO_METER_WIDGET_C::Select (bool sel)
    {
    if ( sel )
        {
        lv_led_set_color (Led, lv_palette_main (LV_PALETTE_RED));
        lv_led_on (Led);
        }
    else
        {
        lv_led_set_color (Led,  lv_color_white ());
        lv_led_on (Led);
        }
    }

//#######################################################################
//#######################################################################
    ADSR_METER_WIDGET_C::ADSR_METER_WIDGET_C (lv_obj_t* base, short x, short y)
    {
    Meter = lv_meter_create (base);
    lv_obj_align (Meter, LV_ALIGN_TOP_MID, x, y);
    lv_obj_set_size (Meter, 122, 122);

    lv_obj_remove_style (Meter, NULL, LV_PART_INDICATOR);       // Remove the circle from the middle

    lv_meter_scale_t* scale = lv_meter_add_scale (Meter);
    lv_meter_set_scale_ticks (Meter, scale, 8, 2, 30, lv_color_hex3(0x444));

    GaugeAttack  = lv_meter_add_arc (Meter, scale, 5, lv_palette_main(LV_PALETTE_RED), 6);
    GaugeDecay   = lv_meter_add_arc (Meter, scale, 5, lv_palette_main(LV_PALETTE_BLUE), 13);
    GaugeRelease = lv_meter_add_arc (Meter, scale, 5, lv_palette_main(LV_PALETTE_GREEN), 20);

    Led  = lv_led_create (Meter);
    lv_obj_align (Led, LV_ALIGN_CENTER, 0, 0);

    y = 160;
    Attack.BeginText (base, " Attack:", "mSec", y, 0xff0000);
    y += 15;
    Decay.BeginText (base, "  Decay:", "mSec", y, 0x0000ff);
    y += 15;
    Release.BeginText (base, "Release:", "mSec", y, 0x008000);

    // Initial positions
    SetAttack  (0);
    SetDecay   (0);
    SetRelease (0);
    LedRed  = false;
    Damper (false);
    }

//#######################################################################
void ADSR_METER_WIDGET_C::Damper (byte mode)
    {
    lv_color_t   color;
    LedDamper = mode;

    if ( LedRed )
        return;

    switch ( LedDamper )
        {
        case 0:     // DAMPER::OFF
            color = lv_color_white ();
            break;
        case 1:     // DAMPER::NORMAL
            color = lv_palette_main (LV_PALETTE_TEAL);
            break;
        case 2:     // DAMPER::INVERT
            color = lv_palette_main (LV_PALETTE_BLUE);
            break;
        default:
            return;     // if we get here, something really screwed up...
            break;
        }

    lv_led_set_color (Led, color);
    lv_led_on (Led);
    }

//#######################################################################
void ADSR_METER_WIDGET_C::Mute (bool sel)
    {
    if ( sel )
        {
        LedRed = true;
        lv_led_set_color (Led, lv_palette_main (LV_PALETTE_RED));
        lv_led_on (Led);
        }
    else
        {
        LedRed = false;
        Damper (LedDamper);
        }
    }

//#######################################################################
void ADSR_METER_WIDGET_C::SetAttack (int val)
    {
    lv_meter_set_indicator_end_value (Meter, GaugeAttack, val);
    Attack.TextInt (val * TIME_MULT);
    }

//#######################################################################
void ADSR_METER_WIDGET_C::SetDecay (int val)
    {
    lv_meter_set_indicator_end_value (Meter, GaugeDecay, val);
    Decay.TextInt (val * TIME_MULT);
    }

//#######################################################################
void ADSR_METER_WIDGET_C::SetRelease (int val)
    {
    lv_meter_set_indicator_end_value (Meter, GaugeRelease, val);
    Release.TextInt (val * TIME_MULT);
    }

//#######################################################################
//#######################################################################
    LEVEL_WIDGET_C::LEVEL_WIDGET_C (lv_obj_t* base, const char* title, const char* ks, short x, short y, short w, lv_palette_t p)
    {
    short size = 127;

    Palette = p;                                    // Save Fader primary color
    lv_color_t color = lv_palette_lighten (p, 1);

    // Create the panel for placing the fader
    Panel = lv_obj_create (base);
    lv_obj_set_size             (Panel, w, size + 50);
    lv_obj_set_pos              (Panel, x, y);
    lv_obj_set_style_pad_top    (Panel, 1, 0);
    lv_obj_set_style_pad_left   (Panel, 2, 0);
    lv_obj_set_style_pad_right  (Panel, 2, 0);
    lv_obj_set_style_pad_bottom (Panel, 1, 0);

    // Label the panel at the top
    lv_style_init           (&StyleLabel);
    lv_style_set_text_font  (&StyleLabel, &lv_font_montserrat_12);
    lv_style_set_text_color (&StyleLabel, color);
    Label  = lv_label_create (Panel);
    lv_obj_align            (Label, LV_ALIGN_TOP_MID, 0, 0);
    lv_label_set_text       (Label, title);
    Title = title;
    lv_obj_add_style        (Label, &StyleLabel, 0);

    lv_style_init         (&StyleMain);
    lv_style_set_bg_opa   (&StyleMain, LV_OPA_COVER);
    lv_style_set_bg_color (&StyleMain, lv_color_hex3 (0x999));
    lv_style_set_radius   (&StyleMain, LV_RADIUS_CIRCLE);
    lv_style_set_width    (&StyleMain, 5);
    lv_style_set_height   (&StyleMain, size);

    lv_style_init         (&StyleIndicator);
    lv_style_set_bg_opa   (&StyleIndicator, LV_OPA_COVER);
    lv_style_set_bg_color (&StyleIndicator, color);

    lv_style_init             (&StyleKnob);
    lv_style_set_bg_opa       (&StyleKnob, LV_OPA_COVER);
    lv_style_set_bg_color     (&StyleKnob, color);
    lv_style_set_border_color (&StyleKnob, lv_palette_main (LV_PALETTE_BLUE_GREY));
    lv_style_set_pad_hor      (&StyleKnob, 8);
    lv_style_set_pad_ver      (&StyleKnob, 1);

    // instantiate the fader
    Fader = lv_slider_create (Panel);
    lv_obj_remove_style_all (Fader);        // Remove the styles coming from the theme
    lv_slider_set_range     (Fader, 0, size);
    lv_obj_align            (Fader, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_add_style        (Fader, &StyleMain, LV_PART_MAIN);
    lv_obj_add_style        (Fader, &StyleIndicator, LV_PART_INDICATOR);
    lv_obj_add_style        (Fader, &StyleKnob, LV_PART_KNOB);

    Value = lv_label_create (Panel);
    lv_obj_align            (Value, LV_ALIGN_BOTTOM_MID, 0, -2);
    lv_style_init           (&StyleValue);
    lv_style_set_text_font  (&StyleValue, &lv_font_montserrat_14);
    lv_style_set_text_color (&StyleValue, color);
    lv_obj_add_style        (Value, &StyleValue, 0);

    lv_obj_t* keys = lv_label_create (Panel);
    lv_obj_align      (keys, LV_ALIGN_LEFT_MID, 3, 0);    // half way from top to bottom
    lv_label_set_text (keys, ks);
    lv_obj_add_style  (keys, &GlobalKeyStyle, 0);

    Multiplier = (float)size / 127.0;
    // Initial position
    SetLevel (0);
    }

//#######################################################################
void LEVEL_WIDGET_C::Active (bool state)
    {
    lv_color_t color;

    if ( state )
        color = lv_palette_lighten (Palette, 1);
    else
        color = lv_palette_lighten (Palette, 5);

    lv_style_set_text_color (&StyleLabel,     color);
//    lv_style_set_bg_color   (&StyleIndicator, color);
    lv_style_set_bg_color   (&StyleKnob,      color);
//    lv_style_set_text_color (&StyleValue,     color);
    lv_obj_invalidate       (Panel);
    }

//#######################################################################
void LEVEL_WIDGET_C::Mute (bool state)
    {
    lv_color_t color;
    String     s;

    if ( state )
        {
        s = "MUTE";
        color = lv_palette_main (LV_PALETTE_RED);
        }
    else
        {
        s = Title;
        color = lv_palette_lighten (Palette, 1);
        }

    lv_label_set_text       (Label, s.c_str ());
    lv_style_set_text_color (&StyleLabel,     color);
    lv_style_set_bg_color   (&StyleIndicator, color);
    lv_style_set_bg_color   (&StyleKnob,      color);
    lv_style_set_text_color (&StyleValue,     color);
    lv_obj_invalidate       (Panel);
    }

//#######################################################################
void LEVEL_WIDGET_C::SetLevel (int val)
    {
    lv_slider_set_value   (Fader, (int32_t)((float)val * Multiplier), LV_ANIM_OFF);
    lv_label_set_text_fmt (Value, "%d%%", (int)(val / 1.27));
    }

//#######################################################################
//#######################################################################
static lv_point_t slopeBack[] = { {0, 20}, {0, 0}, {40, 20}, {40, 0}, {80, 20}};
static lv_point_t slopeFore[] = { {0, 20}, {40, 0}, {40, 20}, {80, 0}, {80, 20}};

    RAMP_WIDGET_C::RAMP_WIDGET_C (lv_obj_t* base, const char* s, lv_align_t align, short x, short y)
    {
    lv_obj_t* keys = lv_label_create (base);
    lv_obj_align (keys, align, x - 60, y);
    lv_label_set_text (keys, s);
    lv_obj_add_style (keys, &GlobalKeyStyle, 0);

    lv_style_init (&StyleFore);
    lv_style_set_line_width (&StyleFore, 3);
    lv_style_set_line_color (&StyleFore, lv_palette_main (LV_PALETTE_BLUE));
    lv_style_set_line_opa (&StyleFore, LV_OPA_TRANSP);
    lv_style_set_line_rounded (&StyleFore, true);

    lv_style_init (&StyleBack);
    lv_style_set_line_width (&StyleBack, 3);
    lv_style_set_line_color (&StyleBack, lv_palette_main (LV_PALETTE_BLUE));
    lv_style_set_line_opa (&StyleBack, LV_OPA_TRANSP);
    lv_style_set_line_rounded (&StyleBack, true);

    SlopeFore = lv_line_create (base);
    lv_line_set_points (SlopeFore, slopeFore, 5);
    lv_obj_align (SlopeFore, align, x, y);
    lv_obj_add_style (SlopeFore, &StyleFore, 0);

    SlopeBack = lv_line_create (base);
    lv_line_set_points (SlopeBack, slopeBack, 5);
    lv_obj_align (SlopeBack, align, x, y);
    lv_obj_add_style (SlopeBack, &StyleBack, 0);

    SetDir (true);
    }

//#######################################################################
void RAMP_WIDGET_C::SetDir (bool dir)
    {
    if ( dir )
        {
        lv_style_set_line_opa (&StyleFore, LV_OPA_TRANSP);
        lv_obj_add_style (SlopeFore, &StyleFore, 0);
        lv_style_set_line_opa (&StyleBack, LV_OPA_100);
        lv_obj_add_style (SlopeBack, &StyleBack, 0);
        }
    else
        {
        lv_style_set_line_opa (&StyleFore, LV_OPA_100);
        lv_obj_add_style (SlopeFore, &StyleFore, 0);
        lv_style_set_line_opa (&StyleBack, LV_OPA_TRANSP);
        lv_obj_add_style (SlopeBack, &StyleBack, 0);
        }
    }

//#######################################################################
//#######################################################################
static lv_point_t squareWave[SQUARE_SIZE] = { {0, 0}, {0, 20}, {0, 20}, {0, 0}, {40, 0}, {40, 20}, {40, 20}, {40, 0}, {80, 0}, {80, 20}};

    PULSE_WIDGET_C::PULSE_WIDGET_C (lv_obj_t* base, const char* s, lv_align_t align, short x, short y)
    {
    lv_obj_t* keys = lv_label_create (base);
    lv_obj_align (keys, align, x - 60, y);
    lv_label_set_text (keys, s);
    lv_obj_add_style (keys, &GlobalKeyStyle, 0);

    lv_style_init (&Style);
    lv_style_set_line_width (&Style, 2);
    lv_style_set_line_color (&Style, lv_palette_main (LV_PALETTE_BLUE));
    lv_style_set_line_rounded (&Style, true);

    memcpy (&Pulse, &squareWave, sizeof (lv_point_t) * SQUARE_SIZE);

    Wave = lv_line_create (base);
    lv_line_set_points (Wave, Pulse, SQUARE_SIZE);
    lv_obj_align (Wave, align, x, y);
    lv_obj_add_style (Wave, &Style, 0);
    }

//#######################################################################
void PULSE_WIDGET_C::SetWidth (short width)
    {
    float percent = (float)width / 4095.0;
    width = (short)(40.0 * percent);
    Pulse[2].x = squareWave[2].x + width;
    Pulse[3].x = squareWave[3].x + width;
    Pulse[6].x = squareWave[6].x + width;
    Pulse[7].x = squareWave[7].x + width;
    lv_line_set_points (Wave, Pulse, SQUARE_SIZE);
    }

//#######################################################################
//#######################################################################
static const char* FreqNote[12] =
   { "C ",   "C#"     ,"D ",    "Eb",     "E ",     "F ",     "Gb",     "G " ,   "G#",    "A ",    "Bb",    "B " };
   // ——      ——        ——       ——        ——        ——        ——        ——       ——       ——       ——       —— 
static const float FreqTable[128] =
 {   8.18,    8.66,    9.18,    9.72,    10.30,    10.91,    11.56,    12.25,   12.98,   13.75,   14.57,   15.43,
    16.35,   17.32,   18.35,   19.45,    20.60,    21.83,    23.12,    24.50,   25.96,   27.50,   29.14,   30.87,
    32.70,   34.65,   36.71,   38.89,    41.20,    43.65,    46.25,    49.00,   51.91,   55.00,   58.27,   61.74,
    65.41,   69.30,   73.42,   77.78,    82.41,    87.31,    92.50,    98.00,  103.83,  110.00,  116.54,  123.47,
   130.81,  138.59,  146.83,  155.56,   164.81,   174.61,   185.00,   196.00,  207.65,  220.00,  233.08,  246.94,
   261.63,  277.18,  293.66,  311.13,   329.63,   349.23,   369.99,   392.00,  415.30,  440.00,  466.16,  493.88,
   523.25,  554.37,  587.33,  622.25,   659.26,   698.46,   739.99,   783.99,  830.61,  880.00,  932.33,  987.77,
  1.04650, 1.10873, 1.17466, 1.24451,  1.31851,  1.39691,  1.47998,  1.56798, 1.66122, 1.76000, 1.86466, 1.97553,
  2.09300, 2.21746, 2.34932, 2.48902,  2.63702,  2.79383,  2.95996,  3.13596, 3.32244, 3.52000, 3.72931, 3.95107,
  4.18601, 4.43492, 4.69864, 4.97803,  5.27404,  5.58765,  5.91991,  6.27193, 6.64488, 7.04000, 7.45862, 7.90213,
  8.37202, 8.86984, 9.39727, 9.95606, 1.054808, 1.117530,  1.1839,   1.254385                                    };

//#######################################################################
    NOTE_WIDGET_C::NOTE_WIDGET_C (lv_obj_t* base, short x, short y)
    {
    lv_obj_t* panel = lv_obj_create (base);
    lv_obj_set_size (panel, 210, 28);
    lv_obj_set_pos (panel, x, y);
    lv_obj_set_style_pad_top (panel, 1, 0);
    lv_obj_set_style_pad_left (panel, 2, 0);
    lv_obj_set_style_pad_right (panel, 2, 0);
    lv_obj_set_style_pad_bottom (panel, 1, 0);

    Value = lv_label_create (panel);
    lv_obj_align (Value, LV_ALIGN_CENTER, 0, 0);
    lv_style_init (&StyleValue);
    lv_style_set_text_font (&StyleValue, &lv_font_montserrat_18);
    lv_style_set_text_color (&StyleValue, lv_palette_main (LV_PALETTE_DEEP_PURPLE));
    lv_obj_add_style (Value, &StyleValue, 0);

    SetValue (0);
    }

//#######################################################################
void NOTE_WIDGET_C::SetValue (short val)
    {
    char c = ' ';

    val %= 128;
    if ( val == 108 )
        c = '+';
    if ( val == 12 )
        c = '-';
    if ( val < 84 )
        lv_label_set_text_fmt(Value, "%c %s %d = %.2f Hz ", c, FreqNote[val % 12], val, FreqTable[val]);
    else
        lv_label_set_text_fmt(Value, "%c %s %d = %.4f kHz", c, FreqNote[val % 12], val, FreqTable[val]);
    }

//#######################################################################
//#######################################################################
static const char* fltText[] = { "OSC", "LP", "LBP", "UBP", "HP" };
    TUNE_OSC_WIDGET_C::TUNE_OSC_WIDGET_C (lv_obj_t* base, short x, short y)
    {
    lv_obj_t * panel = lv_obj_create (base);
    lv_obj_set_size (panel, 268, 42);
    lv_obj_set_pos (panel, x, y);
    lv_obj_set_style_pad_top (panel, 1, 0);
    lv_obj_set_style_pad_left (panel, 2, 0);
    lv_obj_set_style_pad_right (panel, 2, 0);
    lv_obj_set_style_pad_bottom (panel, 1, 0);

    lv_obj_t* keys = lv_label_create (panel);
    lv_obj_add_style (keys, &GlobalKeyStyle, 0);
    lv_obj_align (keys, LV_ALIGN_BOTTOM_LEFT, 8, -20);
    lv_label_set_text (keys, "F1    F2      F3      F4      F5     F6     F7     F8");

    x = 8;
    lv_style_init (&Style);
    lv_style_set_text_font (&Style, &lv_font_montserrat_14);
    for ( short z = 0;  z < VOICE_COUNT;  z++, x+=31 )
        {
        Osc[z] = lv_label_create (panel);
        lv_label_set_recolor (Osc[z], true);
        lv_obj_align (Osc[z], LV_ALIGN_BOTTOM_LEFT, x, -4);
        lv_obj_add_style (Osc[z], &Style, 0);
        lv_label_set_text_fmt (Osc[z], "#D0D0D0 %d#", z + 1);
        }
    }

//#######################################################################
void TUNE_OSC_WIDGET_C::Set (short chan)
    {
    for ( short z = 0;  z < VOICE_COUNT;  z++ )
        lv_label_set_text_fmt (Osc[z], "#D0D0D0 %d#", z + 1);
    lv_label_set_text_fmt (Osc[chan], "#000000 %d#", chan + 1);
    }

//#######################################################################
void TUNE_OSC_WIDGET_C::Set2 (short chan)
    {
    lv_label_set_text_fmt (Osc[chan], "#000000 %d#", chan);
    }

//#######################################################################
    TUNE_FLT_WIDGET_C::TUNE_FLT_WIDGET_C (lv_obj_t* base, short x, short y)
    {
    Panel = lv_obj_create (base);
    lv_obj_set_size             (Panel, 201, 42);
    lv_obj_set_pos              (Panel, x, y);
    lv_obj_set_style_pad_top    (Panel, 1, 0);
    lv_obj_set_style_pad_left   (Panel, 2, 0);
    lv_obj_set_style_pad_right  (Panel, 2, 0);
    lv_obj_set_style_pad_bottom (Panel, 1, 0);

    lv_obj_t* keys = lv_label_create (Panel);
    lv_obj_add_style (keys, &GlobalKeyStyle, 0);
    lv_obj_align (keys, LV_ALIGN_BOTTOM_LEFT, 19, -20);
    lv_label_set_text (keys, "F9     F10        F11       F12     F13");

    x = 8;
    lv_style_init (&Style);
    lv_style_set_text_font (&Style, &lv_font_montserrat_14);
    for ( short z = 0;  z < 5;  z++, x+=39 )
        {
        Flt[z] = lv_label_create (Panel);
        lv_label_set_recolor (Flt[z], true);
        lv_obj_align (Flt[z], LV_ALIGN_BOTTOM_LEFT, x, -4);
        lv_obj_add_style (Flt[z], &Style, 0);
        lv_label_set_text_fmt (Flt[z], "#D0D0D0 %s#", fltText[z]);
        }
    }

//#######################################################################
void TUNE_FLT_WIDGET_C::Set (byte sel)
    {
    for ( short z = 0;  z < 5;  z++ )
        {
        if ( (sel >> z) & 1 )
            lv_label_set_text_fmt (Flt[z], "#000000 %s#", fltText[z]);
        else
            lv_label_set_text_fmt (Flt[z], "#D0D0D0 %s#", fltText[z]);
        }
    }

//#######################################################################
//#######################################################################
    VALUE_WIDGET_C::VALUE_WIDGET_C (lv_obj_t* base, short x, short y, const char* str)
    {
    PreStr = str;
    lv_obj_t * panel = lv_obj_create (base);
    lv_obj_set_size (panel, 120, 32);
    lv_obj_set_pos (panel, x, y);
    lv_obj_set_style_pad_top (panel, 1, 0);
    lv_obj_set_style_pad_left (panel, 2, 0);
    lv_obj_set_style_pad_right (panel, 2, 0);
    lv_obj_set_style_pad_bottom (panel, 1, 0);

    Value = lv_label_create (panel);
    lv_obj_align (Value, LV_ALIGN_CENTER, 0, 0);
    lv_style_init (&StyleValue);
    lv_style_set_text_font (&StyleValue, &lv_font_montserrat_18);
    lv_style_set_text_color (&StyleValue, lv_palette_main (LV_PALETTE_DEEP_PURPLE));
    lv_obj_add_style (Value, &StyleValue, 0);

    Set (0);
    }

//#######################################################################
void VALUE_WIDGET_C::Set (short val)
    {
    lv_label_set_text_fmt (Value, "%s%d", PreStr.c_str (), val);
    }



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

void KeyLabel (lv_obj_t* base, const char* s, short x, short y)
    {
    lv_obj_t* keys = lv_label_create (base);
    lv_obj_align (keys, LV_ALIGN_DEFAULT, x, y);
    lv_label_set_text (keys, s);
    lv_obj_add_style (keys, &GlobalKeyStyle, 0);
    }

//#######################################################################
//#######################################################################
    TEXT_INFO_C::TEXT_INFO_C ()
    {
    this->Label = nullptr;
    this->Unit  = nullptr;
    this->Value = nullptr;
    this->Valid = false;
    lv_style_init (&this->StyleLabel);
    lv_style_set_text_font (&this->StyleLabel, &lv_font_montserrat_14);
    lv_style_init (&this->StyleValue);
    lv_style_set_text_font (&this->StyleValue, &lv_font_montserrat_14);
    }

//#######################################################################
void TEXT_INFO_C::BeginText (lv_obj_t* base, const char* s, const char* su, short y, uint32_t color)
    {
    short x = 2;

    this->Label = lv_label_create (base);
    lv_obj_align (this->Label, LV_ALIGN_TOP_LEFT, 0, y);
    lv_label_set_text (this->Label, s);
    lv_obj_add_style (this->Label, &this->StyleLabel, 0);
    lv_style_set_text_color (&this->StyleLabel, lv_color_hex (color));

    this->Value = lv_label_create(base);
    lv_obj_align (this->Value, LV_ALIGN_TOP_LEFT, 62, y);
    lv_obj_add_style (this->Value, &this->StyleValue, 0);
    lv_style_set_text_color (&this->StyleValue, lv_color_hex (color));

    if ( strlen (su) > 0 )
        {
        this->Unit = lv_label_create(base);
        lv_obj_align (this->Unit, LV_ALIGN_TOP_RIGHT, -11, y);
        lv_label_set_text (this->Unit, su);
        lv_obj_add_style (this->Unit, &this->StyleValue, 0);
        }
    }

//#######################################################################
//#######################################################################
    TITLE_WIDGET_C::TITLE_WIDGET_C (lv_obj_t* base, const char* s)
    {
    lv_style_init (&this->Style);
    lv_style_set_text_font (&this->Style, &lv_font_montserrat_18);
    this->Label  = lv_label_create (base);
    lv_obj_align (this->Label, LV_ALIGN_TOP_MID, 3, 0);
    lv_label_set_text (this->Label, s);
    lv_obj_add_style (this->Label, &this->Style, 0);
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

    lv_style_init (&this->StyleSelect);
    lv_style_set_text_font (&this->StyleSelect, &lv_font_montserrat_14);

    this->Roller = lv_roller_create (panel);
    lv_roller_set_options (this->Roller, items, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count (this->Roller, 3);
    lv_obj_add_style (this->Roller, &this->StyleSelect, LV_PART_SELECTED);
    lv_obj_align (this->Roller, LV_ALIGN_LEFT_MID, 0, 0);
    this->Current = 0;
    lv_roller_set_selected (this->Roller, this->Current, LV_ANIM_ON);

    this->Button = lv_btn_create (panel);
    lv_obj_align      (this->Button, LV_ALIGN_CENTER, offset, 0);
    lv_obj_add_flag   (this->Button, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height (this->Button, LV_SIZE_CONTENT);

    this->Label = lv_label_create (this->Button);
    lv_label_set_text (this->Label, s);
    lv_obj_center     (this->Label);
    }

//#######################################################################
void SELECT_WIDGET_C::Select (bool state)
    {
    if ( state )
        lv_obj_add_state (this->Button, LV_STATE_CHECKED);
    else
        lv_obj_clear_state (this->Button, LV_STATE_CHECKED);
    }

//#######################################################################
void SELECT_WIDGET_C::Set (short val)
    {
    this->Current = val;
    lv_roller_set_selected (this->Roller, this->Current, LV_ANIM_ON);
    }

//#######################################################################
//#######################################################################
    LFO_METER_WIDGET_C::LFO_METER_WIDGET_C (lv_obj_t* base, short x, short y, bool software, const char* estr)
    {
    SoftwareLFO = software;
    this->Meter = lv_meter_create (base);
    lv_obj_align (this->Meter, LV_ALIGN_TOP_LEFT, x, y);
    lv_obj_set_size (this->Meter, 140, 140);

    lv_obj_remove_style (this->Meter, NULL, LV_PART_INDICATOR);       // Remove the circle from the middle

    lv_meter_scale_t* scale = lv_meter_add_scale (this->Meter);
    lv_meter_set_scale_ticks (this->Meter, scale, 6, 2, 30, lv_color_hex3 (0x444));

    this->Gauge = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main (LV_PALETTE_RED), 2);
    this->Led  = lv_led_create (Meter);
    lv_obj_align (this->Led, LV_ALIGN_CENTER, 0, 0);

    y = 160;
    this->MeterFreq.BeginText (base, estr, "", y, 0x0000F0);
    // Initial positions
    this->SetFreq (0);
    this->Select (false);
    }

//#######################################################################
void LFO_METER_WIDGET_C::SetFreq (short val)
    {
    float zf;

    if ( this->SoftwareLFO )
        {
        zf = val * 0.014652;
        if ( zf < 0.015 )   zf = 0.015;
        }
    else
        {
        zf = val * 0.014652;
        if ( zf < 0.015 )   zf = 0.015;
        }
    this->MeterFreq.TextFloat (zf);
    lv_meter_set_indicator_end_value (this->Meter, this->Gauge, zf * 1.6666);
    }

//#######################################################################
void LFO_METER_WIDGET_C::Select (bool sel)
    {
    if ( sel )
        {
        lv_led_set_color (this->Led, lv_palette_main (LV_PALETTE_RED));
        lv_led_on (this->Led);
        }
    else
        {
        lv_led_set_color (this->Led,  lv_color_white ());
        lv_led_on (this->Led);
        }
    }

//#######################################################################
//#######################################################################
    ADSR_METER_WIDGET_C::ADSR_METER_WIDGET_C (lv_obj_t* base, short x, short y)
    {
    this->Meter = lv_meter_create (base);
    lv_obj_align (this->Meter, LV_ALIGN_TOP_MID, x, y);
    lv_obj_set_size (this->Meter, 140, 140);

    lv_obj_remove_style (this->Meter, NULL, LV_PART_INDICATOR);       // Remove the circle from the middle

    lv_meter_scale_t* scale = lv_meter_add_scale (this->Meter);
    lv_meter_set_scale_ticks (this->Meter, scale, 6, 2, 30, lv_color_hex3(0x444));

    this->GaugeAttack  = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main(LV_PALETTE_GREEN), 20);
    this->GaugeDecay   = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main(LV_PALETTE_BLUE), 11);
    this->GaugeRelease = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main(LV_PALETTE_RED), 2);

    this->Led  = lv_led_create (this->Meter);
    lv_obj_align (this->Led, LV_ALIGN_CENTER, 0, 0);

    y = 160;
    this->Attack.BeginText (base, " Attack:", "mSec", y, 0x008000);
    y += 15;
    this->Decay.BeginText (base, "  Decay:", "mSec", y, 0x0000ff);
    y += 15;
    this->Release.BeginText (base, "Release:", "mSec", y, 0xff0000);

    // Initial positions
    this->SetAttack  (0);
    this->SetDecay   (0);
    this->SetRelease (0);
    this->Select     (false);
    }

//#######################################################################
void ADSR_METER_WIDGET_C::Select (bool sel)
    {
    if ( sel )
        {
        lv_led_set_color (Led, lv_palette_main (LV_PALETTE_RED));
        lv_led_on (Led);
        }
    else
        {
        lv_led_set_color (Led,  lv_color_white());
        lv_led_on (Led);
        }
    }

//#######################################################################
void ADSR_METER_WIDGET_C::SetAttack (int val)
    {
    lv_meter_set_indicator_end_value (this->Meter, this->GaugeAttack, val);
    this->Attack.TextInt (val * TIME_MULT);
    }

//#######################################################################
void ADSR_METER_WIDGET_C::SetDecay (int val)
    {
    lv_meter_set_indicator_end_value (this->Meter, this->GaugeDecay, val);
    this->Decay.TextInt (val * TIME_MULT);
    }

//#######################################################################
void ADSR_METER_WIDGET_C::SetRelease (int val)
    {
    lv_meter_set_indicator_end_value (this->Meter, this->GaugeRelease, val);
    this->Release.TextInt (val * TIME_MULT);
    }

//#######################################################################
//#######################################################################
    LEVEL_WIDGET_C::LEVEL_WIDGET_C (lv_obj_t* base, const char* s, short x, short y, lv_palette_t p)
    {
    short size = 127;

    lv_obj_t* panel = lv_obj_create (base);
    lv_obj_set_size (panel, 75, size + 50);
    lv_obj_set_pos (panel, x, y);
    lv_obj_set_style_pad_top (panel, 1, 0);
    lv_obj_set_style_pad_left (panel, 2, 0);
    lv_obj_set_style_pad_right (panel, 2, 0);
    lv_obj_set_style_pad_bottom (panel, 1, 0);

    lv_style_init (&this->StyleLabel);
    lv_style_set_text_font (&this->StyleLabel, &lv_font_montserrat_12);
    lv_style_set_text_color (&this->StyleLabel, lv_palette_main (p));
    this->Label  = lv_label_create (panel);
    lv_obj_align (this->Label, LV_ALIGN_TOP_MID, 0, 0);
    lv_label_set_text (this->Label, s);
    lv_obj_add_style (this->Label, &this->StyleLabel, 0);

    lv_style_init (&this->StyleMain);
    lv_style_set_bg_opa (&this->StyleMain, LV_OPA_COVER);
    lv_style_set_bg_color (&this->StyleMain, lv_color_hex3 (0x999));
    lv_style_set_radius (&this->StyleMain, LV_RADIUS_CIRCLE);
    lv_style_set_width (&this->StyleMain, 5);
    lv_style_set_height (&this->StyleMain, size);

    lv_style_init (&this->StyleIndicator);
    lv_style_set_bg_opa (&this->StyleIndicator, LV_OPA_COVER);
    lv_style_set_bg_color (&this->StyleIndicator, lv_palette_lighten (p, 3));

    lv_style_init (&this->StyleKnob);
    lv_style_set_bg_opa (&this->StyleKnob, LV_OPA_COVER);
    lv_style_set_bg_color (&this->StyleKnob, lv_palette_main (p));
    lv_style_set_border_color (&this->StyleKnob, lv_palette_main (LV_PALETTE_BLUE_GREY));
    lv_style_set_pad_hor (&this->StyleKnob, 8);
    lv_style_set_pad_ver (&this->StyleKnob, 1);

    this->Slider = lv_slider_create (panel);
    lv_obj_remove_style_all (this->Slider);        // Remove the styles coming from the theme
    lv_slider_set_range (this->Slider, 0, size);
    lv_obj_align (this->Slider, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_add_style (this->Slider, &this->StyleMain, LV_PART_MAIN);
    lv_obj_add_style (this->Slider, &this->StyleIndicator, LV_PART_INDICATOR);
    lv_obj_add_style (this->Slider, &this->StyleKnob, LV_PART_KNOB);

    this->Value = lv_label_create (panel);
    lv_obj_align (this->Value, LV_ALIGN_BOTTOM_MID, 0, -2);
    lv_style_init (&this->StyleValue);
    lv_style_set_text_font (&this->StyleValue, &lv_font_montserrat_14);
    lv_style_set_text_color (&this->StyleValue, lv_palette_main (p));
    lv_obj_add_style (this->Value, &this->StyleValue, 0);

    this->Multiplier = (float)size / 127.0;
    // Initial position
    this->SetLevel (0);
    }

//#######################################################################
void LEVEL_WIDGET_C::SetLevel (int val)
    {
    lv_slider_set_value (Slider, (int32_t)((float)val * Multiplier), LV_ANIM_OFF);
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

    lv_style_init (&this->StyleFore);
    lv_style_set_line_width (&this->StyleFore, 3);
    lv_style_set_line_color (&this->StyleFore, lv_palette_main (LV_PALETTE_BLUE));
    lv_style_set_line_opa (&this->StyleFore, LV_OPA_TRANSP);
    lv_style_set_line_rounded (&this->StyleFore, true);

    lv_style_init (&this->StyleBack);
    lv_style_set_line_width (&this->StyleBack, 3);
    lv_style_set_line_color (&this->StyleBack, lv_palette_main (LV_PALETTE_BLUE));
    lv_style_set_line_opa (&this->StyleBack, LV_OPA_TRANSP);
    lv_style_set_line_rounded (&this->StyleBack, true);

    this->SlopeFore = lv_line_create (base);
    lv_line_set_points (this->SlopeFore, slopeFore, 5);
    lv_obj_align (this->SlopeFore, align, x, y);
    lv_obj_add_style (this->SlopeFore, &StyleFore, 0);

    this->SlopeBack = lv_line_create (base);
    lv_line_set_points (this->SlopeBack, slopeBack, 5);
    lv_obj_align (this->SlopeBack, align, x, y);
    lv_obj_add_style (this->SlopeBack, &StyleBack, 0);

    this->SetDir (true);
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
    this->Pulse[2].x = squareWave[2].x + width;
    this->Pulse[3].x = squareWave[3].x + width;
    this->Pulse[6].x = squareWave[6].x + width;
    this->Pulse[7].x = squareWave[7].x + width;
    lv_line_set_points (this->Wave, this->Pulse, SQUARE_SIZE);
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
  1046.50, 1108.73, 1174.66, 1244.51,  1318.51,  1396.91,  1479.98,  1567.98, 1661.22, 1760.00, 1864.66, 1975.53,
  2093.00, 2217.46, 2349.32, 2489.02,  2637.02,  2793.83,  2959.96,  3135.96, 3322.44, 3520.00, 3729.31, 3951.07,
  4186.01, 4434.92, 4698.64, 4978.03,  5274.04,  5587.65,  5919.91,  6271.93, 6644.88, 7040.00, 7458.62, 7902.13,
  8372.02, 8869.84, 9397.27, 9956.06, 10548.08, 11175.30, 11839.82, 12543.85                                     };

//#######################################################################
    NOTE_WIDGET_C::NOTE_WIDGET_C (lv_obj_t* base, short x, short y)
    {
    lv_obj_t* panel = lv_obj_create (base);
    lv_obj_set_size (panel, 190, 28);
    lv_obj_set_pos (panel, x, y);
    lv_obj_set_style_pad_top (panel, 1, 0);
    lv_obj_set_style_pad_left (panel, 2, 0);
    lv_obj_set_style_pad_right (panel, 2, 0);
    lv_obj_set_style_pad_bottom (panel, 1, 0);

    this->Value = lv_label_create (panel);
    lv_obj_align (this->Value, LV_ALIGN_CENTER, 0, 0);
    lv_style_init (&this->StyleValue);
    lv_style_set_text_font (&this->StyleValue, &lv_font_montserrat_18);
    lv_style_set_text_color (&this->StyleValue, lv_palette_main (LV_PALETTE_DEEP_PURPLE));
    lv_obj_add_style (this->Value, &this->StyleValue, 0);

    this->SetValue (0);
    }

//#######################################################################
void NOTE_WIDGET_C::SetValue (short val)
    {
    char c = ' ';

    val %= 128;
    if ( val == 113 )
        c = '+';
    if ( val == 21 )
        c = '-';
    lv_label_set_text_fmt (this->Value, "%c%s %d = %.2f Hz", c, FreqNote[val % 12], val, FreqTable[val]);
    }

//#######################################################################
//#######################################################################
    TUNES_WIDGET_C::TUNES_WIDGET_C (lv_obj_t* base, short x, short y)
    {
    lv_obj_t * panel = lv_obj_create (base);
    lv_obj_set_size (panel, 254, 42);
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
    lv_style_init (&this->Style);
    lv_style_set_text_font (&this->Style, &lv_font_montserrat_14);
    for ( short z = 0;  z < VOICE_COUNT;  z++ )
        {
        this->Osc[z] = lv_label_create (panel);
        lv_label_set_recolor (this->Osc[z], true);
        lv_obj_align (this->Osc[z], LV_ALIGN_BOTTOM_LEFT, x, -4);
        lv_obj_add_style (this->Osc[z], &this->Style, 0);
        lv_label_set_text_fmt (Osc[z], "#D0D0D0 %d#", z);
        x += 31;
        }
    }

//#######################################################################
void TUNES_WIDGET_C::Set (short chan)
    {
    for ( short z = 0;  z < VOICE_COUNT;  z++ )
        lv_label_set_text_fmt (Osc[z], "#D0D0D0 %d#", z);
    lv_label_set_text_fmt (Osc[chan], "#000000 %d#", chan);
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

    this->Value = lv_label_create (panel);
    lv_obj_align (this->Value, LV_ALIGN_CENTER, 0, 0);
    lv_style_init (&this->StyleValue);
    lv_style_set_text_font (&this->StyleValue, &lv_font_montserrat_18);
    lv_style_set_text_color (&this->StyleValue, lv_palette_main (LV_PALETTE_DEEP_PURPLE));
    lv_obj_add_style (this->Value, &this->StyleValue, 0);

    this->Set (0);
    }

//#######################################################################
void VALUE_WIDGET_C::Set (short val)
    {
    lv_label_set_text_fmt (this->Value, "%s%d", PreStr.c_str (), val);
    }



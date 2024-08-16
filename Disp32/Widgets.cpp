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
    TITLE_WIDGET_C::TITLE_WIDGET_C (lv_obj_t* base, const char* s)
    {
    lv_style_init (&Style);
    lv_style_set_text_font (&Style, &lv_font_montserrat_18);
    Label  = lv_label_create (base);
    lv_obj_align (Label, LV_ALIGN_TOP_MID, 3, 0);
    lv_label_set_text (Label, s);
    lv_obj_add_style (Label, &Style, 0);
    }

//#######################################################################
//#######################################################################
    LFO_METER_WIDGET_C::LFO_METER_WIDGET_C (lv_obj_t* base, short x, short y, bool software)
    {
    SoftwareLFO = software;
    this->Meter = lv_meter_create (base);
    lv_obj_align (this->Meter, LV_ALIGN_TOP_MID, x, y);
    lv_obj_set_size (this->Meter, 140, 140);

    lv_obj_remove_style (this->Meter, NULL, LV_PART_INDICATOR);       // Remove the circle from the middle

    lv_meter_scale_t* scale = lv_meter_add_scale (this->Meter);
    lv_meter_set_scale_ticks (this->Meter, scale, 6, 2, 30, lv_color_hex3(0x444));

    this->MeterFreq.Gauge = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main(LV_PALETTE_BLUE), 20);

    y = 160;
    InfoLine (base, MeterFreq, " Freq:", "Hz", y, 0x0000F0);

    // Initial positions
    this->SetFreq (0);
    }

//#######################################################################
void LFO_METER_WIDGET_C::InfoLine (lv_obj_t* base, METER_ELEMENT_S &element, const char* s, const char* su, short y, uint32_t color)
    {
    element.Label = lv_label_create (base);
    lv_obj_align (element.Label, LV_ALIGN_TOP_LEFT, 0, y);
    lv_label_set_text (element.Label, s);
    lv_style_init (&element.Style);
    lv_style_set_text_font (&element.Style, &lv_font_montserrat_14);
    lv_style_set_text_color (&element.Style, lv_color_hex (color));
    lv_obj_add_style (element.Label, &element.Style, 0);

    if ( strlen (s) > 1 )
        {
        element.Value = lv_label_create(base);
        lv_obj_align (element.Value, LV_ALIGN_TOP_LEFT, 62, y);
        lv_obj_add_style (element.Value, &element.Style, 0);

        element.Unit = lv_label_create (base);
        lv_obj_align (element.Unit, LV_ALIGN_TOP_RIGHT, -11, y);
        lv_label_set_text (element.Unit, su);
        lv_obj_add_style (element.Unit, &element.Style, 0);
        }
    else
        {
        element.Value =  nullptr;
        element.Unit  =  nullptr;
        }
    }

//#######################################################################
void LFO_METER_WIDGET_C::SetFreq (short val)
    {
    float zf = val * 0.007874;

    if ( SoftwareLFO )
        zf *= 83.4;
    else
        {
        zf *= 83.4;
        if ( zf < 0.025 )
            zf = 0.025;
        }
    lv_label_set_text_fmt (MeterFreq.Value, "%.3f", zf);
    }

//#######################################################################
//#######################################################################
    ADSR_METER_WIDGET_C::ADSR_METER_WIDGET_C (lv_obj_t* base, short x, short y)
    {
    this->Meter = lv_meter_create (base);
    lv_obj_align (this->Meter, LV_ALIGN_TOP_MID, x, y);
    lv_obj_set_size (this->Meter, 140, 140);

    lv_obj_remove_style (this->Meter, NULL, LV_PART_INDICATOR);       // Remove the circle from the middle

    lv_meter_scale_t* scale = lv_meter_add_scale (Meter);
    lv_meter_set_scale_ticks (this->Meter, scale, 6, 2, 30, lv_color_hex3(0x444));

    this->Attack.Gauge  = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main(LV_PALETTE_GREEN), 20);
    this->Decay.Gauge   = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main(LV_PALETTE_BLUE), 13);
    this->Sustain.Gauge = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main(LV_PALETTE_PURPLE), 6);
    this->Release.Gauge = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main(LV_PALETTE_RED), -1);

    this->Led  = lv_led_create (Meter);
    lv_obj_align (this->Led, LV_ALIGN_CENTER, 0, 0);

    y = 160;
    InfoLine (base, this->Attack, " Attack:", y, 0x008000);
    y += 16;
    InfoLine (base, this->Decay, "  Decay:", y, 0x0000ff);
    y += 16;
    InfoLine (base, this->Sustain, "Sustain:", y, 0x800080);
    y += 16;
    InfoLine (base, this->Release, "Release:", y, 0xff0000);

    // Initial positions
    this->SetAttack  (0);
    this->SetDecay   (0);
    this->SetSustain (0);
    this->SetRelease (0);
    this->Select     (false);
    }

//#######################################################################
void ADSR_METER_WIDGET_C::InfoLine (lv_obj_t* base, METER_ELEMENT_S &element, const char* s, short y, uint32_t color)
    {
    element.Label = lv_label_create (base);
    lv_obj_align (element.Label, LV_ALIGN_TOP_LEFT, 0, y);
    lv_label_set_text (element.Label, s);
    lv_style_init (&element.Style);
    lv_style_set_text_font (&element.Style, &lv_font_montserrat_14);
    lv_style_set_text_color (&element.Style, lv_color_hex (color));
    lv_obj_add_style (element.Label, &element.Style, 0);

    element.Value = lv_label_create (base);
    lv_obj_align (element.Value, LV_ALIGN_TOP_LEFT, 62, y);
    lv_obj_add_style (element.Value, &element.Style, 0);

    element.Unit = lv_label_create (base);
    lv_obj_align (element.Unit, LV_ALIGN_TOP_RIGHT, -11, y);
    lv_label_set_text (element.Unit, "mSec");
    lv_obj_add_style (element.Unit, &element.Style, 0);
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
    lv_meter_set_indicator_end_value (Meter, Attack.Gauge, val);
    lv_label_set_text_fmt (Attack.Value, "%d", val * TIME_MULT);
    }

//#######################################################################
void ADSR_METER_WIDGET_C::SetDecay (int val)
    {
    lv_meter_set_indicator_end_value (Meter, Decay.Gauge, val);
    lv_label_set_text_fmt (Decay.Value, "%d", val * TIME_MULT);
    }

//#######################################################################
void ADSR_METER_WIDGET_C::SetSustain (int val)
    {
    lv_meter_set_indicator_end_value (Meter, Sustain.Gauge, val);
    lv_label_set_text_fmt (Sustain.Value, "%d", val * TIME_MULT);
    }

//#######################################################################
void ADSR_METER_WIDGET_C::SetRelease (int val)
    {
    lv_meter_set_indicator_end_value (Meter, Release.Gauge, val);
    lv_label_set_text_fmt (Release.Value, "%d", val * TIME_MULT);
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

    lv_style_init (&StyleLabel);
    lv_style_set_text_font (&StyleLabel, &lv_font_montserrat_12);
    lv_style_set_text_color (&StyleLabel, lv_palette_main(p));
    Label  = lv_label_create (panel);
    lv_obj_align (Label, LV_ALIGN_TOP_MID, 0, 0);
    lv_label_set_text (Label, s);
    lv_obj_add_style (Label, &StyleLabel, 0);

    lv_style_init (&StyleMain);
    lv_style_set_bg_opa (&StyleMain, LV_OPA_COVER);
    lv_style_set_bg_color (&StyleMain, lv_color_hex3(0x999));
    lv_style_set_radius (&StyleMain, LV_RADIUS_CIRCLE);
    lv_style_set_width (&StyleMain, 5);
    lv_style_set_height (&StyleMain, size);

    lv_style_init (&StyleIndicator);
    lv_style_set_bg_opa (&StyleIndicator, LV_OPA_COVER);
    lv_style_set_bg_color (&StyleIndicator, lv_palette_lighten(p, 3));

    lv_style_init (&StyleKnob);
    lv_style_set_bg_opa (&StyleKnob, LV_OPA_COVER);
    lv_style_set_bg_color (&StyleKnob, lv_palette_main(p));
    lv_style_set_border_color (&StyleKnob, lv_palette_main(LV_PALETTE_BLUE_GREY));
    lv_style_set_pad_hor (&StyleKnob, 8);
    lv_style_set_pad_ver (&StyleKnob, 1);

    Slider = lv_slider_create (panel);
    lv_obj_remove_style_all (Slider);        // Remove the styles coming from the theme
    lv_slider_set_range (Slider, 0, size);
    lv_obj_align (Slider, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_add_style (Slider, &StyleMain, LV_PART_MAIN);
    lv_obj_add_style (Slider, &StyleIndicator, LV_PART_INDICATOR);
    lv_obj_add_style (Slider, &StyleKnob, LV_PART_KNOB);

    Value = lv_label_create (panel);
    lv_obj_align (Value, LV_ALIGN_BOTTOM_MID, 0, -2);
    lv_style_init (&StyleValue);
    lv_style_set_text_font (&StyleValue, &lv_font_montserrat_14);
    lv_style_set_text_color (&StyleValue, lv_palette_main(p));
    lv_obj_add_style (Value, &StyleValue, 0);

    Multiplier = (float)size / 127.0;
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

    SAWTOOTH_WIDGET_C::SAWTOOTH_WIDGET_C (lv_obj_t* base,  lv_align_t align, short x, short y)
    {
    lv_style_init (&StyleFore);
    lv_style_set_line_width (&StyleFore, 3);
    lv_style_set_line_color (&StyleFore, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_line_opa (&StyleFore, LV_OPA_TRANSP);
    lv_style_set_line_rounded (&StyleFore, true);

    lv_style_init (&StyleBack);
    lv_style_set_line_width (&StyleBack, 3);
    lv_style_set_line_color (&StyleBack, lv_palette_main(LV_PALETTE_BLUE));
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
    }

//#######################################################################
void SAWTOOTH_WIDGET_C::SetDir (bool dir)
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

    PULSE_WIDGET_C::PULSE_WIDGET_C (lv_obj_t* base,  lv_align_t align, short x, short y)
    {
    lv_style_init (&Style);
    lv_style_set_line_width (&Style, 2);
    lv_style_set_line_color (&Style, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_line_rounded (&Style, true);

    memcpy (&Pulse, &squareWave, sizeof (lv_point_t) * SQUARE_SIZE);

    Wave = lv_line_create (base);
    lv_line_set_points (Wave, Pulse, SQUARE_SIZE);
    lv_obj_align (Wave, align, x, y);
    lv_obj_add_style (Wave, &Style, 0);
    }

void PULSE_WIDGET_C::SetWidth (short width)
    {
    float percent = (float)width / 127.0;
    width = (short)(40.0 * percent);
    Pulse[2].x = squareWave[2].x + width;
    Pulse[3].x = squareWave[3].x + width;
    Pulse[6].x = squareWave[6].x + width;
    Pulse[7].x = squareWave[7].x + width;
    lv_line_set_points (Wave, Pulse, SQUARE_SIZE);
    }


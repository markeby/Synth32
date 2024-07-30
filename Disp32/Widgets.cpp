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
    ADSR_WIDGET_C::ADSR_WIDGET_C (lv_obj_t* base, const char* s, short x, short y)
    {
    lv_obj_t * panel = lv_obj_create (base);
    lv_obj_set_size (panel, 176, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_top (panel, 1, 0);
    lv_obj_set_style_pad_left (panel, 5, 0);
    lv_obj_set_pos (panel, x, y);

    lv_style_init (&LabelStyle);
    lv_style_set_text_font (&LabelStyle, &lv_font_montserrat_18);
    Label  = lv_label_create (panel);
    lv_obj_set_align (Label, LV_ALIGN_TOP_MID);
    lv_obj_set_pos (Label, 3, 0);
    lv_label_set_text (Label, s);
    lv_obj_add_style (Label, &LabelStyle, 0);

    Meter = lv_meter_create (panel);
    lv_obj_set_align (Meter, LV_ALIGN_TOP_MID);
    lv_obj_set_pos (Meter, 0, 20);
    lv_obj_set_size (Meter, 140, 140);

    /*Remove the circle from the middle*/
    lv_obj_remove_style (Meter, NULL, LV_PART_INDICATOR);

    lv_meter_scale_t* scale = lv_meter_add_scale (Meter);
    lv_meter_set_scale_ticks (Meter, scale, 6, 2, 30, lv_color_hex3(0x444));

    Attack.Gauge  = lv_meter_add_arc (Meter, scale, 6, lv_palette_main(LV_PALETTE_GREEN), 20);
    Decay.Gauge   = lv_meter_add_arc (Meter, scale, 6, lv_palette_main(LV_PALETTE_BLUE), 12);
    Sustain.Gauge = lv_meter_add_arc (Meter, scale, 6, lv_palette_main(LV_PALETTE_PURPLE), 4);
    Release.Gauge = lv_meter_add_arc (Meter, scale, 6, lv_palette_main(LV_PALETTE_RED), -4);

    Led  = lv_led_create (Meter);
    lv_obj_align (Led, LV_ALIGN_CENTER, 0, 0);

    y = 162;
    Attack.Label = lv_label_create (panel);
    lv_obj_set_align (Attack.Label, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos (Attack.Label, 0, y);
    lv_label_set_text (Attack.Label, " Attack:");
    lv_style_init (&Attack.Style);
    lv_style_set_text_font (&Attack.Style, &lv_font_montserrat_14);
    lv_style_set_text_color(&Attack.Style, lv_color_hex(0x008000));
    lv_obj_add_style (Attack.Label, &Attack.Style, 0);

    Attack.Value = lv_label_create (panel);
    lv_obj_set_align (Attack.Value, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos (Attack.Value, 68, y);
    lv_obj_add_style (Attack.Value, &Attack.Style, 0);

    Attack.Unit = lv_label_create (panel);
    lv_obj_set_align (Attack.Unit, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_pos (Attack.Unit, 0, y);
    lv_label_set_text (Attack.Unit, "mSec");
    lv_obj_add_style (Attack.Unit, &Attack.Style, 0);

    y += 16;
    Decay.Label = lv_label_create (panel);
    lv_obj_set_align (Decay.Label, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos (Decay.Label, 0, y);
    lv_label_set_text (Decay.Label, "  Decay:");
    lv_style_init (&Decay.Style);
    lv_style_set_text_font (&Decay.Style, &lv_font_montserrat_14);
    lv_style_set_text_color(&Decay.Style, lv_color_hex(0x0000ff));
    lv_obj_add_style (Decay.Label, &Decay.Style, 0);

    Decay.Value = lv_label_create (panel);
    lv_obj_set_align (Decay.Value, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos (Decay.Value, 68, y);
    lv_obj_add_style (Decay.Value, &Decay.Style, 0);

    Decay.Unit = lv_label_create (panel);
    lv_obj_set_align (Decay.Unit, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_pos (Decay.Unit, 0, y);
    lv_label_set_text (Decay.Unit, "mSec");
    lv_obj_add_style (Decay.Unit, &Decay.Style, 0);

    y += 16;
    Sustain.Label = lv_label_create (panel);
    lv_obj_set_align (Sustain.Label, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos (Sustain.Label, 0, y);
    lv_label_set_text (Sustain.Label, "Sustain:");
    lv_style_init (&Sustain.Style);
    lv_style_set_text_font (&Sustain.Style, &lv_font_montserrat_14);
    lv_style_set_text_color(&Sustain.Style, lv_color_hex(0x800080));
    lv_obj_add_style (Sustain.Label, &Sustain.Style, 0);

    Sustain.Value = lv_label_create (panel);
    lv_obj_set_align (Sustain.Value, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos (Sustain.Value, 68, y);
    lv_obj_add_style (Sustain.Value, &Sustain.Style, 0);

    Sustain.Unit = lv_label_create (panel);
    lv_obj_set_align (Sustain.Unit, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_pos (Sustain.Unit, 0, y);
    lv_label_set_text (Sustain.Unit, "mSec");
    lv_obj_add_style (Sustain.Unit, &Sustain.Style, 0);

    y += 16;
    Release.Label = lv_label_create (panel);
    lv_obj_set_align (Release.Label, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos (Release.Label, 0, y);
    lv_label_set_text (Release.Label, "Release:");
    lv_style_init (&Release.Style);
    lv_style_set_text_font (&Release.Style, &lv_font_montserrat_14);
    lv_style_set_text_color(&Release.Style, lv_color_hex(0xff0000));
    lv_obj_add_style (Release.Label, &Release.Style, 0);

    Release.Value = lv_label_create (panel);
    lv_obj_set_align (Release.Value, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos (Release.Value, 68, y);
    lv_obj_add_style (Release.Value, &Release.Style, 0);

    Release.Unit = lv_label_create (panel);
    lv_obj_set_align (Release.Unit, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_pos (Release.Unit, 0, y);
    lv_label_set_text (Release.Unit, "mSec");
    lv_obj_add_style (Release.Unit, &Release.Style, 0);

    // Initial positions
    SetAttack  (0);
    SetDecay   (0);
    SetSustain (0);
    SetRelease (0);
    Select     (false);
    }

//#######################################################################
void ADSR_WIDGET_C::Select (bool sel)
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
void ADSR_WIDGET_C::SetAttack (int val)
    {
    lv_meter_set_indicator_end_value (Meter, Attack.Gauge, val);
    lv_label_set_text_fmt (Attack.Value, "%d", val * TIME_MULT);
    }

//#######################################################################
void ADSR_WIDGET_C::SetDecay (int val)
    {
    lv_meter_set_indicator_end_value (Meter, Decay.Gauge, val);
    lv_label_set_text_fmt (Decay.Value, "%d", val * TIME_MULT);
    }

//#######################################################################
void ADSR_WIDGET_C::SetSustain (int val)
    {
    lv_meter_set_indicator_end_value (Meter, Sustain.Gauge, val);
    lv_label_set_text_fmt (Sustain.Value, "%d", val * TIME_MULT);
    }

//#######################################################################
void ADSR_WIDGET_C::SetRelease (int val)
    {
    lv_meter_set_indicator_end_value (Meter, Release.Gauge, val);
    lv_label_set_text_fmt (Release.Value, "%d", val * TIME_MULT);
    }


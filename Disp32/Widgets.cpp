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
    ADSR_WIDGET_C::ADSR_WIDGET_C (const char* s, short x, short y)
    {
    lv_obj_t*  label;
    lv_style_t style;
    short      xoff = x + 64;

    lvgl_port_lock (-1);    // Lock the mutex due to the LVGL APIs are not thread-safe

    lv_style_init (&style);
    lv_style_set_text_font (&style, &lv_font_montserrat_18); // <--- you have to enable other font sizes in menuconfig
    label  = lv_label_create (lv_scr_act());
    lv_obj_add_style (label, &style, 0);
    lv_obj_set_pos (label, x + (70 - (strlen (s) * 5)), y);
    lv_label_set_text (label, s);

    this->Meter = lv_meter_create (lv_scr_act());
    y += 20;
    lv_obj_set_pos (this->Meter, x, y);
    lv_obj_set_size (this->Meter, 140, 140);

    /*Remove the circle from the middle*/
    lv_obj_remove_style (Meter, NULL, LV_PART_INDICATOR);

    lv_meter_scale_t* scale = lv_meter_add_scale (this->Meter);
    lv_meter_set_scale_ticks (this->Meter, scale, 6, 2, 30, lv_color_hex3(0x444));

    this->Attack.Gauge  = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main(LV_PALETTE_GREEN), 22);
    this->Decay.Gauge   = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main(LV_PALETTE_BLUE), 14);
    this->Sustain.Gauge = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main(LV_PALETTE_PURPLE), 6);
    this->Release.Gauge = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main(LV_PALETTE_RED), -2);

    this->Led  = lv_led_create (lv_scr_act ());
    lv_obj_align (this->Led, LV_ALIGN_TOP_LEFT, x + 58, y + 58);

    label = lv_label_create (lv_scr_act());
    y += 141;
    lv_obj_set_pos (label, x, y);
    lv_label_set_text (label, "#008000   Attack#:");
    lv_label_set_recolor (label, true);
    this->Attack.Label = lv_label_create (lv_scr_act());
    lv_obj_set_pos (this->Attack.Label, xoff, y);

    y += 12;
    label = lv_label_create (lv_scr_act());
    lv_obj_set_pos (label, x, y);
    lv_label_set_text (label, "#0000ff   Decay#:");
    lv_label_set_recolor (label, true);
    this->Decay.Label = lv_label_create (lv_scr_act());
    lv_obj_set_pos (this->Decay.Label, xoff, y);

    y += 12;
    label = lv_label_create (lv_scr_act());
    lv_obj_set_pos (label, x, y);
    lv_label_set_text (label, "#800080 Sustain#:");
    lv_label_set_recolor (label, true);
    this->Sustain.Label = lv_label_create (lv_scr_act());
    lv_obj_set_pos (this->Sustain.Label, xoff, y);

    y += 12;
    label = lv_label_create (lv_scr_act());
    lv_obj_set_pos (label, x, y);
    lv_label_set_text (label, "#ff0000 Release#:");
    lv_label_set_recolor (label, true);
    this->Release.Label = lv_label_create (lv_scr_act());
    lv_obj_set_pos (this->Release.Label, xoff, y);

    // Initial positions
    this->SetAttack  (0);
    this->SetDecay   (0);
    this->SetSustain (0);
    this->SetRelease (0);
    this->Select     (false);

    lvgl_port_unlock ();    // Release the mutex
    }

void ADSR_WIDGET_C::Select (bool sel)
    {
//    lvgl_port_lock (-1);
    if ( sel )
        {
        lv_led_set_color (this->Led, lv_palette_main (LV_PALETTE_RED));
        lv_led_on (this->Led);
        }
    else
        {
        lv_led_set_color (this->Led,  lv_color_white());
        lv_led_on (this->Led);
        }
//    lvgl_port_unlock ();
    }

void ADSR_WIDGET_C::SetAttack (int val)
    {
//    lvgl_port_lock (-1);
    lv_meter_set_indicator_end_value (this->Meter, this->Attack.Gauge, val);
    lv_label_set_text_fmt (this->Attack.Label, "%d", val);
//    lvgl_port_unlock ();
    }

void ADSR_WIDGET_C::SetDecay (int val)
    {
//    lvgl_port_lock (-1);
    lv_meter_set_indicator_end_value (this->Meter, this->Decay.Gauge, val);
    lv_label_set_text_fmt (this->Decay.Label, "%d", val);
//    lvgl_port_unlock ();
    }

void ADSR_WIDGET_C::SetSustain (int val)
    {
//    lvgl_port_lock (-1);
    lv_meter_set_indicator_end_value (this->Meter, this->Sustain.Gauge, val);
    lv_label_set_text_fmt (this->Sustain.Label, "%d", val);
//    lvgl_port_unlock ();
    }

void ADSR_WIDGET_C::SetRelease (int val)
    {
//    lvgl_port_lock (-1);
    lv_meter_set_indicator_end_value (this->Meter, this->Release.Gauge, val);
    lv_label_set_text_fmt (this->Release.Label, "%d", val);
//    lvgl_port_unlock ();
    }


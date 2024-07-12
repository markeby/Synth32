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
    ADSR_WIDGET_C::ADSR_WIDGET_C (const char* s, int x, int y)
    {
    lv_style_t style;

    lv_style_init(&style);
    lv_style_set_text_font(&style, &lv_font_montserrat_18); // <--- you have to enable other font sizes in menuconfig
    lv_obj_t* label  = lv_label_create (lv_scr_act());
    lv_obj_add_style (label, &style, 0);
    lv_obj_set_pos (label, x + (70 - (strlen (s) * 5)), y);
    lv_label_set_text (label, s);

    this->Meter = lv_meter_create (lv_scr_act());
    y += 20;
    lv_obj_set_pos (this->Meter, x, y);
    lv_obj_set_size (this->Meter, 140, 140);

    /*Remove the circle from the middle*/
    lv_obj_remove_style(Meter, NULL, LV_PART_INDICATOR);

    lv_meter_scale_t* scale = lv_meter_add_scale (this->Meter);
    lv_meter_set_scale_ticks (this->Meter, scale, 6, 2, 30, lv_color_hex3(0xeee));

    Attack  = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main(LV_PALETTE_GREEN), 22);
    Decay   = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main(LV_PALETTE_BLUE), 14);
    Sustain = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main(LV_PALETTE_PURPLE), 6);
    Release = lv_meter_add_arc (this->Meter, scale, 6, lv_palette_main(LV_PALETTE_RED), -2);

    lv_obj_t* label_attack  = lv_label_create (lv_scr_act());
    y += 141;
    lv_obj_set_pos (label_attack, x, y);
    lv_label_set_text (label_attack, "#008000  Attack#:");
    lv_label_set_recolor (label_attack, true);
    y += 12;
    lv_obj_t* label_decay   = lv_label_create (lv_scr_act());
    lv_obj_set_pos (label_decay, x, y);
    lv_label_set_text (label_decay, "#0000ff   Decay#:");
    lv_label_set_recolor (label_decay, true);
    y += 12;
    lv_obj_t* label_sustain = lv_label_create (lv_scr_act());
    lv_obj_set_pos (label_sustain, x, y);
    lv_label_set_text (label_sustain, "#800080 Sustain#:");
    lv_label_set_recolor (label_sustain, true);
    y += 12;
    lv_obj_t* label_release = lv_label_create (lv_scr_act());
    lv_obj_set_pos (label_release, x, y);
    lv_label_set_text (label_release, "#ff0000 Release#:");
    lv_label_set_recolor (label_release, true);


    SetAttack  (30);
    SetDecay   (45);
    SetSustain (100);
    SetRelease (60);
    }

void ADSR_WIDGET_C::SetAttack (int val)
    {
    lv_meter_set_indicator_end_value (this->Meter, this->Attack, val);
    }

void ADSR_WIDGET_C::SetDecay (int val)
    {
    lv_meter_set_indicator_end_value (this->Meter, this->Decay, val);
    }

void ADSR_WIDGET_C::SetSustain (int val)
    {
    lv_meter_set_indicator_end_value (this->Meter, this->Sustain, val);
    }

void ADSR_WIDGET_C::SetRelease (int val)
    {
    lv_meter_set_indicator_end_value (this->Meter, this->Release, val);
    }


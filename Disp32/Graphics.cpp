//#######################################################################
// Module:     Graphics.cpp
// Descrption: Graphics module
// Creator:    markeby
// Date:       7/7/2024
//#######################################################################
#define ALLOCATE_DISP_TEXT_REF 1

#include <Arduino.h>
#include <ESP_Panel_Library.h>
#include <lvgl.h>
#include "lvgl_port_v8.h"
#include <demos/lv_demos.h>

#include "config.h"
#include "Graphics.h"
#define   ALLOCATE_DISP_TEXT_REF 1
#include "../Common/DispMessages.h"
#include "Widgets.h"

using namespace DISP_MESSAGE_N;

//#######################################################################
    GRPH_C::GRPH_C ()
    {
    }

//#######################################################################
void GRPH_C::Begin ()
    {
    Panel = new ESP_Panel ();
    Panel->init ();
    Panel->begin ();
    lvgl_port_init (Panel->getLcd (), Panel->getTouch());

    lvgl_port_lock (-1);    // Lock the mutex due to the LVGL APIs are not thread-safe
    Pages = lv_tabview_create ( lv_scr_act(), LV_DIR_LEFT, 0);

    PageOsc = lv_tabview_add_tab (Pages, "");
    PageFilter = lv_tabview_add_tab (Pages, "");
    PageTuning = lv_tabview_add_tab (Pages, "");

    InitPageOsc    (PageOsc);
    InitPageFilter (PageFilter);
    InitPageTuning (PageTuning);

    lv_style_init (&TitleStyle);       // for page titles
    lv_style_set_text_font (&TitleStyle, &lv_font_montserrat_36);

    PageSelect (0);

    lvgl_port_unlock ();    // Release the mutex
    }

//#######################################################################
void GRPH_C::InitPageOsc (lv_obj_t* base)
    {
    int x = 0;
    int y = 40;

    lv_obj_set_style_pad_top    (base, 0, 0);
    lv_obj_set_style_pad_left   (base, 3, 0);
    lv_obj_set_style_pad_bottom (base, 0, 0);
    lv_obj_set_style_pad_right  (base, 3, 0);

    TitleOsc = lv_label_create (base);
    lv_obj_align      (TitleOsc, LV_ALIGN_TOP_MID, 0, 0);
    lv_label_set_text (TitleOsc, "OSCILLATORS");
    lv_obj_add_style  (TitleOsc, &TitleStyle, 0);

    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        lv_obj_t * panel = lv_obj_create (base);
        lv_obj_set_size (panel, 159, 440);
        lv_obj_set_pos (panel, x, y);
        lv_obj_set_style_pad_top (panel, 0, 0);
        lv_obj_set_style_pad_bottom (panel, 0, 0);
        lv_obj_set_style_pad_left (panel, 2, 0);
        lv_obj_set_style_pad_right (panel, 2, 0);

        TitleControl[z] = new TITLE_WIDGET_C(panel, ClassADSR[z]);
        MeterADSR[z]    = new ADSR_WIDGET_C(panel, ClassADSR[z], 0, 18);
        SustainLevel[z] = new LEVEL_WIDGET_C(panel, "SUSTAIN", 0, 225, LV_PALETTE_ORANGE);
        MaxLevel[z]     = new LEVEL_WIDGET_C(panel, "MAX", 73, 225, LV_PALETTE_INDIGO);

        if ( z == 2 )
            SawtoothDir = new SAWTOOTH_WIDGET_C(panel, LV_ALIGN_BOTTOM_MID, 0, -6);

        if ( z == 3 )
            PulseWidth = new PULSE_WIDGET_C(panel, LV_ALIGN_BOTTOM_MID, 0, -6);

        x += 158;
        }
    }


//#######################################################################
void GRPH_C::InitPageFilter (lv_obj_t* base)
    {
    lv_obj_set_style_pad_top    (base, 0, 0);
    lv_obj_set_style_pad_left   (base, 3, 0);
    lv_obj_set_style_pad_bottom (base, 0, 0);
    lv_obj_set_style_pad_right  (base, 3, 0);

    TitleFlt = lv_label_create (base);
    lv_obj_align      (TitleFlt, LV_ALIGN_TOP_MID, 0, 0);
    lv_label_set_text (TitleFlt, "FILTERS");
    lv_obj_add_style  (TitleFlt, &TitleStyle, 0);

    }

//#######################################################################
void GRPH_C::InitPageTuning (lv_obj_t* base)
    {
    Tuningfont = &lv_font_montserrat_48;

    lv_style_init (&TuningStyle);
    lv_style_set_text_font  (&TuningStyle, Tuningfont);
    lv_style_set_text_color (&TuningStyle, lv_color_hex(0xF00000));

    TuningTitle = lv_label_create(base);
    lv_obj_set_pos    (TuningTitle, 180, 180);
    lv_label_set_text (TuningTitle, "TUNING MODE");
    lv_obj_add_style  (TuningTitle, &TuningStyle, 0);
    }

//#######################################################################
void GRPH_C::Pause (bool state)
    {
    if ( state )
        lvgl_port_lock (-1);    // Lock the mutex due to the LVGL APIs are not thread-safe
    else
        lvgl_port_unlock ();    // Release the mutex
    }

//#######################################################################
void GRPH_C::PageSelect (byte page)
    {
    lv_tabview_set_act (Pages, page, LV_ANIM_OFF);
    }

//#######################################################################
void GRPH_C::UpdatePage (byte ch, byte effect, short value)
    {
    switch ( (EFFECT_C)effect )
        {
        case EFFECT_C::SELECTED:
            MeterADSR[ch]->Select (value);
            break;
        case EFFECT_C::BASE_VOL:
            break;
        case EFFECT_C::MAX_LEVEL:
            MaxLevel[ch]->SetLevel (value);
            break;
        case EFFECT_C::ATTACK_TIME:
            MeterADSR[ch]->SetAttack (value);
            break;
        case EFFECT_C::DECAY_TIME:
            MeterADSR[ch]->SetDecay (value);
            break;
        case EFFECT_C::SUSTAIN_TIME:
            MeterADSR[ch]->SetSustain (value);
            break;
        case EFFECT_C::RELEASE_TIME:
            MeterADSR[ch]->SetRelease (value);
            break;
        case EFFECT_C::SUSTAIN_LEVEL:
            SustainLevel[ch]->SetLevel (value);
            break;
        case EFFECT_C::SAWTOOTH_DIRECTION:
            SawtoothDir->SetDir (value);
            break;
        case EFFECT_C::PULSE_WIDTH:
            PulseWidth->SetWidth (value);
            break;
        default:
            break;
        }
    }

GRPH_C  Graphics;

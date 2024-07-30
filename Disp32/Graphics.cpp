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

//    lvgl_port_lock (-1);    // Lock the mutex due to the LVGL APIs are not thread-safe
    Pages = lv_tabview_create( lv_scr_act(), LV_DIR_LEFT, 0);

    PageOsc = lv_tabview_add_tab (Pages, "");
    PageFilter = lv_tabview_add_tab (Pages, "");
    PageTuning = lv_tabview_add_tab( Pages, "");

    InitPageOsc (PageOsc);
    InitPageFilter (PageFilter);
    InitPageTuning (PageTuning);

//    lvgl_port_unlock ();    // Release the mutex

    PageSelect (0);
    }

//#######################################################################
void GRPH_C::InitPageOsc (lv_obj_t* base)
    {
    int x = 0;
    int y = 0;

//    lvgl_port_lock (-1);
//    lv_obj_set_flex_flow(base, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_top (base, 0, 0);
    lv_obj_set_style_pad_left (base, 0, 0);
    lv_obj_set_style_pad_bottom (base, 0, 0);
    lv_obj_set_style_pad_right (base, 0, 0);

    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        MeterADSR[z] = new ADSR_WIDGET_C(base, ClassADSR[z], x, y);
        x += 155;
        }
//    lvgl_port_unlock ();
    }


//#######################################################################
void GRPH_C::InitPageFilter (lv_obj_t* base)
    {



    }

//#######################################################################
void GRPH_C::InitPageTuning (lv_obj_t* base)
    {
    Tuningfont = &lv_font_montserrat_48;

    lv_style_init (&TuningStyle);
    lv_style_set_text_font (&TuningStyle, Tuningfont);
    lv_style_set_text_color(&TuningStyle, lv_color_hex(0xF00000));

    TuningTitle = lv_label_create(base);
    lv_obj_set_pos (TuningTitle, 180, 180);
    lv_label_set_text(TuningTitle, "TUNING MODE");
    lv_obj_add_style(TuningTitle, &TuningStyle, 0);
    }

//#######################################################################
void GRPH_C::Pause (bool state)
    {
    if ( state )
//        lvgl_port_lock (-1);    // Lock the mutex due to the LVGL APIs are not thread-safe
        lvgl_port_unlock ();    // Release the mutex
    else
        lvgl_port_unlock ();    // Release the mutex
    }

//#######################################################################
void GRPH_C::PageSelect (byte page)
    {
    lv_tabview_set_act (Pages, page, LV_ANIM_ON);
//    lv_tabview_set_act (Pages, page, LV_ANIM_OFF);
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
            break;
        case EFFECT_C::SAWTOOTH_DIRECTION:
            break;
        default:
            break;
        }
    }

GRPH_C  Graphics;

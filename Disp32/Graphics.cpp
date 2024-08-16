//#######################################################################
// Module:     Graphics.cpp
// Descrption: Graphics module
// Creator:    markeby
// Date:       7/7/2024
//#######################################################################
#define  ALLOCATE_DISP_MESSAGES 1  // This can only be set in one cpp file and no other files

#include <Arduino.h>
#include <ESP_Panel_Library.h>
#include <lvgl.h>
#include "lvgl_port_v8.h"
#include <demos/lv_demos.h>

#include "config.h"
#include "Debug.h"
#include "Graphics.h"
#include "Widgets.h"

static const char* Label = "G";
#define DBG(args...) {if(DebugGraphics){ DebugMsg(Label,DEBUG_NO_INDEX,args);}}

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

    PageOsc    = lv_tabview_add_tab (Pages, "");
    PageMod    = lv_tabview_add_tab (Pages, "");
    PageFilter = lv_tabview_add_tab (Pages, "");
    PageTuning = lv_tabview_add_tab (Pages, "");

    InitPageOsc    (PageOsc);
    InitPageMod    (PageMod);
    InitPageFilter (PageFilter);
    InitPageTuning (PageTuning);

    lv_style_init (&TitleStyle);       // for page titles
    lv_style_set_text_font (&TitleStyle, &lv_font_montserrat_36);

    PageSelect (PAGE_C::PAGE_OSC);

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

        TitleControl[z] = new TITLE_WIDGET_C(panel, ChannelText[z]);
        MeterADSR[z]    = new ADSR_METER_WIDGET_C(panel, 0, 18);
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
void GRPH_C::InitPageMod (lv_obj_t* base)
    {
    lv_obj_t*   panel;
    int         x = 0;
    int         y = 40;

    lv_obj_set_style_pad_top    (base, 0, 0);
    lv_obj_set_style_pad_left   (base, 3, 0);
    lv_obj_set_style_pad_bottom (base, 0, 0);
    lv_obj_set_style_pad_right  (base, 3, 0);

    TitleMod = lv_label_create (base);
    lv_obj_align      (TitleMod, LV_ALIGN_TOP_MID, 0, 0);
    lv_label_set_text (TitleMod, "Modulators");
    lv_obj_add_style  (TitleMod, &TitleStyle, 0);

    panel = lv_obj_create (base);
    lv_obj_set_size (panel, 159, 440);
    lv_obj_set_pos (panel, x, y);
    lv_obj_set_style_pad_top (panel, 0, 0);
    lv_obj_set_style_pad_bottom (panel, 0, 0);
    lv_obj_set_style_pad_left (panel, 2, 0);
    lv_obj_set_style_pad_right (panel, 2, 0);

    TitleSoftware = new TITLE_WIDGET_C(panel, "Software");
    MeterSoftware = new LFO_METER_WIDGET_C(panel, 0, 18, true);

    x += 158;

    panel = lv_obj_create (base);
    lv_obj_set_size (panel, 159, 440);
    lv_obj_set_pos (panel, x, y);
    lv_obj_set_style_pad_top (panel, 0, 0);
    lv_obj_set_style_pad_bottom (panel, 0, 0);
    lv_obj_set_style_pad_left (panel, 2, 0);
    lv_obj_set_style_pad_right (panel, 2, 0);

    TitleHardware = new TITLE_WIDGET_C(panel, "Hardware");
    MeterHardware = new LFO_METER_WIDGET_C(panel, 0, 18, false);
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
void GRPH_C::PageSelect (PAGE_C page)
    {
    if ( page == PAGE_C::PAGE_ADVANCE )
        {
        page = (PAGE_C)((byte)this->CurrentPage + 1);
        if ( page == PAGE_C::PAGE_TUNING )
            page = PAGE_C::PAGE_OSC;
        }
    if ( CurrentPage != page )
        {
        CurrentPage = page;
        DBG ("Page Select %s (%d)", PageText[(byte)page], (byte)page);
        lv_tabview_set_act(Pages, (byte)page, LV_ANIM_OFF);
        }
    }

//#######################################################################
void GRPH_C::UpdatePageMod (byte ch, EFFECT_C effect, short value)
    {
    switch ( (CHANNEL_C)ch )
        {
        case CHANNEL_C::HARDWARE_LFO:
            switch ( effect )
                {
                case EFFECT_C::SELECTED:
                    break;
                case EFFECT_C::FREQ_LFO:
                    this->MeterHardware->SetFreq (value);
                    break;
                case EFFECT_C::SAWTOOTH_DIRECTION:
                    break;
                case EFFECT_C::PULSE_WIDTH:
                    break;
                default:
                    break;
                }
            break;
        case CHANNEL_C::SOFTWARE_LFO:
            switch ( effect )
                {
                case EFFECT_C::SELECTED:
                    break;
                case EFFECT_C::FREQ_LFO:
                    this->MeterSoftware->SetFreq (value);
                    break;
                default:
                    break;
                }
            break;
        default:
            break;
        }


    }

//#######################################################################
void GRPH_C::UpdatePageOsc (byte ch, EFFECT_C effect, short value)
    {
    switch ( effect )
        {
        case EFFECT_C::SELECTED:
            this->MeterADSR[ch]->Select (value);
            break;
        case EFFECT_C::BASE_VOL:
            break;
        case EFFECT_C::MAX_LEVEL:
            this->MaxLevel[ch]->SetLevel (value);
            break;
        case EFFECT_C::ATTACK_TIME:
            this->MeterADSR[ch]->SetAttack (value);
            break;
        case EFFECT_C::DECAY_TIME:
            this->MeterADSR[ch]->SetDecay (value);
            break;
        case EFFECT_C::SUSTAIN_TIME:
            this->MeterADSR[ch]->SetSustain (value);
            break;
        case EFFECT_C::RELEASE_TIME:
            this->MeterADSR[ch]->SetRelease (value);
            break;
        case EFFECT_C::SUSTAIN_LEVEL:
            this->SustainLevel[ch]->SetLevel (value);
            break;
        case EFFECT_C::SAWTOOTH_DIRECTION:
            this->SawtoothDir->SetDir (value);
            break;
        case EFFECT_C::PULSE_WIDTH:
            this->PulseWidth->SetWidth (value);
            break;
        default:
            break;
        }
    }

GRPH_C  Graphics;

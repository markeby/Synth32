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

    PAGE_TITLE_C::PAGE_TITLE_C (lv_obj_t* base, const char* str)
    {
    lv_obj_set_style_pad_top    (base, 0, 0);
    lv_obj_set_style_pad_left   (base, 3, 0);
    lv_obj_set_style_pad_bottom (base, 0, 0);
    lv_obj_set_style_pad_right  (base, 3, 0);

    Title = lv_label_create (base);
    lv_obj_align      (Title, LV_ALIGN_TOP_MID, 0, 0);
    lv_label_set_text (Title, str);
    lv_style_init (&TitleStyle);       // for page titles
    lv_style_set_text_font (&TitleStyle, &lv_font_montserrat_36);
    lv_obj_add_style  (Title, &TitleStyle, 0);
    }

//#######################################################################
//#######################################################################
    PAGE_OSC_C::PAGE_OSC_C (lv_obj_t* base, const char* str) : PAGE_TITLE_C (base, str)
    {
    int x = 0;
    int y = 40;

    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        lv_obj_t * panel = lv_obj_create (base);
        lv_obj_set_size (panel, 159, 440);
        lv_obj_set_pos (panel, x, y);
        lv_obj_set_style_pad_top (panel, 0, 0);
        lv_obj_set_style_pad_bottom (panel, 0, 0);
        lv_obj_set_style_pad_left (panel, 2, 0);
        lv_obj_set_style_pad_right (panel, 2, 0);

        TitleControl[z] = new TITLE_WIDGET_C (panel, ChannelText[z]);
        MeterADSR[z]    = new ADSR_METER_WIDGET_C (panel, 0, 18);
        SustainLevel[z] = new LEVEL_WIDGET_C (panel, "SUSTAIN", 0, 225, LV_PALETTE_ORANGE);
        MaxLevel[z]     = new LEVEL_WIDGET_C (panel, "MAX", 73, 225, LV_PALETTE_INDIGO);

        switch ( z )
            {
            case 2:
                RampDir = new RAMP_WIDGET_C (panel, LV_ALIGN_BOTTOM_MID, 0, -6);
                break;
            case 3:
                PulseWidth = new PULSE_WIDGET_C (panel, LV_ALIGN_BOTTOM_MID, 0, -6);
                break;
            case 4:
                Noise = new NOISE_WIDGET_C (panel, LV_ALIGN_BOTTOM_MID, 0, -8);
                break;
            default:
                break;
            }

        x += 158;
        }
    }

//#######################################################################
void PAGE_OSC_C::UpdatePage (byte ch, EFFECT_C effect, short value)
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
        case EFFECT_C::RELEASE_TIME:
            this->MeterADSR[ch]->SetRelease (value);
            break;
        case EFFECT_C::SUSTAIN_LEVEL:
            this->SustainLevel[ch]->SetLevel (value);
            break;
        case EFFECT_C::SAWTOOTH_DIRECTION:
            this->RampDir->SetDir (value);
            break;
        case EFFECT_C::PULSE_WIDTH:
            this->PulseWidth->SetWidth (value);
            break;
        case EFFECT_C::NOISE:
            this->Noise->Set (value & 0x0F, value >> 7);
            break;
        default:
            break;
        }
    }

//#######################################################################
//#######################################################################
     PAGE_MOD_C::PAGE_MOD_C (lv_obj_t* base) : PAGE_TITLE_C (base, "Modulators")
    {
    lv_obj_t*   panel;
    int         x = 0;
    int         y = 40;

    panel = lv_obj_create (base);
    lv_obj_set_size             (panel, 159, 320);
    lv_obj_set_pos              (panel, x, y);
    lv_obj_set_style_pad_top    (panel, 0, 0);
    lv_obj_set_style_pad_bottom (panel, 0, 0);
    lv_obj_set_style_pad_left   (panel, 2, 0);
    lv_obj_set_style_pad_right  (panel, 2, 0);

    TitleSoftware = new TITLE_WIDGET_C (panel, "Software");
    MeterSoftware = new LFO_METER_WIDGET_C (panel, 0, 18, true);

    x += 158;

    panel = lv_obj_create (base);
    lv_obj_set_size             (panel, 159, 320);
    lv_obj_set_pos              (panel, x, y);
    lv_obj_set_style_pad_top    (panel, 0, 0);
    lv_obj_set_style_pad_bottom (panel, 0, 0);
    lv_obj_set_style_pad_left   (panel, 2, 0);
    lv_obj_set_style_pad_right  (panel, 2, 0);

    TitleHardware = new TITLE_WIDGET_C (panel, "Hardware");
    MeterHardware = new LFO_METER_WIDGET_C (panel, 0, 18, false);
    RampDir       = new RAMP_WIDGET_C (panel, LV_ALIGN_BOTTOM_MID, 0, -36);
    PulseWidth    = new PULSE_WIDGET_C (panel, LV_ALIGN_BOTTOM_MID, 0, -6);

    SelectSine  = false;
    SelectRamp  = false;
    SelectPulse = false;
    }

//#######################################################################
void PAGE_MOD_C::UpdatePage (byte ch, EFFECT_C effect, short value)
    {
    switch ( (CHANNEL_C)ch )
        {
        case CHANNEL_C::HARDWARE_LFO:
            switch ( effect )
                {
                case EFFECT_C::MAX_LEVEL:
                    break;
                case EFFECT_C::SELECTED:
                    switch ( ch )
                        {
                        case 0:
                            SelectSine = value;
                            break;
                        case 1:
                            SelectRamp = value;
                            break;
                        case 2:
                            SelectPulse = value;
                            break;
                        default:
                            break;
                        }
                    if ( SelectSine )
                        this->MeterHardware->SetSine (Level);
                    else
                        this->MeterHardware->SetSine (0);
                    if ( SelectRamp )
                        this->MeterHardware->SetRamp (Level);
                    else
                        this->MeterHardware->SetRamp (0);
                    if ( SelectPulse )
                        this->MeterHardware->SetPulse (Level);
                    else
                        this->MeterHardware->SetPulse (0);
                    break;
                case EFFECT_C::LFO_FREQ:
                    this->MeterHardware->SetFreq (value);
                    break;
                case EFFECT_C::SAWTOOTH_DIRECTION:
                    this->RampDir->SetDir (value);
                    break;
                case EFFECT_C::PULSE_WIDTH:
                    this->PulseWidth->SetWidth (value);
                    break;
                default:
                    break;
                }

            if ( SelectSine || SelectRamp || SelectPulse )
                this->MeterHardware->Select (true);
            else
                this->MeterHardware->Select (false);
            break;
        case CHANNEL_C::SOFTWARE_LFO:
            switch ( effect )
                {
                case EFFECT_C::SELECTED:
                    break;
                case EFFECT_C::LFO_FREQ:
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
//#######################################################################
    PAGE_FILTER_C::PAGE_FILTER_C (lv_obj_t* base) : PAGE_TITLE_C (base, "FILTERS")
    {
    }

//#######################################################################
//#######################################################################
    PAGE_TUNE_C::PAGE_TUNE_C (lv_obj_t* base)
    {
    int x = 155;
    int y = 140;

    Tuningfont = &lv_font_montserrat_48;
    lv_style_init (&TuningStyle);
    lv_style_set_text_font  (&TuningStyle, Tuningfont);
    lv_style_set_text_color (&TuningStyle, lv_color_hex(0xF00000));

    TuningTitle = lv_label_create (base);
    lv_obj_set_pos    (TuningTitle, x + 45, 20);
    lv_label_set_text (TuningTitle, "TUNING MODE");
    lv_obj_add_style  (TuningTitle, &TuningStyle, 0);

    Note = new NOTE_WIDGET_C (base, x + 125, 80);
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        LevelTuning[z] = new LEVEL_WIDGET_C (base, ChannelText[z], x, y, LV_PALETTE_INDIGO);
        x += 90;
        }
    }

//#######################################################################
void PAGE_TUNE_C::UpdatePage (byte ch, EFFECT_C effect, short value)
    {
    switch ( effect )
        {
        case EFFECT_C::MAX_LEVEL:
            this->LevelTuning[ch]->SetLevel (value);
            break;
        case EFFECT_C::NOTE:
            Note->SetValue (value);
            break;
        default:
            break;
        }
    }

//#######################################################################
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
    lvgl_port_init (Panel->getLcd (), Panel->getTouch ());

    lvgl_port_lock (-1);    // Lock the mutex due to the LVGL APIs are not thread-safe
    Pages = lv_tabview_create ( lv_scr_act (), LV_DIR_LEFT, 0);

    BasePageOsc0   = lv_tabview_add_tab (Pages, "");
    PageOsc0       = new PAGE_OSC_C     (BasePageOsc0, "Oscillators      zone 0 -- 8x");
    BasePageOsc1   = lv_tabview_add_tab (Pages, "");
    PageOsc1       = new PAGE_OSC_C     (BasePageOsc1, "Oscillators      zone 1 -- 4x");
    BasePageOsc2   = lv_tabview_add_tab (Pages, "");
    PageOsc2       = new PAGE_OSC_C     (BasePageOsc2, "Oscillators      zone 2 -- 4x");
    BasePageMod    = lv_tabview_add_tab (Pages, "");
    PageMod        = new PAGE_MOD_C     (BasePageMod);
    BasePageFilter = lv_tabview_add_tab (Pages, "");
    PageFilter     = new PAGE_FILTER_C  (BasePageFilter);
    BasePageTuning = lv_tabview_add_tab (Pages, "");
    PageTune       = new PAGE_TUNE_C    (BasePageTuning);

    PageSelect (PAGE_C::PAGE_OSC0);

    lvgl_port_unlock ();    // Release the mutex
    }

//#######################################################################

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
            page = PAGE_C::PAGE_OSC0;
        }
    if ( CurrentPage != page )
        {
        CurrentPage = page;
        DBG ("Page Select %s (%d)", PageText[(byte)page], (byte)page);
        lv_tabview_set_act(Pages, (byte)page, LV_ANIM_OFF);
        }
    }

GRPH_C  Graphics;

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

        String key = "TF";
        key += String (z);
        KeyLabel (panel, key.c_str (), 0, 0);
        TitleControl[z] = new TITLE_WIDGET_C (panel, ChannelText[z]);
        MeterADSR[z]    = new ADSR_METER_WIDGET_C (panel, 0, 18);
        SustainLevel[z] = new LEVEL_WIDGET_C (panel, "SUSTAIN", 0, 210, LV_PALETTE_ORANGE);
        MaxLevel[z]     = new LEVEL_WIDGET_C (panel, "MAX", 73, 210, LV_PALETTE_INDIGO);

        switch ( z )
            {
            case 2:
                RampDir = new RAMP_WIDGET_C (panel, "TF6", LV_ALIGN_BOTTOM_MID, 0, -6);
                break;
            case 3:
                PulseWidth = new PULSE_WIDGET_C (panel, "PD8", LV_ALIGN_BOTTOM_MID, 0, -6);
                break;
            case 4:
                Noise = new NOISE_WIDGET_C (panel, LV_ALIGN_BOTTOM_MID, 0, -25);
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
            {
            float zf = value * 32.244;
            this->PulseWidth->SetWidth ((short)zf);
            }
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

    TitleSoft = new TITLE_WIDGET_C (panel, "Amplitide");
    MeterSoft = new LFO_METER_WIDGET_C (panel, 0, 18, true);

    y = 190;
    this->SoftLabelSine.BeginText (panel, "   F1", "", y);
    this->UpdateSoftButtons (0, false);
    y += 14;
    this->SoftLabelTriangle.BeginText (panel, "   F2", "", y);
    this->UpdateSoftButtons (1, false);
    y += 14;
    this->SoftLabelRamp.BeginText (panel, "   F3", "", y);
    this->UpdateSoftButtons (2, false);
    y += 14;
    this->SoftLabelPulse.BeginText (panel, "   F4", "", y);
    this->UpdateSoftButtons (3, false);
    y += 14;
    this->SoftLabelNoise.BeginText (panel, "   F5", "", y);
    this->UpdateSoftButtons (4, false);
    this->SoftInUse[0] = this->SoftInUse[1] = this->SoftInUse[2] = this->SoftInUse[3] = this->SoftInUse[4] = false;

    x += 158;
    y = 40;
    panel = lv_obj_create (base);
    lv_obj_set_size             (panel, 159, 320);
    lv_obj_set_pos              (panel, x, y);
    lv_obj_set_style_pad_top    (panel, 0, 0);
    lv_obj_set_style_pad_bottom (panel, 0, 0);
    lv_obj_set_style_pad_left   (panel, 2, 0);
    lv_obj_set_style_pad_right  (panel, 2, 0);

    this->TitleHard  = new TITLE_WIDGET_C (panel, "Frequency");
    this->MeterHard  = new LFO_METER_WIDGET_C (panel, 0, 18, false);
    this->RampDir    = new RAMP_WIDGET_C (panel, "F12", LV_ALIGN_BOTTOM_MID, 0, -44);
    this->PulseWidth = new PULSE_WIDGET_C (panel, "E3", LV_ALIGN_BOTTOM_MID, 0, -10);

    y = 190;
    HardLabelSine.BeginText (panel, "   F9", "", y);
    this->UpdateHardButtons (0, false);
    y += 14;
    HardLabelRamp.BeginText (panel, "   F10", "", y);
    this->UpdateHardButtons (1, false);
    y += 14;
    HardLabelPulse.BeginText (panel, "   F11", "", y);
    this->UpdateHardButtons (2, false);
    this->HardInUse[0] = this->HardInUse[1] = this->HardInUse[2] = false;
    }

//#######################################################################
void PAGE_MOD_C::UpdateHardButtons (short value, bool sel)
    {
    uint32_t color = ( sel ) ? 0x0000F0 : 0xD0D0D0;
    HardInUse[value] = sel;
    switch ( value )
        {
        case 0:
            this->HardLabelSine.SetValueColor (color);
            this->HardLabelSine.SetLabel ("Sine");
            break;
        case 1:
            this->HardLabelRamp.SetValueColor (color);
            this->HardLabelRamp.SetLabel ("Ramp");
            break;
        case 2:
            this->HardLabelPulse.SetValueColor (color);
            this->HardLabelPulse.SetLabel ("Pulse");
            break;
        default:
            break;
        }
    }

//#######################################################################
void PAGE_MOD_C::UpdateSoftButtons (short value, bool sel)
    {
    uint32_t color = ( sel ) ? 0x0000F0 : 0xD0D0D0;
    SoftInUse[value] = sel;
    switch ( value )
        {
        case 0:
            this->SoftLabelSine.SetValueColor (color);
            this->SoftLabelSine.SetLabel ("Sine");
            break;
        case 1:
            this->SoftLabelTriangle.SetValueColor (color);
            this->SoftLabelTriangle.SetLabel ("Triangle");
            break;
        case 2:
            this->SoftLabelRamp.SetValueColor (color);
            this->SoftLabelRamp.SetLabel ("Ramp");
            break;
        case 3:
            this->SoftLabelPulse.SetValueColor (color);
            this->SoftLabelPulse.SetLabel ("Pulse");
            break;
        case 4:
            this->SoftLabelNoise.SetValueColor (color);
            this->SoftLabelNoise.SetLabel ("Noise");
            break;
        default:
            break;
        }
    }

//#######################################################################
void PAGE_MOD_C::UpdatePage (byte ch, EFFECT_C effect, short value)
    {
    switch ( (CHANNEL_C)ch )
        {
        case CHANNEL_C::HARDWARE_LFO:
            switch ( effect )
                {
                case EFFECT_C::SELECTED:
                    this->UpdateHardButtons (value, true);
                    this->MeterHard->Select (true);
                    break;
                case EFFECT_C::DESELECTED:
                    this->UpdateHardButtons (value, false);
                    if ( !HardInUse[0] & !HardInUse[1] & !HardInUse[2] )
                        this->MeterHard->Select (false);
                    break;
                case EFFECT_C::LFO_FREQ:
                    this->MeterHard->SetFreq (value);
                    break;
                case EFFECT_C::SAWTOOTH_DIRECTION:
                    this->RampDir->SetDir (!value);
                    break;
                case EFFECT_C::PULSE_WIDTH:
                    this->PulseWidth->SetWidth (value);
                    break;
                default:
                    break;
                }
            break;
        case CHANNEL_C::SOFTWARE_LFO:
            switch ( effect )
                {
                case EFFECT_C::SELECTED:
                    this->UpdateSoftButtons (value, true);
                    this->MeterSoft->Select (true);
                    break;
                case EFFECT_C::DESELECTED:
                    this->UpdateSoftButtons (value, false);
                    if ( !SoftInUse[0] & !SoftInUse[1] & !SoftInUse[2] & !SoftInUse[3] & !SoftInUse[4] )
                        this->MeterSoft->Select (false);
                    break;
                case EFFECT_C::LFO_FREQ:
                    if ( value == 0 )
                        value = 1;
                    this->MeterSoft->SetFreq (value);
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

    this->Tuningfont = &lv_font_montserrat_48;
    lv_style_init (&this->TuningStyle);
    lv_style_set_text_font  (&this->TuningStyle, this->Tuningfont);
    lv_style_set_text_color (&this->TuningStyle, lv_color_hex(0xF00000));

    this->TuningTitle = lv_label_create (base);
    lv_obj_set_pos    (this->TuningTitle, x + 45, 20);
    lv_label_set_text (this->TuningTitle, "TUNING MODE");
    lv_obj_add_style  (this->TuningTitle, &this->TuningStyle, 0);

    this->Note = new NOTE_WIDGET_C (base, x + 116, 80);
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        this->LevelTuning[z] = new LEVEL_WIDGET_C (base, ChannelText[z], x, y, LV_PALETTE_INDIGO);
        x += 90;
        }
    this->TuneSelection = new TUNES_WIDGET_C (base, 248, 340);
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
        case EFFECT_C::SELECTED:
            this->TuneSelection->Set (ch, value);
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

    // initialize keyboard sugguestions
    lv_style_init (&GlobalKeyStyle);
    lv_style_set_text_font (&GlobalKeyStyle, &lv_font_montserrat_12);
    lv_style_set_text_color (&GlobalKeyStyle, lv_color_hex (0xD0D0D0));

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

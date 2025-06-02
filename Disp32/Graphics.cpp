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
//#######################################################################
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
    PAGE_OSC_C::PAGE_OSC_C (lv_obj_t* base) : PAGE_TITLE_C (base, "Voice Midi 1")
    {
    int x = 0;
    int y = 40;

    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        lv_obj_t* panel = lv_obj_create (base);
        lv_obj_set_size (panel, 159, 440);
        lv_obj_set_pos (panel, x, y);
        lv_obj_set_style_pad_top (panel, 0, 0);
        lv_obj_set_style_pad_bottom (panel, 0, 0);
        lv_obj_set_style_pad_left (panel, 2, 0);
        lv_obj_set_style_pad_right (panel, 2, 0);

        String key = "TF";
        key += String (z);
        KeyLabel (panel, key.c_str (), 0, 0);
        this->TitleControl[z] = new TITLE_WIDGET_C (panel, VoiceText[z]);
        this->MeterADSR[z]    = new ADSR_METER_WIDGET_C (panel, 0, 18);
        this->SustainLevel[z] = new LEVEL_WIDGET_C (panel, "SUSTAIN", 0, 210, LV_PALETTE_ORANGE);
        this->MaxLevel[z]     = new LEVEL_WIDGET_C (panel, "MAX", 73, 210, LV_PALETTE_INDIGO);
        this->Midi            = 0;

        switch ( z )
            {
            case 2:
                this->RampDir = new RAMP_WIDGET_C (panel, "TF6", LV_ALIGN_BOTTOM_MID, 0, -6);
                break;
            case 3:
                this->PulseWidth = new PULSE_WIDGET_C (panel, "PD8", LV_ALIGN_BOTTOM_MID, 0, -6);
                break;
            default:
                break;
            }

        x += 158;
        }
    }

//#######################################################################
void PAGE_OSC_C::SetPage (byte midi)
    {
    String s = "Voice  Midi  " + String (midi);
    this->Midi = midi;
    lv_label_set_text (Title, s.c_str ());
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

    TitleSoft = new TITLE_WIDGET_C (panel, "Amplitide 1");
    MeterSoft = new LFO_METER_WIDGET_C (panel, 0, 18, true, "  E1");

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

    this->CreateLFO (0, x, y, lv_obj_create (base), "Frequency 1", "  E2", "F12", "E3", "   F9", "   F10", "   F11");
    this->UpdateHardButtons (0, 0, false);
    this->UpdateHardButtons (0, 1, false);
    this->UpdateHardButtons (0, 2, false);

    x += 158;
    y = 40;

    this->CreateLFO (1, x, y, lv_obj_create (base), "Frequency 2", "  E4", "F12", "E5", "  F13", "   F14", "   F15");
    this->UpdateHardButtons (1, 0, false);
    this->UpdateHardButtons (1, 1, false);
    this->UpdateHardButtons (1, 2, false);
    }

//#######################################################################
void PAGE_MOD_C::CreateLFO (int num, int x, int y, lv_obj_t* panel, const char* title, const char* mstr, const char* rs, const char* pws, const char* ssine, const char* sramp, const char* spulse)
    {
    LFO_C& lfo = this->LowFreq[num];

    lv_obj_set_size             (panel, 159, 320);
    lv_obj_set_pos              (panel, x, y);
    lv_obj_set_style_pad_top    (panel, 0, 0);
    lv_obj_set_style_pad_bottom (panel, 0, 0);
    lv_obj_set_style_pad_left   (panel, 2, 0);
    lv_obj_set_style_pad_right  (panel, 2, 0);

    lfo.TitleHard  = new TITLE_WIDGET_C (panel, title);
    lfo.MeterHard  = new LFO_METER_WIDGET_C (panel, 0, 18, false, mstr);
    lfo.RampDir    = new RAMP_WIDGET_C (panel, rs, LV_ALIGN_BOTTOM_MID, 0, -44);
    lfo.PulseWidth = new PULSE_WIDGET_C (panel, pws, LV_ALIGN_BOTTOM_MID, 0, -10);

    y = 190;
    lfo.HardLabelSine.BeginText (panel, ssine, "", y);
    y += 14;
    lfo.HardLabelRamp.BeginText (panel, sramp, "", y);
    y += 14;
    lfo.HardLabelPulse.BeginText (panel, spulse, "", y);
    lfo.HardInUse[0] = lfo.HardInUse[1] = lfo.HardInUse[2] = false;
    }

//#######################################################################
void PAGE_MOD_C::UpdateHardButtons (short index, short value, bool sel)
    {
    LFO_C& lfo = this->LowFreq[index];
    uint32_t color = ( sel ) ? 0x0000F0 : 0xD0D0D0;
    lfo.HardInUse[value] = sel;
    switch ( value )
        {
        case 0:
            lfo.HardLabelSine.SetValueColor (color);
            lfo.HardLabelSine.SetLabel ("Sine");
            break;
        case 1:
            lfo.HardLabelRamp.SetValueColor (color);
            lfo.HardLabelRamp.SetLabel ("Ramp");
            break;
        case 2:
            lfo.HardLabelPulse.SetValueColor (color);
            lfo.HardLabelPulse.SetLabel ("Pulse");
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
void PAGE_MOD_C::UpdatePage (byte index, byte ch, EFFECT_C effect, short value)
    {
    switch ( (VOICE_C)ch )
        {
        case VOICE_C::HARDWARE_LFO:
            switch ( effect )
                {
                case EFFECT_C::SELECTED:
                    this->UpdateHardButtons (index, value, true);
                    this->LowFreq[index].MeterHard->Select (true);
                    break;
                case EFFECT_C::DESELECTED:
                    this->UpdateHardButtons (index, value, false);
                    if ( !this->LowFreq[index].HardInUse[0] & !this->LowFreq[index].HardInUse[1] & !this->LowFreq[index].HardInUse[2] )
                        this->LowFreq[index].MeterHard->Select (false);
                    break;
                case EFFECT_C::LFO_FREQ:
                    this->LowFreq[index].MeterHard->SetFreq (value);
                    break;
                case EFFECT_C::SAWTOOTH_DIRECTION:
                    this->LowFreq[index].RampDir->SetDir (!value);
                    break;
                case EFFECT_C::PULSE_WIDTH:
                    this->LowFreq[index].PulseWidth->SetWidth (value);
                    break;
                default:
                    break;
                }
            break;
        case VOICE_C::SOFTWARE_LFO:
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
    PAGE_MAPPING_C::PAGE_MAPPING_C (lv_obj_t* base)
    {
    lv_style_init          (&TitleStyle);
    lv_style_set_text_font (&TitleStyle, &lv_font_montserrat_20);

    MidiTitle = lv_label_create (base);
    lv_obj_align           (MidiTitle, LV_ALIGN_TOP_LEFT, 45, -10);
    lv_label_set_text      (MidiTitle, "Voice");
    lv_obj_add_style       (MidiTitle, &TitleStyle, 0);

    NoiseTitle = lv_label_create (base);
    lv_obj_align           (NoiseTitle, LV_ALIGN_TOP_LEFT, 200, -10);
    lv_label_set_text      (NoiseTitle, "Noise");
    lv_obj_add_style       (NoiseTitle, &TitleStyle, 0);

    ModTitle = lv_label_create (base);
    lv_obj_align           (ModTitle, LV_ALIGN_TOP_LEFT, 335, -10);
    lv_label_set_text      (ModTitle, "First Stage");
    lv_obj_add_style       (ModTitle, &TitleStyle, 0);

    ModTitle = lv_label_create (base);
    lv_obj_align           (ModTitle, LV_ALIGN_TOP_LEFT, 510, -10);
    lv_label_set_text      (ModTitle, "LFO");
    lv_obj_add_style       (ModTitle, &TitleStyle, 0);

    for (int z = 0, c = 1;  z < 4;  z++, c += 2 )
        {
        String s = "Voice\n" + String (c) + " & " + String (c + 1);
        this->SelVoice[z]  = new SELECT_WIDGET_C (base, s.c_str (),   0, 22 + (z * 108), 136, "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16", 24);
        this->SelNoise[z]  = new SELECT_WIDGET_C (base, s.c_str (), 137, 22 + (z * 108), 164, "White\nPink\nRed\nBlue", 38);
        this->SelOutput[z] = new SELECT_WIDGET_C (base, s.c_str (), 300, 22 + (z * 108), 155, "Off\nOsc\nFilter", 34);
        if ( z < 2 )
            s = "Freq\n " + String (z + 1);
        if ( z > 1 )
            s = "Amp\n " + String (z - 1);
        if ( z < 3 )
            this->Sellfo[z] = new SELECT_WIDGET_C(base, s.c_str(), 455, 22 + (z * 108), 172, "Off\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16", 28);
        }
    this->UpdatePage (0, EFFECT_C::MAP_VOICE, 0);
    }

//#######################################################################
void PAGE_MAPPING_C::UpdatePage (byte ch, EFFECT_C effect, short value)
    {
    switch ( effect )
        {
        case EFFECT_C::MAP_VOICE:
            this->Selected = ch;
            for ( int z = 0;  z < MAP_COUNT;  z++ )     // process first column
                this->SelVoice[z]->Select (z == ch);
            if ( this->Selected < MAP_COUNT )
                {
                this->SelVoice[ch]->Set (value - 1);
                ch = 255;
                }
            else
                ch -= MAP_COUNT;

            for ( int z = 0;  z < MAP_COUNT;  z++ )     // Process second column
                this->SelNoise[z]->Select (z == ch);
            if ( ch  < MAP_COUNT )
                {
                this->SelNoise[ch]->Set (value);
                ch = 255;
                }
            else
                ch -= MAP_COUNT;

            for ( int z = 0;  z < MAP_COUNT;  z++ )     // Process third column
                this->SelOutput[z]->Select (z == ch);
            if ( ch  < MAP_COUNT )
                {
                this->SelOutput[ch]->Set (value);
                ch = 255;
                }
            else
                ch -= MAP_COUNT;

            for ( int z = 0;  z < MAP_COUNT - 1;  z++ )     // Process fourth column
                this->Sellfo[z]->Select (z == ch);
            if ( ch  < (MAP_COUNT - 1) )
                this->Sellfo[ch]->Set (value);
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

    this->TuningFont = &lv_font_montserrat_48;
    lv_style_init (&this->TuningStyle);
    lv_style_set_text_font  (&this->TuningStyle, this->TuningFont);
    lv_style_set_text_color (&this->TuningStyle, lv_color_hex(0xF00000));

    this->TuningTitle = lv_label_create (base);
    lv_obj_set_pos    (this->TuningTitle, x + 45, 20);
    lv_label_set_text (this->TuningTitle, "TUNING MODE");
    lv_obj_add_style  (this->TuningTitle, &this->TuningStyle, 0);

    this->Note = new NOTE_WIDGET_C (base, x + 116, 80);
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        this->LevelTuning[z] = new LEVEL_WIDGET_C (base, VoiceText[z], x, y, LV_PALETTE_INDIGO);
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
            this->Note->SetValue (value);
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
    PAGE_CALIBRATE_C::PAGE_CALIBRATE_C (lv_obj_t* base)
    {
    int         x = 155;
    lv_obj_t*   ctitle;

    this->CalibFont = &lv_font_montserrat_48;
    lv_style_init (&this->CalibStyle);
    lv_style_set_text_font  (&this->CalibStyle, this->CalibFont);
    lv_style_set_text_color (&this->CalibStyle, lv_color_hex(0xF00000));

    ctitle = lv_label_create (base);
    lv_obj_set_pos    (ctitle, x + 45, 20);
    lv_label_set_text (ctitle, "Calibrating");
    lv_obj_add_style  (ctitle, &this->CalibStyle, 0);

    ctitle = lv_label_create (base);
    lv_obj_set_pos    (ctitle, x + 45, 80);
    lv_label_set_text (ctitle, "Please wait");
    lv_obj_add_style  (ctitle, &this->CalibStyle, 0);
    }

//#######################################################################
//#######################################################################
    PAGE_LOAD_SAVE_C::PAGE_LOAD_SAVE_C (lv_obj_t* base) : PAGE_TITLE_C (base, "LOAD / SAVE")
    {
    lv_obj_t*   panel;
    int         x = 210;
    int         y = 150;

    panel = lv_obj_create (base);
    lv_obj_set_size             (panel, 328, 188);
    lv_obj_set_pos              (panel, x, y);
    lv_obj_set_style_pad_top    (panel, 0, 0);
    lv_obj_set_style_pad_bottom (panel, 0, 0);
    lv_obj_set_style_pad_left   (panel, 2, 0);
    lv_obj_set_style_pad_right  (panel, 2, 0);

    this->Selection  = new SELECT_WIDGET_C (panel, "Selection",   62, 20, 180, "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20", 32);
//    this->Selection->Select (true);

    this->MessageFont = &lv_font_montserrat_38;
    lv_style_init (&this->MessageStyle);
    lv_style_set_text_font (&this->MessageStyle, this->MessageFont);
    this->Message = lv_label_create (panel);
    lv_obj_add_style  (this->Message, &this->MessageStyle, 0);
    }

//#######################################################################
void PAGE_LOAD_SAVE_C::UpdatePage (EFFECT_C effect, short value)
    {
    String str;
    switch ( effect )
        {
        case EFFECT_C::MESSAGE:
            switch ( value )
                {
                case 1:
                    str = "LOADING #" + String (this->SelectedValue);
                    break;
                case 2:
                    str = "SAVING #" + String (this->SelectedValue);
                    break;
                default:
                    str.clear ();
                    break;
                }
            this->SetMessage (str.c_str ());
            break;
        case EFFECT_C::VALUE:
            this->SetMessage ("");

            this->SelectedValue = value;
            this->Selection->Set (value - 1);
            break;
        default:
            break;
        }
    }

//#######################################################################
void PAGE_LOAD_SAVE_C::SetMessage (const char* str)
    {
    lv_label_set_text (this->Message, str);
    lv_obj_align      (this->Message, LV_ALIGN_BOTTOM_MID, 0, -11);
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

    for ( short z = 0;  z < MAP_COUNT;  z++ )
        {
        BasePageVoice[z] = lv_tabview_add_tab (Pages, "");
        PageVoice[z]     = new PAGE_OSC_C     (BasePageVoice[z]);
        }
    PageVoice[0]->SetPage (1);                              // initialize only the first voice page for midi allocation as default
    BasePageMod         = lv_tabview_add_tab   (Pages, "");
    PageMod             = new PAGE_MOD_C       (BasePageMod);
    BasePageFilter      = lv_tabview_add_tab   (Pages, "");
    PageFilter          = new PAGE_FILTER_C    (BasePageFilter);
    BasePageMap         = lv_tabview_add_tab   (Pages, "");
    PageMap             = new PAGE_MAPPING_C   (BasePageMap);
    BasePageCalibration = lv_tabview_add_tab   (Pages, "");
    PageCalibrate       = new PAGE_CALIBRATE_C (BasePageCalibration);
    BasePageTuning      = lv_tabview_add_tab   (Pages, "");
    PageTune            = new PAGE_TUNE_C      (BasePageTuning);
    BasePageLoadSave    = lv_tabview_add_tab   (Pages, "");
    PageLoadSave        = new PAGE_LOAD_SAVE_C (BasePageLoadSave);

    PageSelect (PAGE_C::PAGE_OSC0);

    lvgl_port_unlock ();    // Release the mutex
    }

//#######################################################################
void GRPH_C::ClearData (short num)
    {
    switch ( num )
        {
        case 0:
            PageLoadSave->SetMessage ("");
            break;
        default:
            break;
        }
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
void GRPH_C::SetPage (byte num, byte midi)
    {
    this->PageVoice[num]->SetPage (midi);
    }

//#######################################################################
void GRPH_C::PageSelect (PAGE_C page)
    {
    while ( page == PAGE_C::PAGE_ADVANCE )
        {
        page = (PAGE_C)((byte)this->CurrentPage + 1);
        if ( page == PAGE_C::PAGE_CALIBRATION )
            page = PAGE_C::PAGE_OSC0;
        if ( (page <= PAGE_C::PAGE_OSC3) )
            break;
        }
    if ( CurrentPage != page )
        {
        CurrentPage = page;
        DBG ("Page Select %s (%d)", PageText[(byte)page], (byte)page);
        lv_tabview_set_act(Pages, (byte)page, LV_ANIM_OFF);
        }
    }

//#######################################################################
GRPH_C  Graphics;


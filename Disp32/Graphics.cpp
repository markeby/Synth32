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
    lv_obj_align      (Title, LV_ALIGN_TOP_MID, 0, -2);
    lv_label_set_text (Title, str);
    lv_style_init (&TitleStyle);       // for page titles
    lv_style_set_text_font (&TitleStyle, &lv_font_montserrat_36);
    lv_obj_add_style  (Title, &TitleStyle, 0);
    }

//#######################################################################
//#######################################################################
static const char* filterTitle[]  = { "Frequency", "Q" };
static const char* filterOutput[] = { "Bypass", "   LP   ", "   LBP  ", "   UBP  ", "   HP   " };
static const char* filterCtrl[]   = { "Fx", "Env", "Mod", "M W" };
static const char* filterKeys[]   = { "1", "4", "2", "5", "3", "6" };
    PAGE_FILTER_C::PAGE_FILTER_C (lv_obj_t* base) : PAGE_TITLE_C (base, "Filter Midi 1")
    {
    lv_obj_t*       panel;
    lv_obj_t*       label;
    TITLE_WIDGET_C* title;
    int             x = 0;
    int             y = 34;

    Midi = 0;
    for ( short z = 0;  z < 2;  z++ )
        {
        panel = lv_obj_create (base);
        lv_obj_set_size (panel, 225, 392);
        lv_obj_set_pos (panel, x, y);
        lv_obj_set_style_pad_top (panel, 0, 0);
        lv_obj_set_style_pad_bottom (panel, 0, 0);
        lv_obj_set_style_pad_left (panel, 0, 0);
        lv_obj_set_style_pad_right (panel, 0, 0);

        title           = new TITLE_WIDGET_C (panel, filterTitle[z]);
        MeterADSR[z]    = new ADSR_METER_WIDGET_C (panel, 0, 23);
        ValueStart[z]   = new LEVEL_WIDGET_C (panel, "START",   filterKeys[z],   0, 210, 63, LV_PALETTE_INDIGO);
        ValueEnd[z]     = new LEVEL_WIDGET_C (panel, "END",     filterKeys[z + 2],  73, 210, 63, LV_PALETTE_TEAL);
        ValueSustain[z] = new LEVEL_WIDGET_C (panel, "SUSTAIN", filterKeys[z + 4], 146, 210, 63, LV_PALETTE_ORANGE);

        x += 360;
        }
    x = 223;

    for ( short zz = 0;  zz < 2;  zz++ )
        {
        panel = lv_obj_create (base);
        lv_obj_set_size (panel, 138, 196);
        lv_obj_set_pos (panel,  x, y);
        lv_obj_set_style_pad_top (panel, 0, 0);
        lv_obj_set_style_pad_bottom (panel, 0, 0);
        lv_obj_set_style_pad_left (panel, 2, 0);
        lv_obj_set_style_pad_right (panel, 2, 0);
        title = new TITLE_WIDGET_C (panel, filterTitle[zz]);
        for ( int z = 0;  z < 4;  z++ )
            {
            lv_obj_t*  plv;
            plv = lv_btn_create (panel);
            Ctrl[zz][z] = plv;
            lv_obj_align      (plv, LV_ALIGN_TOP_MID, 0, (z * 39) + 27);
            lv_obj_add_flag   (plv, LV_OBJ_FLAG_CHECKABLE);
            lv_obj_add_state  (plv, LV_STATE_DISABLED);
            lv_obj_set_height (plv, 30);

            label = lv_label_create (plv);
            lv_label_set_text (label, filterCtrl[z]);
            lv_obj_center     (label);
            }
        y += 194;
        }

    panel = lv_obj_create (base);
    lv_obj_set_size (panel, 586, 58);
    lv_obj_set_pos (panel, 0, 422);
    lv_obj_set_style_pad_top (panel, 0, 0);
    lv_obj_set_style_pad_bottom (panel, 0, 0);
    x = 6;
    for ( int z = 0;  z < 5;  z++ )
        {
        Output[z] = lv_btn_create (panel);
        lv_obj_align      (Output[z], LV_ALIGN_LEFT_MID, x, 0);
        lv_obj_add_flag   (Output[z], LV_OBJ_FLAG_CHECKABLE);
        lv_obj_add_state  (Output[z], LV_STATE_DISABLED);
        lv_obj_set_height (Output[z], 30);

        label = lv_label_create (Output[z]);
        lv_label_set_text (label, filterOutput[z]);
        lv_obj_center     (label);

        x += 109;
        }

    }

//#######################################################################
void PAGE_FILTER_C::Control (byte fn, byte select)
    {
    for ( short z = 0;  z < 4;  z++ )
        {
        if ( select == z )
            {
            lv_obj_clear_state (Ctrl[fn][z], LV_STATE_DISABLED);
            lv_obj_add_state (Ctrl[fn][z], LV_STATE_CHECKED);
            }
        else
            {
            lv_obj_clear_state (Ctrl[fn][z], LV_STATE_CHECKED);
            lv_obj_add_state (Ctrl[fn][z], LV_STATE_DISABLED);
            }
        }
    }

//#######################################################################
void PAGE_FILTER_C::Select (byte fmap)
    {
    for ( short z = 0;  z < 5;  z++ )
        {
        if ( (fmap >> z) & 1 )
            {
            lv_obj_clear_state (Output[z], LV_STATE_DISABLED);
            lv_obj_add_state (Output[z], LV_STATE_CHECKED);
            }
        else
            {
            lv_obj_clear_state (Output[z], LV_STATE_CHECKED);
            lv_obj_add_state (Output[z], LV_STATE_DISABLED);
            }
        }
    }

//#######################################################################
void PAGE_FILTER_C::SetMidi (byte midi)
    {
    String s = "Filter  Midi  " + String (midi);
    Midi = midi;
    lv_label_set_text (Title, s.c_str ());
    }

//#######################################################################
void PAGE_FILTER_C::UpdatePage (byte fn, EFFECT_C effect, short value)
    {
    switch ( effect )
        {
        case EFFECT_C::SELECTED:
            MeterADSR[fn]->Select (value);
            break;
        case EFFECT_C::BASE_LEVEL:
            ValueStart[fn]->SetLevel (value);
            break;
        case EFFECT_C::MAX_LEVEL:
            ValueEnd[fn]->SetLevel (value);
            break;
        case EFFECT_C::ATTACK_TIME:
            MeterADSR[fn]->SetAttack (value);
            break;
        case EFFECT_C::DECAY_TIME:
            MeterADSR[fn]->SetDecay (value);
            break;
        case EFFECT_C::RELEASE_TIME:
            MeterADSR[fn]->SetRelease (value);
            break;
        case EFFECT_C::SUSTAIN_LEVEL:
            ValueSustain[fn]->SetLevel (value);
            break;
        case EFFECT_C::MAP_VOICE:
            Select (value);
            break;
        case EFFECT_C::CONTROL:
            Control (fn, value);
            break;
        default:
            break;
        }
    }

//#######################################################################
//#######################################################################
static const char* OscKeys[]   = { "1", "2", "3", "4", "5", "8" };
    PAGE_OSC_C::PAGE_OSC_C (lv_obj_t* base) : PAGE_TITLE_C (base, "Voice Midi 1")
    {
    int x = 0;
    int y = 40;

    Midi = 0;
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        lv_obj_t* panel = lv_obj_create (base);
        lv_obj_set_size             (panel, 145, 440);
        lv_obj_set_pos              (panel, x, y);
        lv_obj_set_style_pad_top    (panel, 0, 0);
        lv_obj_set_style_pad_bottom (panel, 0, 0);
        lv_obj_set_style_pad_left   (panel, 2, 0);
        lv_obj_set_style_pad_right  (panel, 2, 0);

        String key = "TF";
        key += String (z);
        KeyLabel (panel, key.c_str (), 0, 0);
        TitleControl[z] = new TITLE_WIDGET_C      (panel, VoiceOptText[z]);
        MeterADSR[z]    = new ADSR_METER_WIDGET_C (panel, 0, 18);
        MaxLevel[z]     = new LEVEL_WIDGET_C      (panel, "MAX",     OscKeys[z],  0, 210, 63, LV_PALETTE_INDIGO);
        SustainLevel[z] = new LEVEL_WIDGET_C      (panel, "SUSTAIN", OscKeys[z], 70, 210, 63, LV_PALETTE_ORANGE);
        SustainLevel[z]->Active (false);

        switch ( z )
            {
            case 2:
                RampDir = new RAMP_WIDGET_C (panel, "TF6", LV_ALIGN_BOTTOM_MID, 0, -10);
                break;
            case 3:
                PulseWidth = new PULSE_WIDGET_C (panel, "PD8", LV_ALIGN_BOTTOM_MID, 0, -10);
                break;
            default:
                break;
            }

        x += 145;
        }

    MasterLevel = new LEVEL_WIDGET_C (base, "MASTER", OscKeys[5],  727, 252, 63, LV_PALETTE_INDIGO);
    }

//#######################################################################
void PAGE_OSC_C::SetMidi (byte midi)
    {
    String s = "Voice  Midi  " + String (midi);
    Midi = midi;
    lv_label_set_text (Title, s.c_str ());
    }

//#######################################################################
void PAGE_OSC_C::UpdatePage (byte ch, EFFECT_C effect, short value)
    {
    switch ( effect )
        {
        case EFFECT_C::MASTER_LEVEL:
            MasterLevel->SetLevel (value);
            break;
        case EFFECT_C::DAMPER:
            MeterADSR[ch]->Damper (value);
            break;
        case EFFECT_C::MUTE:
//            for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
//                MeterADSR[z]->Mute (value);
            MasterLevel->Mute (value);
            break;
        case EFFECT_C::SELECTED:
            MaxLevel[ch]->Active (!value);
            SustainLevel[ch]->Active (value);
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
        case EFFECT_C::RELEASE_TIME:
            MeterADSR[ch]->SetRelease (value);
            break;
        case EFFECT_C::SUSTAIN_LEVEL:
            SustainLevel[ch]->SetLevel (value);
            break;
        case EFFECT_C::RAMP_DIRECTION:
            RampDir->SetDir (value);
            break;
        case EFFECT_C::PULSE_WIDTH:
            {
            float zf = value * 32.244;
            PulseWidth->SetWidth ((short)zf);
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

    CreateLFO (0, x, y, base, "Vibrato 1", "A1-A2", "TF4", "A3", "   TF1", "   TF2", "   TF3");
    UpdateHardButtons (0, 0, false);
    UpdateHardButtons (0, 1, false);
    UpdateHardButtons (0, 2, false);
    LowFreq[0].PulseWidth->SetWidth (64);
    x += 202;
    y = 40;

    CreateLFO (1, x, y, base, "Vibrato 2", "A5-A6", "TF8", "A7", "   TF5", "   TF6", "   TF7");
    UpdateHardButtons (1, 0, false);
    UpdateHardButtons (1, 1, false);
    UpdateHardButtons (1, 2, false);
    LowFreq[1].PulseWidth->SetWidth (64);

    x += 404;
    y = 40;

    panel = lv_obj_create (base);
    lv_obj_set_size             (panel, 159, 320);
    lv_obj_set_pos              (panel, x, y);
    lv_obj_set_style_pad_top    (panel, 0, 0);
    lv_obj_set_style_pad_bottom (panel, 0, 0);
    lv_obj_set_style_pad_left   (panel, 2, 0);
    lv_obj_set_style_pad_right  (panel, 2, 0);

    TitleSoft = new TITLE_WIDGET_C (panel, "Tremolo 1");
    MeterSoft = new LFO_METER_WIDGET_C (panel, 9, 20, true, "P1-P2");

    y = 190;
    SoftLabelSine.BeginText (panel, "  TC1", "", y);
    UpdateSoftButtons (0, false);
    y += 14;
    SoftLabelTriangle.BeginText (panel, "  TC2", "", y);
    UpdateSoftButtons (1, false);
    y += 14;
    SoftLabelRamp.BeginText (panel, "  TC3", "", y);
    UpdateSoftButtons (2, false);
    y += 14;
    SoftLabelPulse.BeginText (panel, "  TC4", "", y);
    UpdateSoftButtons (3, false);
    y += 14;
    SoftLabelNoise.BeginText (panel, "  TC5", "", y);
    UpdateSoftButtons (4, false);
    SoftInUse[0] = SoftInUse[1] = SoftInUse[2] = SoftInUse[3] = SoftInUse[4] = false;
    }

//#######################################################################
static const char* textModAlt[] = { "Mod Wheel", "G49 S1"};
void PAGE_MOD_C::CreateLFO (int num, int x, int y, lv_obj_t* base, const char* title, const char* mstr, const char* rs, const char* pws, const char* ssine, const char* sramp, const char* spulse)
    {
    LFO_C& lfo = LowFreq[num];
    lv_obj_t* panel = lv_obj_create (base);

    lv_obj_set_size             (panel, 159, 390);
    lv_obj_set_pos              (panel, x, y);
    lv_obj_set_style_pad_top    (panel, 0, 0);
    lv_obj_set_style_pad_bottom (panel, 0, 0);
    lv_obj_set_style_pad_left   (panel, 2, 0);
    lv_obj_set_style_pad_right  (panel, 2, 0);

    lfo.TitleHard  = new TITLE_WIDGET_C (panel, title);
    lfo.MeterHard  = new LFO_METER_WIDGET_C (panel, 9, 20, false, mstr);
    lfo.RampDir    = new RAMP_WIDGET_C (panel, rs, LV_ALIGN_CENTER, 0, 60);
    lfo.PulseWidth = new PULSE_WIDGET_C (panel, pws, LV_ALIGN_CENTER, 0, 91);
    lfo.MeterHard->SetFreq (0);

    y = 190;
    lfo.HardLabelSine.BeginText (panel, ssine, "", y);
    y += 14;
    lfo.HardLabelRamp.BeginText (panel, sramp, "", y);
    y += 14;
    lfo.HardLabelPulse.BeginText (panel, spulse, "", y);
    lfo.HardInUse[0] = lfo.HardInUse[1] = lfo.HardInUse[2] = false;

    x = 0;
    y = -50;
    for ( short z = 0;  z < 2;  z++ )
        {
        lfo.ModLevelAlt[z] = lv_btn_create (panel);
        lv_obj_align      (lfo.ModLevelAlt[z], LV_ALIGN_BOTTOM_MID, x, y);
        lv_obj_add_flag   (lfo.ModLevelAlt[z], LV_OBJ_FLAG_CHECKABLE);
            lv_obj_add_state(lfo.ModLevelAlt[z], LV_STATE_DISABLED);
        lv_obj_set_height (lfo.ModLevelAlt[z], 30);

        String str = textModAlt[z];
        if ( z == 1 )
            str.replace("1",  String (num + 1));
        lv_obj_t* label = lv_label_create (lfo.ModLevelAlt[z]);
        lv_label_set_text (label, str.c_str ());
        lv_obj_center     (label);

        y += 36;
        }
    LevelAlt (num, false);
    }

//#######################################################################
void PAGE_MOD_C::LevelAlt (short num, bool state)
    {
    if ( state )
        {
        lv_obj_clear_state (LowFreq[num].ModLevelAlt[0], LV_STATE_CHECKED);
        lv_obj_add_state   (LowFreq[num].ModLevelAlt[0], LV_STATE_DISABLED);
        lv_obj_clear_state (LowFreq[num].ModLevelAlt[1], LV_STATE_DISABLED);
        lv_obj_add_state   (LowFreq[num].ModLevelAlt[1], LV_STATE_CHECKED);
        }
    else
        {
        lv_obj_clear_state (LowFreq[num].ModLevelAlt[0], LV_STATE_DISABLED);
        lv_obj_add_state   (LowFreq[num].ModLevelAlt[0], LV_STATE_CHECKED);
        lv_obj_clear_state (LowFreq[num].ModLevelAlt[1], LV_STATE_CHECKED);
        lv_obj_add_state   (LowFreq[num].ModLevelAlt[1], LV_STATE_DISABLED);
        }
    }

//#######################################################################
void PAGE_MOD_C::UpdateHardButtons (short index, short value, bool sel)
    {
    LFO_C& lfo = LowFreq[index];
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
            SoftLabelSine.SetValueColor (color);
            SoftLabelSine.SetLabel ("Sine");
            break;
        case 1:
            SoftLabelTriangle.SetValueColor (color);
            SoftLabelTriangle.SetLabel ("Triangle");
            break;
        case 2:
            SoftLabelRamp.SetValueColor (color);
            SoftLabelRamp.SetLabel ("Ramp");
            break;
        case 3:
            SoftLabelPulse.SetValueColor (color);
            SoftLabelPulse.SetLabel ("Pulse");
            break;
        case 4:
            SoftLabelNoise.SetValueColor (color);
            SoftLabelNoise.SetLabel ("Noise");
            break;
        default:
            break;
        }
    }

//#######################################################################
void PAGE_MOD_C::UpdatePage (byte index, byte ch, EFFECT_C effect, short value)
    {
    switch ( (VOICE_OPT_C)ch )
        {
        case VOICE_OPT_C::HARDWARE_LFO:
            switch ( effect )
                {
                case EFFECT_C::SELECTED:
                    UpdateHardButtons (index, value, true);
                    LowFreq[index].MeterHard->Select (true);
                    break;
                case EFFECT_C::DESELECTED:
                    UpdateHardButtons (index, value, false);
                    if ( !LowFreq[index].HardInUse[0] & !LowFreq[index].HardInUse[1] & !LowFreq[index].HardInUse[2] )
                        LowFreq[index].MeterHard->Select (false);
                    break;
                case EFFECT_C::LFO_FREQ:
                    LowFreq[index].MeterHard->SetFreq (value);
                    break;
                case EFFECT_C::RAMP_DIRECTION:
                    LowFreq[index].RampDir->SetDir (!value);
                    break;
                case EFFECT_C::PULSE_WIDTH:
                    LowFreq[index].PulseWidth->SetWidth (value);
                    break;
                case EFFECT_C::ALTERNATE:
                    LevelAlt (index, value);
                    break;
                default:
                    break;
                }
            break;
        case VOICE_OPT_C::SOFTWARE_LFO:
            switch ( effect )
                {
                case EFFECT_C::SELECTED:
                    UpdateSoftButtons (value, true);
                    MeterSoft->Select (true);
                    break;
                case EFFECT_C::DESELECTED:
                    UpdateSoftButtons (value, false);
                    if ( !SoftInUse[0] & !SoftInUse[1] & !SoftInUse[2] & !SoftInUse[3] & !SoftInUse[4] )
                        MeterSoft->Select (false);
                    break;
                case EFFECT_C::LFO_FREQ:
                    if ( value == 0 )
                        value = 1;
                    MeterSoft->SetFreq (value);
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

    lv_obj_t* mtitle = lv_label_create (base);
    lv_obj_align           (mtitle, LV_ALIGN_TOP_LEFT, 45, -10);
    lv_label_set_text      (mtitle, "Voice");
    lv_obj_add_style       (mtitle, &TitleStyle, 0);

    mtitle = lv_label_create (base);
    lv_obj_align           (mtitle, LV_ALIGN_TOP_LEFT, 200, -10);
    lv_label_set_text      (mtitle, "Noise");
    lv_obj_add_style       (mtitle, &TitleStyle, 0);

    mtitle = lv_label_create (base);
    lv_obj_align           (mtitle, LV_ALIGN_TOP_LEFT, 335, -10);
    lv_label_set_text      (mtitle, "LFO");
    lv_obj_add_style       (mtitle, &TitleStyle, 0);

    for (int z = 0, c = 1;  z < 4;  z++, c += 2 )
        {
        String s = "Voice\n" + String (c) + " & " + String (c + 1);
        SelVoice[z]  = new SELECT_WIDGET_C (base, s.c_str (),   0, 22 + (z * 108), 136, "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16", 24);
        SelNoise[z]  = new SELECT_WIDGET_C (base, s.c_str (), 137, 22 + (z * 108), 164, "White\nPink\nRed\nBlue", 38);
        if ( z < 2 )
            s = "Freq\n " + String (z + 1);
        if ( z > 1 )
            s = "Amp\n " + String (z - 1);
        if ( z < 3 )
            Sellfo[z] = new SELECT_WIDGET_C(base, s.c_str(), 300, 22 + (z * 108), 172, "Off\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16", 28);
        }
    UpdatePage (0, EFFECT_C::MAP_VOICE, 0);
    }

//#######################################################################
void PAGE_MAPPING_C::UpdatePage (byte ch, EFFECT_C effect, short value)
    {
    switch ( effect )
        {
        case EFFECT_C::MAP_VOICE:
            Selected = ch;
            for ( int z = 0;  z < MAP_COUNT;  z++ )     // process first column
                SelVoice[z]->Select (z == ch);
            if ( Selected < MAP_COUNT )
                {
                SelVoice[ch]->Set (value - 1);
                ch = 255;
                }
            else
                ch -= MAP_COUNT;

            for ( int z = 0;  z < MAP_COUNT;  z++ )     // Process second column
                SelNoise[z]->Select (z == ch);
            if ( ch  < MAP_COUNT )
                {
                SelNoise[ch]->Set (value);
                ch = 255;
                }
            else
                ch -= MAP_COUNT;

            for ( int z = 0;  z < MAP_COUNT - 1;  z++ )     // Process third column
                Sellfo[z]->Select (z == ch);
            if ( ch  < MAP_COUNT )
                {
                Sellfo[ch]->Set (value);
                ch = 255;
                }
            else
                ch -= MAP_COUNT;
        default:
            break;
        }
    }

//#######################################################################
//#######################################################################
static const char* TuneKeys[]   = { "7", "8" };
    PAGE_TUNE_C::PAGE_TUNE_C (lv_obj_t* base)
    {
    int x = 155;
    int y = 156;

    TuningFont = &lv_font_montserrat_48;
    lv_style_init (&TuningStyle);
    lv_style_set_text_font  (&TuningStyle, TuningFont);
    lv_style_set_text_color (&TuningStyle, lv_color_hex(0xF00000));

    TuningTitle = lv_label_create (base);
    lv_obj_set_pos    (TuningTitle, x + 45, 20);
    lv_label_set_text (TuningTitle, "TUNING MODE");
    lv_obj_add_style  (TuningTitle, &TuningStyle, 0);

    Note            = new NOTE_WIDGET_C     (base, x + 116, 80);
    Value           = new VALUE_WIDGET_C    (base, x + 151, 111, "D/A = ");
    TuneSelection   = new TUNE_OSC_WIDGET_C (base, 95, 340);
    FilterSelection = new TUNE_FLT_WIDGET_C (base, 530, 340);

    x = 19;
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++, x+=90 )
        LevelTuning[z] = new LEVEL_WIDGET_C (base, VoiceOptText[z], OscKeys[z], x, y, 80, LV_PALETTE_INDIGO);

    x += 81;
    for ( int z = 0;  z < FILTER_DEVICES; z++, x+=90 )
        LevelFilter[z] = new LEVEL_WIDGET_C (base, filterTitle[z], TuneKeys[z], x, y, 80, LV_PALETTE_INDIGO);
    }

//#######################################################################
void PAGE_TUNE_C::UpdatePage (byte ch, EFFECT_C effect, short value)
    {
    switch ( effect )
        {
        case EFFECT_C::MAX_LEVEL:
            LevelTuning[ch]->SetLevel (value);
            break;
        case EFFECT_C::NOTE:
            Note->SetValue (value);
            break;
        case EFFECT_C::SELECTED:
            TuneSelection->Set (ch);
            break;
        case EFFECT_C::ALTERNATE:
            TuneSelection->Set2 (ch);
            break;
        case EFFECT_C::VALUE:
            Value->Set (value);
            break;
        case EFFECT_C::FILTER:
            LevelFilter[ch]->SetLevel (value);
            break;
        case EFFECT_C::CONTROL:
            FilterSelection->Set (value);
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

    CalibFont = &lv_font_montserrat_48;
    lv_style_init (&CalibStyle);
    lv_style_set_text_font  (&CalibStyle, CalibFont);
    lv_style_set_text_color (&CalibStyle, lv_color_hex(0xF00000));

    ctitle = lv_label_create (base);
    lv_obj_set_pos    (ctitle, x + 45, 20);
    lv_label_set_text (ctitle, "Calibrating");
    lv_obj_add_style  (ctitle, &CalibStyle, 0);

    ctitle = lv_label_create (base);
    lv_obj_set_pos    (ctitle, x + 45, 80);
    lv_label_set_text (ctitle, "Please wait");
    lv_obj_add_style  (ctitle, &CalibStyle, 0);
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

    Selection  = new SELECT_WIDGET_C (panel, "Selection",   62, 20, 180, "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20", 32);
//    Selection->Select (true);

    MessageFont = &lv_font_montserrat_38;
    lv_style_init (&MessageStyle);
    lv_style_set_text_font (&MessageStyle, MessageFont);
    Message = lv_label_create (panel);
    lv_obj_add_style  (Message, &MessageStyle, 0);
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
                    str = "LOADING #" + String (SelectedValue);
                    break;
                case 2:
                    str = "SAVING #" + String (SelectedValue);
                    break;
                default:
                    str.clear ();
                    break;
                }
            SetMessage (str.c_str ());
            break;
        case EFFECT_C::VALUE:
            SetMessage ("");

            SelectedValue = value;
            Selection->Set (value - 1);
            break;
        default:
            break;
        }
    }

//#######################################################################
void PAGE_LOAD_SAVE_C::SetMessage (const char* str)
    {
    lv_label_set_text (Message, str);
    lv_obj_align      (Message, LV_ALIGN_BOTTOM_MID, 0, -11);
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

//    lvgl_port_lock (-1);    // Lock the mutex due to the LVGL APIs are not thread-safe

    // initialize keyboard sugguestions
    lv_style_init (&GlobalKeyStyle);
    lv_style_set_text_font (&GlobalKeyStyle, &lv_font_montserrat_12);
    lv_style_set_text_color (&GlobalKeyStyle, lv_palette_lighten (LV_PALETTE_GREY, 1));

    Pages = lv_tabview_create ( lv_scr_act (), LV_DIR_LEFT, 0);

    BasePageVoice = lv_tabview_add_tab (Pages, "");
    PageVoice     = new PAGE_OSC_C     (BasePageVoice);
    PageVoice->SetMidi (1);                              // initialize only the first voice page for midi allocation as default

    BasePageFilter = lv_tabview_add_tab (Pages, "");
    PageFilter     = new PAGE_FILTER_C  (BasePageFilter);
    PageFilter->SetMidi (1);                              // initialize only the first filter page for midi allocation as default

    BasePageMod         = lv_tabview_add_tab   (Pages, "");
    PageMod             = new PAGE_MOD_C       (BasePageMod);
    BasePageMap         = lv_tabview_add_tab   (Pages, "");
    PageMap             = new PAGE_MAPPING_C   (BasePageMap);
    BasePageCalibration = lv_tabview_add_tab   (Pages, "");
    PageCalibrate       = new PAGE_CALIBRATE_C (BasePageCalibration);
    BasePageTuning      = lv_tabview_add_tab   (Pages, "");
    PageTune            = new PAGE_TUNE_C      (BasePageTuning);
    BasePageLoadSave    = lv_tabview_add_tab   (Pages, "");
    PageLoadSave        = new PAGE_LOAD_SAVE_C (BasePageLoadSave);

    PageSelect (PAGE_C::PAGE_OSC);

//    lvgl_port_unlock ();    // Release the mutex
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
void GRPH_C::SetPage (byte num, byte midi)
    {
    switch ( num )
        {
        case (byte)PAGE_C::PAGE_OSC:
            PageVoice->SetMidi (midi);
            break;
        case (byte)PAGE_C::PAGE_FLT:
            PageFilter->SetMidi (midi);
            break;
        default:
            break;
        }
    PageSelect (num);
    }

//#######################################################################
void GRPH_C::PageSelect (PAGE_C page)
    {
    if ( CurrentPage != page )
        {
        CurrentPage = page;
        DBG ("Page Select %s (%d)", PageText[(byte)page], (byte)page);
        lv_tabview_set_act(Pages, (byte)page, LV_ANIM_OFF);
        }
    }

//#######################################################################
GRPH_C  Graphics;


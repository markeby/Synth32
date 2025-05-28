//#######################################################################
// Module:     Graphics.cpp
// Descrption: Graphics processing class
// Creator:    markeby
// Date:       7/23/2024
//#######################################################################

//   Graphics.h
//
//  Graphics for control displays
//
//  author: markeby
//
#pragma once

#include <lvgl.h>
#include "../Common/SynthCommon.h"
#include "../Common/DispMessages.h"
#include "Widgets.h"

extern lv_style_t  GlobalKeyStyle;

class ESP_Panel;
class TITLE_WIDGET_C;
class ADSR_METER_WIDGET_C;
class LEVEL_WIDGET_C;
class RAMP_WIDGET_C;
class PULSE_WIDGET_C;
class LFO_METER_WIDGET_C;
class LFO_METER_WIDGET_C;
class NOTE_WIDGET_C;
class MIDI_SEL_WIDGET_C;

class PAGE_TITLE_C
    {
protected:
    lv_obj_t*       Title;
    lv_style_t      TitleStyle;
        PAGE_TITLE_C (lv_obj_t* base, const char* str);
    };

//############################################
//############################################
class PAGE_OSC_C : protected PAGE_TITLE_C
    {
private:
    TITLE_WIDGET_C*         TitleControl[OSC_MIXER_COUNT];
    ADSR_METER_WIDGET_C*    MeterADSR[OSC_MIXER_COUNT];
    LEVEL_WIDGET_C*         MaxLevel[OSC_MIXER_COUNT];
    LEVEL_WIDGET_C*         SustainLevel[OSC_MIXER_COUNT];
    RAMP_WIDGET_C*          RampDir;
    PULSE_WIDGET_C*         PulseWidth;
    byte                    Midi;

public:
              PAGE_OSC_C (lv_obj_t* base);
    void      SetPage    (byte midi);
    void      UpdatePage (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value);
    byte      GetMidi    (void)     { return (this->Midi); }
    };

//############################################
//############################################
class PAGE_MOD_C : protected PAGE_TITLE_C
    {
private:

    typedef struct
        {
        TITLE_WIDGET_C*     TitleHard;
        LFO_METER_WIDGET_C* MeterHard;
        TEXT_INFO_C         HardLabelSine;
        TEXT_INFO_C         HardLabelRamp;
        TEXT_INFO_C         HardLabelPulse;
        bool                HardInUse[3];
        RAMP_WIDGET_C*      RampDir;
        PULSE_WIDGET_C*     PulseWidth;
        } LFO_C;
    LFO_C   LowFreq[2];

    TITLE_WIDGET_C*     TitleSoft;
    LFO_METER_WIDGET_C* MeterSoft;
    TEXT_INFO_C         SoftLabelSine;
    TEXT_INFO_C         SoftLabelTriangle;
    TEXT_INFO_C         SoftLabelRamp;
    TEXT_INFO_C         SoftLabelPulse;
    TEXT_INFO_C         SoftLabelNoise;
    bool                SoftInUse[5];

    void    CreateLFO         (int num, int x, int y, lv_obj_t* panel, const char* title, const char* mstr, const char* rs, const char* pws, const char* ssine, const char* sramp, const char* spulse);

public:
            PAGE_MOD_C        (lv_obj_t* base);
    void    UpdatePage        (byte index, byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value);
    void    UpdateHardButtons (short index, short value, bool sel);
    void    UpdateSoftButtons (short value, bool sel);
    };

//############################################
//############################################
class PAGE_FILTER_C : protected PAGE_TITLE_C
    {
private:

public:
              PAGE_FILTER_C (lv_obj_t* base);
    void      UpdatePage (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value);
    };

//############################################
//############################################
class PAGE_MAPPING_C
    {
private:
    lv_obj_t*           MidiTitle;
    lv_obj_t*           NoiseTitle;
    lv_obj_t*           ModTitle;
    lv_style_t          TitleStyle;
    SELECT_WIDGET_C*    SelVoice[MAP_COUNT];
    SELECT_WIDGET_C*    SelNoise[MAP_COUNT];
    SELECT_WIDGET_C*    SelOutput[MAP_COUNT];
    SELECT_WIDGET_C*    Sellfo[MAP_COUNT];
    short               Selected;

public:
              PAGE_MAPPING_C (lv_obj_t* base);
    void      UpdatePage (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value);
    };


//############################################
//############################################
class PAGE_TUNE_C
    {
private:
    const lv_font_t*        Tuningfont;
    lv_style_t              TuningStyle;
    lv_obj_t*               TuningTitle;
    NOTE_WIDGET_C*          Note;
    TITLE_WIDGET_C*         TitleTuning[OSC_MIXER_COUNT];
    LEVEL_WIDGET_C*         LevelTuning[OSC_MIXER_COUNT];
    TUNES_WIDGET_C*         TuneSelection;

public:
              PAGE_TUNE_C (lv_obj_t* base);
    void      UpdatePage (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value);
    };

//############################################
//############################################
class GRPH_C
    {
private:
    // Display device
    ESP_Panel*  Panel;

    // Page objects
    lv_obj_t*               Pages;
    DISP_MESSAGE_N::PAGE_C  CurrentPage;

    lv_obj_t*               BasePageVoice[MAP_COUNT];
    lv_obj_t*               BasePageMod;
    lv_obj_t*               BasePageMap;
    lv_obj_t*               BasePageFilter;
    lv_obj_t*               BasePageTuning;

    PAGE_OSC_C*             PageVoice[MAP_COUNT];
    PAGE_MOD_C*             PageMod;
    PAGE_MAPPING_C*         PageMap;
    PAGE_TUNE_C*            PageTune;
    PAGE_FILTER_C*          PageFilter;

public:
            GRPH_C              (void);
    void    Begin               (void);
    void    Pause               (bool state);
    void    SetPage             (byte num, byte midi);
    void    PageSelect          (DISP_MESSAGE_N::PAGE_C page);

    inline void    UpdatePageMap       (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value)              { PageMap->UpdatePage (ch, effect, value); }
    inline void    UpdatePageMod       (byte index, byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value)  { PageMod->UpdatePage (index, ch, effect, value); }
    inline void    UpdatePageTuning    (byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value)              { PageTune->UpdatePage (ch, effect, value); }
    inline void    UpdatePageVoice     (byte index, byte ch, DISP_MESSAGE_N::EFFECT_C effect, short value)  { PageVoice[index]->UpdatePage (ch, effect, value); }
    };

extern GRPH_C  Graphics;

//
//   Graphics.h
//
//  Graphics for control displays
//
//  author: markeby
//
#pragma once

#include <ESP_Panel_Library.h>
#include <lvgl.h>
#include "lvgl_port_v8.h"
#include "SynthData.h"
#include <demos/lv_demos.h>
#include <SynthCommon.h>

//############################################
enum class CNTRL_TYPE_C: byte {
    NONE   = 0,
    ADSR,
    POT,
    };

//############################################
class GRPH_C
    {
private:
    ESP_Panel* Panel;

    VCA_T     Oscillator[OSC_MIXER_COUNT];



public:
            GRPH_C          (void);
    void    Begin           (void);
    void    InitializePage1 (void);
    };

extern GRPH_C  Graphics;

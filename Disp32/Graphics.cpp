//#######################################################################
// Module:     Graphics.cpp
// Descrption: Graphics module
// Creator:    markeby
// Date:       7/7/2024
//#######################################################################
#define ALLOCATE_DISP_TEXT_REF 1

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
    lvgl_port_init (Panel->getLcd (), NULL);
    this->InitializePage1 ();
    }

//#######################################################################
void GRPH_C::InitializePage1 ()
    {
    int x = 8;
    int y = 0;

    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        MeterADSR[z] = new ADSR_WIDGET_C (ClassADSR[z], x, y);
        x += 160;
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
void GRPH_C::UpdatePageVCA (byte ch, byte effect, short value)
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

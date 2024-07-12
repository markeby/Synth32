//#######################################################################
// Module:     Graphics.cpp
// Descrption: Graphics module
// Creator:    markeby
// Date:       7/7/2024
//#######################################################################
#include "config.h"
#include "SynthData.h"
#include "Graphics.h"
#include "SynthData.h"
#include "DispFrontEnd.h"
#include "Widgets.h"

using namespace DISP_MESSAGE_N;

static const char* OSCILLATOR_TITLES[] =
    {
    "SINE",
    "TRIANGLE",
    "SAWTOOTH",
    "PULSE",
    "SQUARE"
    };

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
    this->InitializePage1 ();
    }

//#######################################################################
void GRPH_C::InitializePage1 ()
    {
    int x = 8;
    int y = 0;

    lvgl_port_lock (-1);    // Lock the mutex due to the LVGL APIs are not thread-safe
    for ( int z = 0;  z < OSC_MIXER_COUNT;  z++ )
        {
        VCA_T& v = Oscillator[z];
        v.Title  = OSCILLATOR_TITLES[z];
        v.Meter  = new ADSR_WIDGET_C (OSCILLATOR_TITLES[z], x, y);
        x += 160;
        }

    lvgl_port_unlock ();    // Release the mutex
    }

GRPH_C  Graphics;

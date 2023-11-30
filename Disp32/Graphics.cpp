//#######################################################################
// Module:     Graphics.cpp
// Descrption: Graphics module
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include <SPI.h>
#include <Arduino_GFX_Library.h>

#include "config.h"
#include "SynthData.h"
#include "Graphics.h"
#include "SynthData.h"
#include "DispFrontEnd.h"

using namespace DISP_MESSAGE_N;

int  OscMixerColor[OSC_MIXER_COUNT] = { GREEN, RED, PURPLE, CYAN, ORANGE };

namespace GRPH_DISP_LOCAL_N
    {
    // display pins
    #define TFT_BL      27
    #define TFT_CS      15
    #define TFT_DC       2
    #define TFT_MISO    12
    #define TFT_MOSI    13
    #define TFT_SCLK    14
    #define TFT_RST     -1

    // display general parameters
    #define MAX_Y       (480 - 1)
    #define MAX_X       (320 - 1)
    #define BACKGRND    BLACK
    #define WLINE       WHITE
    #define FUZZ_GREY   RGB565(60, 62 ,60)

    // Vector display parameters
    #define BASE_Y      100
    #define BASE_X      6

    #define INTERVAL_Y  (BASE_Y / 10)

    #define DELTA_Y     (BASE_Y - 1)
    #define DELTA_X     (MAX_X - BASE_X)

    #define DELTA_YF    ((float)DELTA_Y)
    #define DELTA_XF    ((float)DELTA_X)

    #define TOP_Y       (BASE_Y - DELTA_Y)
    #define TOP_X       (BASE_X + DELTA_X)

    Arduino_ESP32SPI *bus = new Arduino_ESP32SPI (TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO);
    Arduino_GFX      *gfx = new Arduino_ST7796   (bus, DF_GFX_RST, 0 /* rotation */, false /* IPS */);
    }// end namespace GRPH_DIAP_LOCAL_N


using namespace GRPH_DISP_LOCAL_N;

//#######################################################################
GRPH_C::GRPH_C ()
    {
    pinMode (TFT_BL, OUTPUT);           // Setup backlighting
    digitalWrite (TFT_BL, HIGH);
    }

//#######################################################################
void GRPH_C::Begin ()
    {
    gfx->begin ();
    gfx->fillScreen (PINK);
    }

//#######################################################################
void GRPH_C::DrawGrid ()
    {
    gfx->fillScreen (BACKGRND);
    gfx->drawLine (BASE_X - 2, BASE_Y + 2, BASE_X - 2, 0, WLINE);
    gfx->drawLine (BASE_X - 2, BASE_Y + 2, MAX_X, BASE_Y + 2, WLINE);

    for ( int z = BASE_Y;  z >= 0;  z -= INTERVAL_Y )
        gfx->drawLine (0, z, 3, z, WLINE);
    }

//#######################################################################
void GRPH_C::DeleteADSR (byte ch)
    {
    int x, y, x1, y1;

    while ( SynthD.GetVector (ch, x, y, x1, y1) )
        gfx->drawLine (x, y, x1, y1, BACKGRND);
    SynthD.InitSaveVectors (ch);                   // Initialize vector save buffers
    }

//#######################################################################
void GRPH_C::ShowADSR (byte ch)
    {
    int x_axis;
    int y_axis;
    int x_now;
    int y_now;
    float mult;
    char buff[16];

    switch ( (SHAPE_C)ch )
        {
        case SHAPE_C::ALL:
            if ( DebugGraphics )
                printf ("\n[g] ShowADSR draw all active", ch);

            this->DrawGrid ();
            for (int z = 0;  z < (int)(SHAPE_C::ALL);  z++)
                {
                if ( SynthD.GetActive (z) )
                    this->ShowADSR(z);
                }
            break;
        default:
            {
            if ( DebugGraphics )
                printf ("\n[g] ShowADSR %s ch = %d\n", SynthD.GetName (ch).c_str (), ch);

            // Delete lines from previous traces.
            DeleteADSR (ch);

            // Determine time accross display witdth
            if ( SynthD.GetNoSustainTime (ch) )
                mult = 1.0 / (SynthD.GetAttackTime (ch) + SynthD.GetDecayTime (ch) + SynthD.GetReleaseTime (ch));
            else
                mult = 1.0 / (SynthD.GetAttackTime (ch) + SynthD.GetDecayTime (ch) + SynthD.GetSustainTime (ch) + SynthD.GetReleaseTime (ch));

            // Draw attack line
            x_axis = BASE_X + (DELTA_XF * (SynthD.GetAttackTime (ch) * mult));
            y_axis = BASE_Y - (DELTA_YF * SynthD.GetMaxLevel (ch));
            gfx->drawLine (BASE_X, BASE_Y, x_axis, y_axis, SynthD.GetColor (ch));
            SynthD.SaveVector (ch, BASE_X, BASE_Y, x_axis, y_axis);
            gfx->drawLine (x_axis, BASE_Y + 3, x_axis, BASE_Y + 7, WHITE);
            SynthD.SaveVector (ch, x_axis, BASE_Y + 3, x_axis, BASE_Y + 7);
            gfx->setCursor (x_axis - 6, BASE_Y + 10);
            gfx->setTextColor (WHITE);
            gfx->setTextSize (2);
            sprintf (buff, "%d", (int)(SynthD.GetAttackTime (ch) * TIME_MULT));
            gfx->println (buff);

            // Determine sustain level not to exceed max level.
            float max = SynthD.GetSustainLevel (ch);
            if ( max >  SynthD.GetMaxLevel (ch) )
                max = SynthD.GetMaxLevel (ch);

            // Draw decay line
            x_now = x_axis + (DELTA_XF * (SynthD.GetDecayTime (ch) * mult));
            y_now = BASE_Y - (DELTA_YF * max);
            gfx->drawLine(x_axis, y_axis, x_now, y_now, SynthD.GetColor (ch));
            SynthD.SaveVector (ch, x_axis, y_axis, x_now, y_now);
            gfx->drawLine (x_now, BASE_Y + 3, x_now, BASE_Y + 7, WHITE);
            SynthD.SaveVector (ch, x_now, BASE_Y + 3, x_now, BASE_Y + 7);

            // Draw a seperator line if infinate sustain
            if ( SynthD.GetNoSustainTime (ch) )
                {
                gfx->drawLine (x_now, BASE_Y, x_now, TOP_Y, FUZZ_GREY);
                SynthD.SaveVector (ch, x_now, BASE_Y, x_now, TOP_Y);
                }
            else
                {
                // Draw sustain line
                x_axis = x_now + (DELTA_XF * (SynthD.GetSustainTime (ch) * mult));
                gfx->drawLine (x_now, y_now, x_axis, y_now, SynthD.GetColor (ch));
                SynthD.SaveVector (ch, x_now, y_now, x_axis, y_now);
                gfx->drawLine (x_axis, BASE_Y + 3, x_axis, BASE_Y + 7, WHITE);
                SynthD.SaveVector (ch, x_axis, BASE_Y + 3, x_axis, BASE_Y + 7);
                x_now = x_axis;
                }

            // Draw release line
            x_axis = x_now + (DELTA_XF * (SynthD.GetReleaseTime (ch) * mult));
            gfx->drawLine (x_now, y_now, x_axis, BASE_Y, SynthD.GetColor (ch));
            SynthD.SaveVector (ch, x_now, y_now, x_axis, BASE_Y);
            gfx->drawLine (x_axis, BASE_Y + 3, x_axis, BASE_Y + 7, WHITE);
            SynthD.SaveVector (ch, x_axis, BASE_Y + 3, x_axis, BASE_Y + 7);
            }
            break;
        }
    }

//#######################################################################
GRPH_C  Graphics;

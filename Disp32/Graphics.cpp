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

    // Vector display parameters
    #define BASE_Y      170
    #define BASE_X      6
    #define DELTA_Y     (BASE_Y - 1)
    #define QUARTER_X   ((MAX_X - BASE_X) / 4)
    #define INTERVAL_Y  (BASE_Y / 10)

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

    for ( int z = BASE_Y - INTERVAL_Y;  z > 0;  z -= INTERVAL_Y )
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
    int totalt;
    int currenty;
    int thist;
    int thisy;
    int max;

    switch ( (SHAPE_C)ch )
        {
        case SHAPE_C::ALL:
            if ( DebugGraphics )
                printf ("[g] ShowADSR draw all active\n", ch);

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
                printf ("[g] ShowADSR %s ch = %d\n", SynthD.GetName (ch).c_str (), ch);
            DeleteADSR (ch);

            totalt = BASE_X + (float)((float)QUARTER_X * PRS_SCALER)  * (float)(SynthD.GetAttackTime (ch) + 1);
            currenty = DELTA_Y - ((float)(DELTA_Y * PRS_SCALER) * (float)(SynthD.GetLimitLevel (ch) + 1));
            gfx->drawLine (BASE_X, BASE_Y, totalt, currenty, SynthD.GetColor (ch));
            SynthD.SaveVector (ch, BASE_X, BASE_Y, totalt, currenty);
            if ( SynthD.GetSustainTime (ch) == 0 )
                {
                thist = totalt;
                totalt = MAX_X - QUARTER_X;
                gfx->drawLine(thist, currenty, totalt, currenty, SynthD.GetColor (ch));
                SynthD.SaveVector (ch, thist, currenty, totalt, currenty);
                }
            else
                {
                if ( SynthD.GetDecayTime (ch) > 0 )
                    {
                    max = SynthD.GetSustainLevel (ch);
                    if ( max >  SynthD.GetLimitLevel (ch) )
                        max = SynthD.GetLimitLevel (ch);
                    thist = totalt;
                    totalt += (float)((float)QUARTER_X * PRS_SCALER)  * (float)SynthD.GetDecayTime (ch);
                    thisy = BASE_Y - ((float)(DELTA_Y * PRS_SCALER) * (float)SynthD.GetSustainLevel (ch));
                    gfx->drawLine(thist, currenty, totalt, thisy, SynthD.GetColor (ch));
                    SynthD.SaveVector (ch, thist, currenty, totalt, thisy);
                    currenty = thisy;
                    }
                else
                    currenty =(BASE_Y - 1) - ((float)(DELTA_Y * PRS_SCALER) * (float)SynthD.GetSustainLevel (ch));

                thist = totalt;
                totalt += (float)((float)QUARTER_X * PRS_SCALER)  * (float)SynthD.GetSustainTime (ch);
                gfx->drawLine (currenty, thist, currenty, totalt, SynthD.GetColor (ch));
                SynthD.SaveVector (ch, currenty, thist, currenty, totalt);
                }

            thist = totalt;
            totalt += (float)((float)QUARTER_X * PRS_SCALER)  * (float)SynthD.GetReleaseTime (ch);
            gfx->drawLine (thist, currenty, totalt, BASE_Y - 1, SynthD.GetColor (ch));
            SynthD.SaveVector (ch, thist, currenty, totalt, BASE_Y - 1);
            }
            break;
        }
    }

//#######################################################################
GRPH_C  Graphics;

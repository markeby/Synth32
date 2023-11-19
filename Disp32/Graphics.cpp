//#######################################################################
// Module:     Graphics.cpp
// Descrption: Graphics module
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>

#include "config.h"
#include "SynthData.h"
#include "Graphics.h"
#include "SynthData.h"
#include "DispFrontEnd.h"

using namespace DISP_MESSAGE_N;

#define TFT_BL   21
#define TFT_CS   15
#define TFT_DC    2
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_RST  -1

#define MAX_X   (ILI9341_TFTWIDTH - 1)
#define MAX_Y   (ILI9341_TFTHEIGHT - 1)

#define BASE_Y      50
#define BASE_X      4

//#######################################################################
GRPH_C::GRPH_C ()
    {
    pDisp = new Adafruit_ILI9341 (TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST, TFT_MISO);
    }

//#######################################################################
void GRPH_C::DrawGrid ()
    {
    int base_y = 50;
    int base_x = 4;

    pDisp->fillScreen(ILI9341_DARKGREY);
    pDisp->drawLine (BASE_Y, 4, BASE_Y, MAX_Y, ILI9341_WHITE);
    pDisp->drawLine (BASE_Y, 4, MAX_X, 4, ILI9341_WHITE);
    int interval = (MAX_X - base_y - 4) / 10;
    for ( int z = base_y;  z < MAX_X;  z += interval )
        pDisp->drawLine (z, 0, z, 8, ILI9341_WHITE);
    }

//#######################################################################
void GRPH_C::Begin ()
    {
    // Grpahics startup
    pinMode (TFT_BL, OUTPUT);           // Setup backlighting
    digitalWrite (TFT_BL, HIGH);

    FullTime = TIME_MULT * 127 * 4;

    pDisp->begin ();

    pDisp->fillScreen(ILI9341_OLIVE);

    }

//#######################################################################
void GRPH_C::ShowADSR (byte wave)
    {
    if ( DebugGraphics )
        printf ("[g] ShowADSR with wave %2.2X\n", wave);

    switch ( (SHAPE_C)wave )
        {
        case SHAPE_C::ALL:
            if ( DebugGraphics )
                printf ("[g] ShowADSR draw all active\n", wave);

            this->DrawGrid ();
            for (int z = 0;  z < (int)(SHAPE_C::ALL);  z++)
                {
                if ( SynthData.GetActive (z) )
                    this->ShowADSR(z);
                }
            break;
        default:
            {


            }
            break;
        }
    }

//#######################################################################
GRPH_C  Graphics;

//#######################################################################
// Module:     Graphics.cpp
// Descrption: Graphics module
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include <lvgl.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#include "config.h"
#include "SynthData.h"
#include "Graphics.h"
#include "SynthData.h"
#include "DispFrontEnd.h"

using namespace DISP_MESSAGE_N;

int  OscMixerColor[OSC_MIXER_COUNT] = { TFT_GREEN, TFT_RED, TFT_PURPLE, TFT_CYAN, TFT_ORANGE };

namespace GRPH_DISP_LOCAL_N
    {
    static const uint32_t       screenWidth  = 320;
    static const uint32_t       screenHeight = 480;
    static lv_disp_draw_buf_t   draw_buf;
    static lv_color_t           buf[2][480 * 29];

    // display general parameters
    #define MAX_Y       (480 - 1)
    #define MAX_X       (320 - 1)
    #define BACKGRND    TFT_BLACK
    #define WLINE       TFT_WHITE
    #define FUZZ_GREY   WLINE

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

    // Class for tft device
    class LGFX : public lgfx::LGFX_Device
        {
        lgfx::Panel_ST7796  _panel_instance;
        lgfx::Bus_SPI       _bus_instance;
        lgfx::Light_PWM     _light_instance;
        lgfx::Touch_XPT2046 _touch_instance;
    //-------------------------------------------
    public:
        LGFX (void)
            {
                {
                auto cfg = _bus_instance.config ();

                cfg.spi_host    = SPI2_HOST;
                cfg.spi_mode    = 0;
                cfg.freq_write  = 80000000;
                cfg.freq_read   = 16000000;
                cfg.spi_3wire   = false;
                cfg.use_lock    = true;
                cfg.dma_channel =  1;
                cfg.dma_channel = SPI_DMA_CH_AUTO;
                cfg.pin_sclk    = 14;
                cfg.pin_mosi    = 13;
                cfg.pin_miso    = 12;
                cfg.pin_dc      =  2;

                _bus_instance.config (cfg);
                _panel_instance.setBus (&_bus_instance);
                }
                {
                auto cfg = _panel_instance.config ();
                cfg.pin_cs           =    15; // CS  (-1 = disable)
                cfg.pin_rst          =    -1; // RST (-1 = disable)
                cfg.pin_busy         =    -1; // BUSY(-1 = disable)
                cfg.memory_width     =   320;
                cfg.memory_height    =   480;
                cfg.panel_width      =   320;
                cfg.panel_height     =   480;
                cfg.offset_x         =     0;
                cfg.offset_y         =     0;
                cfg.offset_rotation  =     0; //  0~7
                cfg.dummy_read_pixel =     8;
                cfg.dummy_read_bits  =     1;
                cfg.readable         = false;
                cfg.invert           = false;
                cfg.rgb_order        = false;
                cfg.dlen_16bit       = false;
                cfg.bus_shared       = false;
                _panel_instance.config (cfg);
                }
                {
                auto cfg        = _light_instance.config ();
                cfg.pin_bl      = 27;             //  BL
                cfg.invert      = false;
                cfg.freq        = 44100;
                cfg.pwm_channel = 7;
                _light_instance.config (cfg);
                _panel_instance.setLight (&_light_instance);
                }
                {
                auto cfg = _touch_instance.config ();
                cfg.x_min      = 222;    // 360  222
                cfg.x_max      = 3367;   //  4200 3367
                cfg.y_min      = 192;    //  180  192
                cfg.y_max      = 3732;   //  3900 3732
                cfg.pin_int    = -1;     // INT, TP IRQ
                cfg.bus_shared = true;
                cfg.offset_rotation = 6; //  0~7
                // SPI
                cfg.spi_host = SPI2_HOST; // HSPI_HOST or VSPI_HOST
                cfg.freq = 1000000;      // SPI
                cfg.pin_sclk = 14;       // SCLK, TP CLK
                cfg.pin_mosi = 13;       // MOSI, TP DIN
                cfg.pin_miso = 12;       // MISO, TP DOUT
                cfg.pin_cs   = 33;       // CS, TP CS
                _touch_instance.config (cfg);
                _panel_instance.setTouch (&_touch_instance);
                }
            setPanel (&_panel_instance);
            }
        };

    LGFX tft;

    //#######################################################################
    void MyDispFlush (lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p)
        {
        if ( tft.getStartCount () == 0 )  // Run if not already started
            tft.startWrite();
        tft.pushImageDMA (area->x1
                          , area->y1
                          , area->x2 - area->x1 + 1
                          , area->y2 - area->y1 + 1
                          , (lgfx::swap565_t *)&color_p->full);
        lv_disp_flush_ready (disp);
        }

    //#######################################################################
    void InitDisplay ()
        {
        static lv_disp_drv_t disp_drv;                      // Descriptor of a display driver

        lv_disp_drv_init (&disp_drv);                       // Basic initialization
        disp_drv.flush_cb = MyDispFlush;                    // Set your driver function
        disp_drv.draw_buf = &draw_buf;                      // Assign the buffer
        disp_drv.hor_res  = screenWidth;                    // horizontal resolution
        disp_drv.ver_res  = screenHeight;                   // vertical resolution
        lv_disp_drv_register (&disp_drv);                   // Finally register the driver
        lv_disp_set_bg_color (NULL, lv_color_hex(0x0000));  // background black
        }

    }// end namespace GRPH_DIAP_LOCAL_N

using namespace GRPH_DISP_LOCAL_N;

//#######################################################################
GRPH_C::GRPH_C ()
    {
    }

//#######################################################################
void GRPH_C::Begin ()
    {
    adc_power_acquire ();    // Bug fixes for GPIO39 and GPIO36

    tft.begin ();
    tft.setRotation (0);       // USB Right 1
    tft.setBrightness (200);
    tft.setSwapBytes (true);
    lv_init();
    lv_disp_draw_buf_init (&draw_buf, buf[0], buf[1], 480 * 29);
    InitDisplay ();
    tft.fillScreen (0x001F);
    }

//#######################################################################
void GRPH_C::DrawGrid ()
    {
    tft.fillScreen (BACKGRND);
    tft.drawLine (BASE_X - 2, BASE_Y + 2, BASE_X - 2, 0, WLINE);
    tft.drawLine (BASE_X - 2, BASE_Y + 2, MAX_X, BASE_Y + 2, WLINE);

    for ( int z = BASE_Y;  z >= 0;  z -= INTERVAL_Y )
        tft.drawLine (0, z, 3, z, WLINE);
    }

//#######################################################################
void GRPH_C::DeleteADSR (byte ch)
    {
    int x, y, x1, y1;

    while ( SynthD.GetVector (ch, x, y, x1, y1) )
        tft.drawLine (x, y, x1, y1, BACKGRND);
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
            tft.drawLine (BASE_X, BASE_Y, x_axis, y_axis, SynthD.GetColor (ch));
            SynthD.SaveVector (ch, BASE_X, BASE_Y, x_axis, y_axis);
            tft.drawLine (x_axis, BASE_Y + 3, x_axis, BASE_Y + 7, TFT_WHITE);
            SynthD.SaveVector (ch, x_axis, BASE_Y + 3, x_axis, BASE_Y + 7);
            tft.setCursor (x_axis - 6, BASE_Y + 10);
            tft.setTextColor (TFT_WHITE);
            tft.setTextSize (2);
            sprintf (buff, "%d", (int)(SynthD.GetAttackTime (ch) * TIME_MULT));
            tft.println (buff);

            // Determine sustain level not to exceed max level.
            float max = SynthD.GetSustainLevel (ch);
            if ( max >  SynthD.GetMaxLevel (ch) )
                max = SynthD.GetMaxLevel (ch);

            // Draw decay line
            x_now = x_axis + (DELTA_XF * (SynthD.GetDecayTime (ch) * mult));
            y_now = BASE_Y - (DELTA_YF * max);
            tft.drawLine(x_axis, y_axis, x_now, y_now, SynthD.GetColor (ch));
            SynthD.SaveVector (ch, x_axis, y_axis, x_now, y_now);
            tft.drawLine (x_now, BASE_Y + 3, x_now, BASE_Y + 7, TFT_WHITE);
            SynthD.SaveVector (ch, x_now, BASE_Y + 3, x_now, BASE_Y + 7);

            // Draw a seperator line if infinate sustain
            if ( SynthD.GetNoSustainTime (ch) )
                {
                tft.drawLine (x_now, BASE_Y, x_now, TOP_Y, FUZZ_GREY);
                SynthD.SaveVector (ch, x_now, BASE_Y, x_now, TOP_Y);
                }
            else
                {
                // Draw sustain line
                x_axis = x_now + (DELTA_XF * (SynthD.GetSustainTime (ch) * mult));
                tft.drawLine (x_now, y_now, x_axis, y_now, SynthD.GetColor (ch));
                SynthD.SaveVector (ch, x_now, y_now, x_axis, y_now);
                tft.drawLine (x_axis, BASE_Y + 3, x_axis, BASE_Y + 7, TFT_WHITE);
                SynthD.SaveVector (ch, x_axis, BASE_Y + 3, x_axis, BASE_Y + 7);
                x_now = x_axis;
                }

            // Draw release line
            x_axis = x_now + (DELTA_XF * (SynthD.GetReleaseTime (ch) * mult));
            tft.drawLine (x_now, y_now, x_axis, BASE_Y, SynthD.GetColor (ch));
            SynthD.SaveVector (ch, x_now, y_now, x_axis, BASE_Y);
            tft.drawLine (x_axis, BASE_Y + 3, x_axis, BASE_Y + 7, TFT_WHITE);
            SynthD.SaveVector (ch, x_axis, BASE_Y + 3, x_axis, BASE_Y + 7);
            }
            break;
        }
    }

//#######################################################################
GRPH_C  Graphics;

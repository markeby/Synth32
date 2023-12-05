//#######################################################################
// Module:     Graphics.hpp
// Descrption: Graphics support module
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <lvgl.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

int  OscMixerColor[OSC_MIXER_COUNT] = { TFT_GREEN, TFT_RED, TFT_PURPLE, TFT_CYAN, TFT_ORANGE };

namespace GRPH_DISP_N
    {
    static const uint32_t       screenWidth  = 320;
    static const uint32_t       screenHeight = 480;
    static lv_disp_draw_buf_t   draw_buf;
    static lv_color_t           buf[2][480 * 29];

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

    // display general parameters
    #define MAX_Y       (480 - 1)
    #define MAX_X       (320 - 1)
    #define BACKGRND    TFT_BLACK
    #define WTEXT       TFT_WHITE

    #define KNOB_OFF_COLOR      TFT_DARKGREY
    #define KNOB_BASE_ANGLE     115.0
    #define KNOB_FULL_ROTATION  320.0
    #define KNOB_FULL_ANGLE     (KNOB_BASE_ANGLE + KNOB_FULL_ROTATION)
    #define KNOB_DIAMETER       12
    #define KNOB_WIDTH          3
    #define KNOB_TEXT_Y         20
    #define FADER_TOP           (KNOB_DIAMETER + KNOB_WIDTH)
    #define FADER_HEIGHT        (FADER_TOP * 2)
    #define FADER_WIDTH         6
    #define FADER_WIDTH_OFFSET  (FADER_WIDTH / 2)
    #define FADER_TEXT_Y        19
    #define FADER_DEL_X         8
    #define FADER_DEL_W         18
    #define FADER_DEL_H         (FADER_HEIGHT + 2)

    typedef struct
        {
        CNTRL_TYPE_C    Type;       // Control type
        String          Label;      // Control label
        int             LabelX;     // Label X position
        int             Color;      // Color of control that is on
        int             PosX;       // Control X position
        float           Range;      // Value of control at 100%
        } CONTROL_T;

    #define NUM_CNT_PAGE1   6

    String WaveShapes[OSC_MIXER_COUNT] = { "Sine", "Triangle", "Square", "Sawtooth", "Pulse" };

    CONTROL_T     ControlPageOsc1[] =
        { {CNTRL_TYPE_C::POT,   "Attack",   61, TFT_GREEN,         84, 2540 },
          {CNTRL_TYPE_C::POT,   "Decay",   113, TFT_GREENYELLOW,  132, 2540 },
          {CNTRL_TYPE_C::FADER, "Sustain", 168, TFT_YELLOW,       175,  100 },
          {CNTRL_TYPE_C::POT,   "",        182, TFT_YELLOW,       207, 2540 },
          {CNTRL_TYPE_C::POT,   "Release", 234, TFT_RED,          255, 2540 },
          {CNTRL_TYPE_C::FADER, "Max",     290, TFT_MAGENTA,      299,  100 },
        };

    String BlankingString = "      ";

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

    }// end namespace GRPH_DISP_N



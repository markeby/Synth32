//#######################################################################
// Module:     Graphics.hpp
// Descrption: Graphics support module
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#define ESP32_4827S043

#include <U8g2lib.h>
#include <Arduino_GFX_Library.h>

int  OscMixerColor[OSC_MIXER_COUNT] = { GREEN, RED, PURPLE, CYAN, ORANGE };

namespace GRPH_DISP_N
    {
    #define GFX_BL 2
    Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel
       (GFX_NOT_DEFINED,    // CS
        GFX_NOT_DEFINED,    // SCK
        GFX_NOT_DEFINED,    // SDA
        40,                 // DE
        41,                 // VSYNC
        39,                 // HSYNC
        42,                 // PCLK
        45,                 // R0
        48,                 // R1
        47,                 // R2
        21,                 // R3
        14,                 // R4
        5,                  // G0
        6,                  // G1
        7,                  // G2
        15,                 // G3
        16,                 // G4
        4,                  // G5
        8,                  // B0
        3,                  // B1
        46,                 // B2
        9,                  // B3
        1                   // B4
    );
    // option 1:
    // ST7262 IPS LCD 800x480
     Arduino_RPi_DPI_RGBPanel *pGFX = new Arduino_RPi_DPI_RGBPanel
      (bus,
       800,         // width
       0,           // hsync_polarity
       8,           // hsync_front_porch
       4,           // hsync_pulse_width
       8,           // hsync_back_porch
       480,         // height
       0,           // vsync_polarity
       8,           // vsync_front_porch
       4,           // vsync_pulse_width
       8,           // vsync_back_porch
       1,           // pclk_active_neg
       16000000,    // prefer_speed
       true         // auto_flush
       );

    // display general parameters
    #define MAX_Y       (800 - 1)
    #define MAX_X       (480 - 1)
    #define BACKGRND    BLACK
    #define WTEXT       WHITE

    #define KNOB_OFF_COLOR      DARKGREY
    #define KNOB_DIAMETER       22
    #define KNOB_SPACING_Y      90
    #define KNOB_OFFSET_Y       57
    #define KNOB_WIDTH          3
    #define KNOB_TEXT_Y         52
    #define KNOB_BASE_ANGLE     115.0
    #define KNOB_FULL_ROTATION  320.0
    #define KNOB_FULL_ANGLE     (KNOB_BASE_ANGLE + KNOB_FULL_ROTATION)
    #define FADER_TOP           (KNOB_DIAMETER + KNOB_WIDTH)
    #define FADER_HEIGHT        (FADER_TOP * 2)
    #define FADER_WIDTH         6
    #define FADER_WIDTH_OFFSET  (FADER_WIDTH / 2)
    #define FADER_TEXT_Y        (FADER_TOP + 23)
    #define FADER_DEL_X         8
    #define FADER_DEL_W         18
    #define FADER_DEL_H         (FADER_HEIGHT + 2)
    #define KNOB_FONT           u8g2_font_crox5h_tf
    #define LABEL_FONT          u8g2_font_crox4tb_tf

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

    String WaveShapes[OSC_MIXER_COUNT] = { "Sine", "Triangle", "Sawtooth", "Pulse", "Square" };

    CONTROL_T     ControlPageOsc1[] =
        { {CNTRL_TYPE_C::POT,   "Attack",   95, GREEN,        120, 2540 },
          {CNTRL_TYPE_C::POT,   "Decay",   185, GREENYELLOW,  220, 2540 },
          {CNTRL_TYPE_C::FADER, "Sustain", 310, YELLOW,       300,  100 },
          {CNTRL_TYPE_C::POT,   "",        400, YELLOW,       380, 2540 },
          {CNTRL_TYPE_C::POT,   "Release", 440, RED,          470, 2540 },
          {CNTRL_TYPE_C::FADER, "Max",     533, MAGENTA,      550,  100 },
        };

    String BlankingString = "        ";

    }// end namespace GRPH_DISP_N


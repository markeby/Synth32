//#######################################################################
// Module:     Graphics.cpp
// Descrption: Graphics module
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>

#include "config.h"
#include "SynthData.h"
#include "Graphics.h"
#include "SynthData.h"
#include "DispFrontEnd.h"
#include "Graphics.hpp"

using namespace DISP_MESSAGE_N;
using namespace GRPH_DISP_N;

//#######################################################################
DISP_CTRL_C::DISP_CTRL_C (CNTRL_TYPE_C _t, int _x, int _y, float _range, int _color) :
                         Type (_t), PosX (_x), PosY (_y), Range (_range), Color (_color), Active (false)
    {
    TextX = _x - 13;

    switch ( Type )
        {
        case CNTRL_TYPE_C::FADER:
            TextY = _y + 22;
            break;
        case CNTRL_TYPE_C::POT:
            TextY = _y + 15;
            break;
        default:
            break;
        }
    }

//#######################################################################
DISP_CTRL_C::~DISP_CTRL_C ()
    {
    if ( Active )
        {
        switch ( Type )
            {
            case CNTRL_TYPE_C::FADER:
                break;
            case CNTRL_TYPE_C::POT:
                tft.fillArc (PosX, PosY, KNOB_DIAMETER, KNOB_DIAMETER + KNOB_WIDTH, KNOB_BASE_ANGLE, KNOB_FULL_ROTATION + KNOB_BASE_ANGLE, BACKGRND);
                break;
            default:
                break;
            }
        }
    }

//#######################################################################
void DISP_CTRL_C::Set (float val)
    {
    Active = true;

    tft.setFont(&fonts::Font2);
    switch ( Type )
        {
        case CNTRL_TYPE_C::FADER:
            break;
        case CNTRL_TYPE_C::POT:
            tft.fillArc (PosX, PosY, KNOB_DIAMETER, KNOB_DIAMETER + KNOB_WIDTH, KNOB_BASE_ANGLE, KNOB_FULL_ROTATION + KNOB_BASE_ANGLE, KNOB_OFF_COLOR);
            tft.fillArc (PosX, PosY, KNOB_DIAMETER, KNOB_DIAMETER + KNOB_WIDTH, KNOB_BASE_ANGLE, (val * KNOB_FULL_ROTATION) + KNOB_BASE_ANGLE, Color);
            tft.drawString (BlankingString, TextX, TextY);
            tft.drawString (String ((int)(val * Range) + 1), TextX, TextY);
            break;
        default:
            break;
        }

    }

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
void GRPH_C::InitPageOsc1 ()
    {
    tft.fillScreen (BACKGRND);

    tft.setFont(&fonts::Font2);

    for ( int z = 0;  z < NUM_CNT_PAGE1;  z++ )
        {
        CONTROL_T& cnt = ControlPageOsc1[z];
        tft.drawString (cnt.Label, cnt.LabelX, 10);
        }
    }

//#######################################################################
void GRPH_C::ShowADSR (byte ch)
    {
    DISP_CTRL_C* pd;

    switch ( (SHAPE_C)ch )
        {
        case SHAPE_C::ALL:
            if ( DebugGraphics )
                printf ("\n[g] ShowADSR draw all active", ch);

            this->InitPageOsc1 ();
            for ( int z = 0;  z < (int)(SHAPE_C::ALL);  z++ )
                {
                for ( int zc = 0;  zc < NUM_CNT_PAGE1;  zc++ )
                    {
                    CONTROL_T& cta = ControlPageOsc1[0];
                    pd = new DISP_CTRL_C (cta.Type, cta.PosX, 65 + (z * 60), cta.Range, cta.Color);
                    SynthD.SetDispPDispAttackTime   (z, pd);
                    CONTROL_T& ctd = ControlPageOsc1[1];
                    pd = new DISP_CTRL_C (ctd.Type, ctd.PosX, 65 + (z * 60), ctd.Range, ctd.Color);
                    SynthD.SetDispPDispADecayTime   (z, pd);
                    CONTROL_T& ctsl = ControlPageOsc1[2];
                    pd = new DISP_CTRL_C (ctsl.Type, ctsl.PosX, 65 + (z * 60), ctsl.Range, ctsl.Color);
                    SynthD.SetDispPDispSustainLevel (z, pd);
                    CONTROL_T& cts = ControlPageOsc1[3];
                    pd = new DISP_CTRL_C (cts.Type, cts.PosX, 65 + (z * 60), cts.Range, cts.Color);
                    SynthD.SetDispPDispSustainTime  (z, pd);
                    CONTROL_T& ctr = ControlPageOsc1[4];
                    pd = new DISP_CTRL_C (ctr.Type, ctr.PosX, 65 + (z * 60), ctr.Range, ctr.Color);
                    SynthD.SetDispPDispReleaseTime  (z, pd);
                    CONTROL_T& ctm = ControlPageOsc1[5];
                    pd = new DISP_CTRL_C (ctm.Type, ctm.PosX, 65 + (z * 60), ctm.Range, ctm.Color);
                    SynthD.SetDispPDispLimitLevel   (z, pd);
                    }
                this->ShowADSR(z);
                }
            break;
        default:
            {
            if ( DebugGraphics )
                printf ("\n[g] ShowADSR %s ch = %d\n", SynthD.GetName (ch).c_str (), ch);

            SynthD.GetDispPAttackTime   (ch)->Set (SynthD.GetAttackTime   (ch));
            SynthD.GetDispPADecayTime   (ch)->Set (SynthD.GetDecayTime    (ch));
            SynthD.GetDispPSustainLevel (ch)->Set (SynthD.GetSustainLevel (ch));
            SynthD.GetDispPSustainTime  (ch)->Set (SynthD.GetSustainTime  (ch));
            SynthD.GetDispPReleaseTime  (ch)->Set (SynthD.GetReleaseTime  (ch));
            SynthD.GetDispPLimitLevel   (ch)->Set (SynthD.GetMaxLevel     (ch));
            }
            break;
        }
    }

//#######################################################################
GRPH_C  Graphics;

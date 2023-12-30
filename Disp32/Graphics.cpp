//#######################################################################
// Module:     Graphics.cpp
// Descrption: Graphics module
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
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
            TextY = _y + FADER_TEXT_Y;
            break;
        case CNTRL_TYPE_C::POT:
            TextY = _y + KNOB_TEXT_Y;
            pGFX->fillCircle (_x, _y, 2, _color);
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
                pGFX->fillArc (PosX, PosY, KNOB_DIAMETER, KNOB_DIAMETER + KNOB_WIDTH, KNOB_BASE_ANGLE, KNOB_FULL_ROTATION + KNOB_BASE_ANGLE, BACKGRND);
                break;
            default:
                break;
            }
        }
    }

//#######################################################################
void DISP_CTRL_C::ShowSawtooth (bool data)
    {
    pGFX->fillRect (PosX + 30, PosY - 30, PosX + 85, PosY + 18, BACKGRND);
    if ( data )
        {
        pGFX->drawLine (PosX + 30, PosY + 18, PosX + 30, PosY - 30, Color);
        pGFX->drawLine (PosX + 30, PosY - 30, PosX + 85, PosY + 18, Color);
        }
    else
        {
        pGFX->drawLine (PosX + 30, PosY + 18, PosX + 85, PosY - 30, Color);
        pGFX->drawLine (PosX + 85, PosY - 30, PosX + 85, PosY + 18, Color);
        }
    }

//#######################################################################
void DISP_CTRL_C::Set (float val)
    {
    int data;

    Active = true;
    val += 0.0003;
    switch ( Type )
        {
        case CNTRL_TYPE_C::FADER:
            // erase
            pGFX->fillRect (PosX - FADER_DEL_X, PosY - FADER_TOP, FADER_DEL_W, FADER_DEL_H, BACKGRND);
            // draw fader slot
            pGFX->drawRect (PosX - FADER_WIDTH_OFFSET, PosY - FADER_TOP, FADER_WIDTH, FADER_HEIGHT, KNOB_OFF_COLOR);
            pGFX->drawRect (PosX - (FADER_WIDTH_OFFSET - 1), PosY - (FADER_TOP - 1), FADER_WIDTH - 2, FADER_HEIGHT - 1, KNOB_OFF_COLOR);

            data = val * FADER_HEIGHT;
            // Fader filler
            pGFX->fillRect (PosX - 2, (PosY + FADER_TOP) - data, 4, data, Color);
            // fader knob
            pGFX->fillRect (PosX - 7, PosY + (FADER_TOP) - data, 14, 2, Color);
            data = (int)(val * Range);
            break;
        case CNTRL_TYPE_C::POT:
            // erase
            pGFX->fillArc (PosX, PosY, 0, KNOB_DIAMETER + 8, KNOB_BASE_ANGLE, KNOB_FULL_ROTATION + KNOB_BASE_ANGLE, BACKGRND);
            // Draw knob`
            pGFX->fillArc (PosX, PosY, KNOB_DIAMETER, KNOB_DIAMETER + KNOB_WIDTH, KNOB_BASE_ANGLE, KNOB_FULL_ROTATION + KNOB_BASE_ANGLE, KNOB_OFF_COLOR);
            // Value filler
            pGFX->fillArc (PosX, PosY, KNOB_DIAMETER, KNOB_DIAMETER + KNOB_WIDTH, KNOB_BASE_ANGLE, (val * KNOB_FULL_ROTATION) + KNOB_BASE_ANGLE, Color);
            // knob pointer
            pGFX->fillArc (PosX, PosY, 1, KNOB_DIAMETER + 8, (val * KNOB_FULL_ROTATION) + KNOB_BASE_ANGLE, (val * KNOB_FULL_ROTATION) + KNOB_BASE_ANGLE, Color);
            data = (int)(val * Range);
            break;
        default:
            break;
        }
    pGFX->setFont (KNOB_FONT);
    pGFX->fillRect (TextX, TextY-24, 56, 25, BACKGRND);
    pGFX->setCursor (TextX, TextY);
    pGFX->print (String (data));
    }

//#######################################################################
GRPH_C::GRPH_C ()
    {
    pinMode (GFX_BL, OUTPUT);
    }

//#######################################################################
void GRPH_C::Begin ()
    {
    pGFX->begin();
    pGFX->setUTF8Print (true);           // enable UTF8 support for the Arduino print() function
    digitalWrite (GFX_BL, HIGH);
    pGFX->fillScreen (BLUE);
    }

//#######################################################################
void GRPH_C::InitPageOsc1 ()
    {
    pGFX->fillScreen (BACKGRND);

    pGFX->setFont (LABEL_FONT);

    for ( int z = 0;  z < NUM_CNT_PAGE1;  z++ )
        {
        CONTROL_T& cnt = ControlPageOsc1[z];
        pGFX->setCursor (cnt.LabelX, 16);
        pGFX->print (cnt.Label);
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
                pGFX->setFont (LABEL_FONT);
                pGFX->setCursor (5, KNOB_OFFSET_Y + (z * KNOB_SPACING_Y) + 3);
                pGFX->print (WaveShapes[z]);
                for ( int zc = 0;  zc < NUM_CNT_PAGE1;  zc++ )
                    {
                    CONTROL_T& cta = ControlPageOsc1[0];
                    pd = new DISP_CTRL_C (cta.Type, cta.PosX, KNOB_OFFSET_Y + (z * KNOB_SPACING_Y), cta.Range, cta.Color);
                    SynthD.SetDispPDispAttackTime   (z, pd);
                    CONTROL_T& ctd = ControlPageOsc1[1];
                    pd = new DISP_CTRL_C (ctd.Type, ctd.PosX, KNOB_OFFSET_Y + (z * KNOB_SPACING_Y), ctd.Range, ctd.Color);
                    SynthD.SetDispPDispADecayTime   (z, pd);
                    CONTROL_T& ctsl = ControlPageOsc1[2];
                    pd = new DISP_CTRL_C (ctsl.Type, ctsl.PosX, KNOB_OFFSET_Y + (z * KNOB_SPACING_Y), ctsl.Range, ctsl.Color);
                    SynthD.SetDispPDispSustainLevel (z, pd);
                    CONTROL_T& cts = ControlPageOsc1[3];
                    pd = new DISP_CTRL_C (cts.Type, cts.PosX, KNOB_OFFSET_Y + (z * KNOB_SPACING_Y), cts.Range, cts.Color);
                    SynthD.SetDispPDispSustainTime  (z, pd);
                    CONTROL_T& ctr = ControlPageOsc1[4];
                    pd = new DISP_CTRL_C (ctr.Type, ctr.PosX, KNOB_OFFSET_Y + (z * KNOB_SPACING_Y), ctr.Range, ctr.Color);
                    SynthD.SetDispPDispReleaseTime  (z, pd);
                    CONTROL_T& ctm = ControlPageOsc1[5];
                    pd = new DISP_CTRL_C (ctm.Type, ctm.PosX, KNOB_OFFSET_Y + (z * KNOB_SPACING_Y), ctm.Range, ctm.Color);
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
            if ( ch == (byte)SHAPE_C::SAWTOOTH )
                SynthD.GetDispPLimitLevel(ch)->ShowSawtooth(SynthD.GetSawtoothDirection ());
            }
            break;
        }
    }

//#######################################################################
GRPH_C  Graphics;

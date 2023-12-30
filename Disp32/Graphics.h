//
//   Graphics.h
//
//  Graphics for control displays
//
//  author: markeby
//
#pragma once

//############################################
enum class CNTRL_TYPE_C: byte {
    NONE   = 0,
    FADER,
    POT,
    };

//####################################...########
class DISP_CTRL_C
    {
private:
    CNTRL_TYPE_C Type;
    int          PosX;
    int          PosY;
    int          TextY;
    int          TextX;
    float        Range;
    int          Color;
    bool         Active;

public:
         DISP_CTRL_C   (CNTRL_TYPE_C _t, int _x, int _y, float _range, int _color);
        ~DISP_CTRL_C   (void);
    void Set           (float val);
    void ShowSawtooth  (bool data);
    };

//############################################
class GRPH_C
    {
private:
    bool    Active;
    int     FullTime;

    void    InitPageOsc1    (void);

public:
            GRPH_C          (void);
    void    Begin           (void);
    void    ShowADSR        (byte wave);
    };

extern GRPH_C  Graphics;

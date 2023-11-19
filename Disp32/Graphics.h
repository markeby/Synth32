//
//   WebOTA.h
//
//  Web Over The Air update tool
//  to load the software.
//
//  author: markeby
//
#pragma once

#include <Adafruit_ILI9341.h>

class GRPH_C
    {
private:
    bool                Active;
    int                 FullTime;
    Adafruit_ILI9341*   pDisp;

    void    DrawGrid    (void);

public:
            GRPH_C      (void);
    void    Begin       (void);
    void    ShowADSR    (byte wave);
    };

extern GRPH_C  Graphics;

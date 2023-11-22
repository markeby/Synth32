//
//   WebOTA.h
//
//  Web Over The Air update tool
//  to load the software.
//
//  author: markeby
//
#pragma once

class GRPH_C
    {
private:
    bool                Active;
    int                 FullTime;

    void    DrawGrid    (void);
    void    DeleteADSR  (byte ch);

public:
            GRPH_C      (void);
    void    Begin       (void);
    void    ShowADSR    (byte wave);
    };

extern GRPH_C  Graphics;

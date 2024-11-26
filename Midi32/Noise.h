//#######################################################################
// Module:     SoftLFO.h
// Descrption: Sine wave processor
// Creator:    markeby
// Date:       7/05/2024
//#######################################################################
#pragma once
#include "config.h"

namespace NOISE_N
{
//#######################################################################
enum class COLOR
    {
    WHITE = 0,
    PINK,
    RED,
    BLUE
    };
}// end namespace NOISE_N



//#######################################################################
//#######################################################################
class NOISE_C
    {
private:
    int     DigitalBase;

    typedef struct
        {
        union
            {
            byte data[4];
            struct
                {
                bool    White;      // White noise selected
                bool    Pink;       // Pink noise selected
                bool    Red;        // Red noise selected
                bool    Blue;       // Blue noise selected
                };
            };
        } SELECT_T;

    SELECT_T    Duct[DUCT_NUM];

public:
            NOISE_C     (int dig_first);
           ~NOISE_C     (void)  {}
    void    Reset       (void);
    void    Select      (int duct, NOISE_N::COLOR color, bool select) { this->Select (duct, (int)color,select); }
    void    Select      (int duct, int color, bool select);

    };



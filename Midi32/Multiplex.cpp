//#######################################################################
// Module:     LFOusc.cpp
// Descrption: Multiplexer controls
// Creator:    markeby
// Date:       10/31/2024
//#######################################################################
#include <Arduino.h>

#include "../Common/SynthCommon.h"
#include "Multiplex.h"
#include "Debug.h"

using namespace MULT_N;

//#######################################################################
//#######################################################################
    MULTIPLEX_C::MULTIPLEX_C (int first_device)
    {
    uint16_t dev = first_device;

    for ( int z = 0;  z < SIZE_OSC_ALL;  z++, dev++)
        this->OscAll[z] = dev;
    this->Clear ();
    }

//#######################################################################
void MULTIPLEX_C::Update (uint16_t* pdev, uint16_t cnt, bool state)
    {
    for (int z = 0;  z < cnt;  z++)
        {
        I2cDevices.DigitalOut (pdev[z], state);
        }
    I2cDevices.UpdateDigital ();
    }

//#######################################################################
void MULTIPLEX_C::SetOn (MULT_SOURCE source, MULT_GROUP group, MULT_OUTPUT output)
    {
    uint16_t* pdev   = nullptr;
    int       cnt    = 0;
    int       offset = 0;

    switch ( source )
        {
        case MULT_SOURCE::OSC:
            switch ( group )
                {
                case MULT_GROUP::ALL:
                    cnt = SIZE_OSC_OUTPUT;
                    break;
                case MULT_GROUP::ONE:
                    cnt  = SIZE_OSC_OUTPUT / GROUP_COUNT_OSC;
                    break;
                case MULT_GROUP::TWO:
                    cnt  = SIZE_OSC_OUTPUT / GROUP_COUNT_OSC;
                    offset = cnt;
                    break;
                default:
                    break;
                }
            switch ( output )
                {
                case MULT_OUTPUT::DIRECT:
                    pdev = this->OscDirectDev;
                    break;
                case MULT_OUTPUT::FILTER3STATE:
                    pdev = this->OscFilterDev;
                    break;
                default:
                    break;
                }
            break;

        case MULT_SOURCE::FILTER3STATE:
            break;

        default:
            break;
        }

    if ( cnt > 0 )
        this->Update (&(pdev[offset]), cnt, true);
    }

//#######################################################################
void MULTIPLEX_C::Clear ()
    {
    this->Update (this->OscAll, SIZE_OSC_ALL, false);
    }



//#######################################################################
//#define DEBUG_SINE 1

#define DELTA_LVL   1
#define MAX_LVL     4095
#define MIN_LVL     0
#define CENTER_LVL  0
#define STRCHAN       56
#define NUMCHAN     2
//#######################################################################
//#######################################################################
inline void TestSine (void)
    {
#ifndef DEBUG_SINE
        return;
#else
#if !DEBUG_SINE
        return;
#endif //DEBUG_SINE
#endif //DEBUG_SINE

    static int  sn  = MIN_LVL;
    static bool dir = false;

    if ( dir )
        {
        sn -= DELTA_LVL;
        if ( sn > CENTER_LVL - MIN_LVL )
            for ( int z = 0;  z < NUMCHAN;  z++ )
                I2cDevices.D2Analog (STRCHAN + z, sn);
        else
            dir = dir ^ true;
        }
    else
        {
        sn += DELTA_LVL;
        if ( sn < CENTER_LVL + MAX_LVL )
            for ( int z = 0;  z < NUMCHAN;  z++ )
                I2cDevices.D2Analog (STRCHAN + z, sn);
        else
            dir = dir ^ true;
        }
    I2cDevices.UpdateAnalog ();
    }



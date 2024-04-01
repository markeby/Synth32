//#######################################################################
// Module:     I2Cdevices.ino
// Descrption: I2C bus devices controller
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include <Wire.h>

#include "config.h"
#include "Debug.h"

static const char* Label = "I2C";
#define DBGDA(args...) {if(DebugI2C){ DebugMsg(Label,'A',args);}}
#define DBGDIG(args...) {if(DebugI2C){ DebugMsg(Label,'D',args);}}

//#######################################################################
bool Error (void)
    {
    SystemError = true;
    }

//#######################################################################
I2C_INTERFACE_C::I2C_INTERFACE_C (I2C_LOCATION_T* ploc)
    {
    I2C_LOCATION_T* zploc = ploc;

    for ( BoardCount = 0;  zploc->Channel != -1;  BoardCount++, zploc++ );

    SystemFail     = false;
    pBoard         = new I2C_BOARD_T[BoardCount];
    DeviceCount    = 0;
    AnalogOutCount = 0;

    for ( int z = 0;  z < BoardCount;  z++ )
        {
        I2C_LOCATION_T& loc = ploc[z];
        if ( loc.NumberDtoA > MAX_ANALOG_PER_BOARD )
            {
            printf ("## ERROR: Allocating too many D to A on board #%d\n", z);
            Error ();
            }
        if ( loc.NumberAtoD > MAX_ANALOG_PER_BOARD )
            {
            printf ("## ERROR: Allocating too many A to D on board #%d\n", z);
            Error ();
            }
        pBoard[z].Board  = loc;
        pBoard[z].Valid  = false;
        AnalogOutCount  += loc.NumberDtoA;
        DeviceCount     += loc.NumberDtoA;
        DeviceCount     += loc.NumberAtoD;
        DeviceCount     += loc.NumberDigital;
        DigitalOutCount += loc.NumberDigital;
        }
    if ( !DeviceCount )
        {
        printf ("## ERROR: No devices allocated for I2C\n");
        Error ();
        return;      // FIXME tone error here
        }
    pDevice = new I2C_DEVICE_T[DeviceCount];

    int at_dev = 0;
    for ( int zb = 0;  zb < BoardCount;  zb++ )
        {
        I2C_BOARD_T& brd = pBoard[zb];
        if ( brd.Board.NumberDtoA )
            {
            brd.DataDtoA = 0;
            brd.LastDataDtoA = 1;
            for ( int zd = 0;  zd < brd.Board.NumberDtoA;  zd++, at_dev++ )
                {
                pDevice[at_dev].pBoard = &(pBoard[zb]);
                pDevice[at_dev].pDtoA  = &(brd.DtoA[zd]);
                }
            }

        if ( brd.Board.NumberDigital )
            {
            brd.DataDigital = 0;
            brd.LastDataDigital = 1;
            for ( int zd = 0;  zd < brd.Board.NumberDigital;  zd++, at_dev++ )
                {
                pDevice[at_dev].pBoard   = &(pBoard[zb]);
                pDevice[at_dev].pDigital = &(brd.DataDigital);
                pDevice[at_dev].Bit      = zd;
                }
            }

        brd.DataAtoD = 0;
        brd.LastDataAtoD = 0;
        if ( brd.Board.NumberAtoD )
            {
            for ( int zd = 0;  zd < brd.Board.NumberAtoD;  zd++, at_dev++ )
                {
                pDevice[at_dev].pBoard = &(pBoard[zb]);
                pDevice[at_dev].pAtoD  = &(brd.AtoD[zd]);
                }
            }
        }
    }

//#######################################################################
//I2C_INTERFACE_C::~I2C_INTERFACE_C ()
//    {
//    delete[] pBoard;
//    delete[] pDevice;
//    }

//#######################################################################
inline void I2C_INTERFACE_C::BusMux (uint8_t cluster, uint8_t slice)
    {
    Wire.beginTransmission (0x70 + cluster);    // TCA9548A address
    Wire.write (1 << slice);                    // send byte to select bus
    Wire.endTransmission();
    }

//#######################################################################
inline void I2C_INTERFACE_C::EndBusMux (uint8_t bus)
    {
    Wire.beginTransmission (0x70 + bus);    // TCA9548A address
    Wire.write (0);                         // send byte to deselect bus
    Wire.endTransmission();
    }

//#######################################################################
void I2C_INTERFACE_C::Init4728 (I2C_LOCATION_T &loc)
    {
    static uint16_t p = 0xA0;        // value for power down
    static uint16_t r = 0x80;        // value for Vref
    static uint16_t g = 0xC0;        // value for gain
    int rval;

    BusMux (loc.Cluster, loc.Slice);

    Wire.beginTransmission(loc.Channel);
    rval = Wire.write (p);
    rval = Wire.endTransmission (true);

    Wire.beginTransmission(loc.Channel);
    rval = Wire.write (r);
    rval = Wire.endTransmission (true);

    Wire.beginTransmission(loc.Channel);
    rval = Wire.write (g);
    rval = Wire.endTransmission (true);

    EndBusMux (loc.Cluster);
    }

//#######################################################################
bool I2C_INTERFACE_C::ValidateDevice (uint8_t board)
    {
    I2C_BOARD_T& brd = pBoard[board];
    brd.Valid = false;
    BusMux (brd.Board.Cluster, brd.Board.Slice);
    Wire.beginTransmission(brd.Board.Channel);
    if ( Wire.endTransmission (true) == 0 )
       brd.Valid = true;
    EndBusMux (brd.Board.Cluster);
    return (!brd.Valid);
    }

//#######################################################################
void I2C_INTERFACE_C::Write (I2C_LOCATION_T &loc, uint8_t* buff, uint8_t length)
    {
    int rval;
    BusMux (loc.Cluster, loc.Slice);
    Wire.beginTransmission(loc.Channel);
    rval = Wire.write (buff, length);
    rval = Wire.endTransmission (true);
    EndBusMux (loc.Cluster);
    }

//#######################################################################
void I2C_INTERFACE_C::Write4728 (I2C_BOARD_T& board)
    {
    uint8_t buf[8];
    I2C_LOCATION_T& loc =  board.Board;

    DBGDA ("%d:%d:%#3.3x  write  %#4.4d  %#4.4d  %#4.4d  %#4.4d", loc.Cluster, loc.Slice, loc.Channel, board.DtoA[0], board.DtoA[1], board.DtoA[2], board.DtoA[3]);

    buf[0] = board.ByteData[1];
    buf[1] = board.ByteData[0];
    buf[2] = board.ByteData[3];
    buf[3] = board.ByteData[2];
    buf[4] = board.ByteData[5];
    buf[5] = board.ByteData[4];
    buf[6] = board.ByteData[7];
    buf[7] = board.ByteData[6];
    Write (loc, buf, 8);
    board.LastDataDtoA = board.DataDtoA;
    }

//#######################################################################
void I2C_INTERFACE_C::Write8575  (I2C_BOARD_T& board)
    {
    I2C_LOCATION_T& loc =  board.Board;
    String str;

    for ( byte z;  z < 16;  z++ )
        str += ( ((board.BitWord >> z) & 1) ) ? " X" : " o";

    DBGDIG ("%d:%d:%#3.3x  write  %s", loc.Cluster, loc.Slice, loc.Channel, str.c_str ());

    Write (loc, board.ByteData, 2);
    board.LastDataDigital = board.DataDigital;
    }

//#######################################################################
void I2C_INTERFACE_C::Zero ()
    {
    for ( int z = 0;  z < BoardCount;  z++ )
        {
        I2C_BOARD_T& bd = pBoard[z];
        if ( bd.Valid )
            {
            if ( bd.Board.NumberDtoA == 4 )         // check for 4728 D/A
                {
                bd.DataDtoA = 0;
                Write4728 (bd);
                }
            if ( bd.Board.NumberDigital )           // Check for 8575 16 channel digital
                {
                bd.DataDigital = 0;
                Write8575 (bd);
                }
            }
        }
    }

//#######################################################################
int I2C_INTERFACE_C::Begin ()
    {
    bool failthis = false;
    int  ecount = 0;

    Wire.begin ();
    Wire.setClock (800000UL);           //400kHz
    for (int z = 0;  z < BoardCount;  z++ )
        {
        I2C_LOCATION_T& board = pBoard[z].Board;
        printf ("\t  >> Init: Cluster = %d  Slice = %d  Chan = 0x%X  %s    ", board.Cluster, board.Slice, board.Channel,  board.Name);
        if ( this->ValidateDevice (z) )
            {
            printf ("**** Failure to access cluster\n");
            ecount++;
            }
        else
            {
            Init4728 (board);
            printf ("Complete.\n");
            }
        }
    Zero ();
    return (ecount);
    }

//#######################################################################
void I2C_INTERFACE_C::D2Analog (uint16_t channel, int value)
    {
    I2C_DEVICE_T& dev = pDevice[channel];
    *(dev.pDtoA) = value;
    }

//#######################################################################
void I2C_INTERFACE_C::DigitalOut (byte device, bool value)
    {
    I2C_DEVICE_T& dev = pDevice[device];
    bitWrite (*(dev.pDigital) , dev.Bit, value);
    }

//#######################################################################
void I2C_INTERFACE_C::UpdateAnalog ()
    {
    for ( int z = 0;  z < BoardCount;  z++ )
        {
        I2C_BOARD_T& brd = pBoard[z];
        if ( brd.Valid && brd.Board.NumberDtoA )
            {
            if ( brd.DataDtoA != brd.LastDataDtoA )
                Write4728 (pBoard[z]);
            }
        }
    }

//#######################################################################
void I2C_INTERFACE_C::UpdateDigital ()
    {
    for ( int z = 0;  z < BoardCount;  z++ )
        {
        I2C_BOARD_T& brd = pBoard[z];
        if ( brd.Valid && brd.Board.NumberDigital )
            {
            if ( brd.DataDigital != brd.LastDataDigital )
                Write8575 (pBoard[z]);
            }
        }
    }
//#######################################################################
void I2C_INTERFACE_C::Error ()
    {
    SystemError = true;
    }


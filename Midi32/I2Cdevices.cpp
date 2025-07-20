//#######################################################################
// Module:     I2Cdevices.ino
// Descrption: I2C bus devices controller
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include <Wire.h>
#include <ADS1115-Driver.h>

#include "config.h"
#include "Debug.h"

#ifdef DEBUG_SYNTH
static const char* LabelDA = "I2C-DA";
static const char* LabelAD = "I2C-AD";
static const char* LabelDI = "I2C-DI";
static const char* LabelMX = "I2C-MUX";
#define DBGDA(args...) {if(DebugI2C){DebugMsg(LabelDA,DEBUG_NO_INDEX,args);}}
#define DBGAD(args...) {if(DebugI2C){DebugMsg(LabelAD,DEBUG_NO_INDEX,args);}}
#define DBGDIG(args...) {if(DebugI2C){DebugMsg(LabelDI,DEBUG_NO_INDEX,args);}}
#define DBGMUX(args...) {if(DebugI2C){DebugMsg(LabelMX,DEBUG_NO_INDEX,args);}}
#else
#define DBGDA(args...)
#define DBGAD(args...)
#define DBGDIG(args...)
#define DBGMUX(args...)
#endif
static const char* LabelError = "I2C";
#define ERROR(args...) {ErrorMsg (LabelError, __FUNCTION__, args);}
#define DBGERROR(args...) {if(DebugI2C){ErrorMsg (LabelError, __FUNCTION__, args);}}

//#######################################################################
//#######################################################################
I2C_INTERFACE_C::I2C_INTERFACE_C (I2C_CLUSTERS_T* pcluster, I2C_LOCATION_T* ploc)
    {
    I2C_LOCATION_T* zploc = ploc;

    for ( this->BoardCount = 0;  zploc->Port != 0xFF;  this->BoardCount++, zploc++ );
    this->CallbackAtoD   = nullptr;
    AtoD_loopDevice      = 0;
    SystemFail           = false;
    this->pBoard         = new I2C_BOARD_T[this->BoardCount];
    this->DeviceCount    = 0;
    this->AnalogOutCount = 0;
    this->pClusterList   = pcluster;

    for ( int z = 0;  z < this->BoardCount;  z++ )
        {
        I2C_LOCATION_T& loc = ploc[z];
        this->pBoard[z].Board  = loc;
        this->pBoard[z].Valid  = false;
        this->AnalogOutCount  += loc.NumberDtoA;
        this->DeviceCount     += loc.NumberDtoA;
        this->DeviceCount     += loc.NumberAtoD;
        this->DeviceCount     += loc.NumberDigital;
        this->DigitalOutCount += loc.NumberDigital;
        }

    if ( !this->DeviceCount )
        {
        this->Error ();
        return;      // FIXME tone error here
        }

    this->pDevice = new I2C_DEVICE_T[DeviceCount];

    int at_dev = 0;
    for ( int zb = 0;  zb < BoardCount;  zb++ )
        {
        I2C_BOARD_T& brd = this->pBoard[zb];

        if ( brd.Board.NumberDtoA )
            {
            brd.DataDtoA = 0;
            brd.LastDataDtoA = 1;
            for ( int zd = 0;  zd < brd.Board.NumberDtoA;  zd++, at_dev++ )
                {
                this->pDevice[at_dev].pBoard = &(pBoard[zb]);
                this->pDevice[at_dev].pDtoA  = &(brd.DtoA[zd]);
                }
            }

        if ( brd.Board.NumberDigital )
            {
            brd.DataDigital = 0;
            brd.LastDataDigital = 1;
            for ( int zd = 0;  zd < brd.Board.NumberDigital;  zd++, at_dev++ )
                {
                this->pDevice[at_dev].pBoard   = &(pBoard[zb]);
                this->pDevice[at_dev].pDigital = &(brd.DataDigital);
                this->pDevice[at_dev].Bit      = zd;
                }
            }

        if ( brd.Board.NumberAtoD )
            {
            brd.DataAtoD = 0;
            brd.LastDataAtoD = 1;
            for ( int zd = 0;  zd < brd.Board.NumberAtoD;  zd++, at_dev++ )
                {
                this->pDevice[at_dev].pBoard  = &(pBoard[zb]);
                this->pDevice[at_dev].pAtoD   = &(brd.AtoD[zd]);
                this->pDevice[at_dev].DtoAain = this->DecodeIndex1115 (zd);
                }
            }
        }
    }

//#######################################################################
char* I2C_INTERFACE_C::ErrorString (int err)
    {
    static char* e1 = "data too long to fit in transmit buffer";
    static char* e2 = "Received NACK on transmit of address";
    static char* e3 = "received NACK on transmit of data";
    static char* e4 =  "other error";
    static char* e5 = "timeout";

    switch ( err )
        {
        case 1:
            return (e1);
        case 2:
            return (e2);
        case 3:
            return (e3);
        case 5:
            return (e5);
        case 4:
            return (e4);
        default:
            break;
        }
    return (e4);
    }


//#######################################################################
void I2C_INTERFACE_C::BusMux (I2C_LOCATION_T& loc)
    {
    DBGMUX ("Selecting cluster %d with slice %d", loc.Cluster, loc.Slice);
    Wire.beginTransmission (0x70 + loc.Cluster);    // TCA9548A address
    Wire.write (1 << loc.Slice);                    // send byte to select bus
    this->LastEndT = Wire.endTransmission();
    if ( this->LastEndT )
        ERROR ("BusMux cluster %d select %d with error: %s", loc.Cluster, loc.Slice, this->ErrorString (this->LastEndT));
    }

//#######################################################################
void I2C_INTERFACE_C::EndBusMux (I2C_LOCATION_T& loc)
    {
    DBGMUX ("Deselecting cluster %d", loc.Cluster);
    Wire.beginTransmission (0x70 + loc.Cluster);    // TCA9548A address
    Wire.write (0);                                 // send byte to deselect bus
    this->LastEndT = Wire.endTransmission();
    if ( this->LastEndT )
        ERROR ("Ending cluster %d  slice: %d   error: %s", loc.Cluster, loc.Slice, this->ErrorString (this->LastEndT));
    }

//#######################################################################
void I2C_INTERFACE_C::WriteRegisterByte (uint8_t port, uint8_t data)
    {
    Wire.beginTransmission (port);
    Wire.write (data);
    this->LastEndT = Wire.endTransmission (true);
    if ( this->LastEndT )
        ERROR ("Port: %#02.2X   data: %#02.2X   error: %s", port, data, this->ErrorString (this->LastEndT));
    }

//#######################################################################
void I2C_INTERFACE_C::WriteRegister16 (uint8_t port, uint8_t addr, uint16_t data)
    {
    DBGAD ("Sending to port %#02.2x for addr %#02.2x with data %#04.4x", port, addr, data);
    Wire.beginTransmission (port);
    Wire.write (addr);
    Wire.write ((uint8_t)(data >> 8));
    Wire.write ((uint8_t)(data &  0xFF));
    this->LastEndT = Wire.endTransmission (true);
    if ( this->LastEndT )
        ERROR ("Result: %s   port: %#02.2x   addr: %#02.2x   data: %#04.4x", this->ErrorString (this->LastEndT), port, addr, data);
    }

//#######################################################################
uint16_t I2C_INTERFACE_C::ReadRegister16 (uint8_t port, uint8_t addr)
    {
    DBGAD ("Setup to read at port %#02.2x for addr %#02.2x", port, addr);
    Wire.beginTransmission (port);
    Wire.write (addr);
    this->LastEndT = Wire.endTransmission (true);
    if ( this->LastEndT )
        ERROR ("Cannot issue read request to port: %#02.2X   addr: %#02.2X   error: %s", port, addr, this->ErrorString (this->LastEndT));
    Wire.requestFrom(port, (uint8_t)2, true);
    if ( Wire.available () )
        {
        uint16_t data = Wire.read () << 8;
        data |= Wire.read ();
        DBGAD ("Success in read data %#04.4x", data);
        return (data);
        }
    DBGAD ("Failure to read data 0x0000");
    return (0);
    }

//#######################################################################
uint8_t I2C_INTERFACE_C::DecodeIndex1115 (uint8_t index)
    {
    uint8_t ain = 0;

    switch ( index )
        {
        case 1:
            ain = ADS1115_MUX_AIN1_GND;
            break;
        case 2:
            ain = ADS1115_MUX_AIN2_GND;
            break;
        case 3:
            ain = ADS1115_MUX_AIN3_GND;
            break;
        default:
            ain = ADS1115_MUX_AIN0_GND;
            break;
        }
    return (ain);
    }

//#######################################################################
void I2C_INTERFACE_C::Init1115 (I2C_LOCATION_T &loc)
    {
    this->BusMux (loc);
    this->WriteRegister16 (loc.Port, ADS1115_CONFIG_REG_ADDR, ADS1115_CONFIG_REG_DEF & ~(1 << ADS1115_OS_FLAG_POS));
    this->WriteRegister16 (loc.Port, ADS1115_LOW_TRESH_REG_ADDR, ADS1115_LOW_TRESH_REG_DEF);
    this->WriteRegister16 (loc.Port, ADS1115_HIGH_TRESH_REG_ADDR, ADS1115_HIGH_TRESH_REG_DEF);
    this->EndBusMux (loc);
    AtoD_loopDevice = 0;
    }

//#######################################################################
void I2C_INTERFACE_C::Start1115 (I2C_DEVICE_T& device)
    {
    I2C_LOCATION_T& loc = device.pBoard->Board;
    uint16_t val =
          (ADS1115_OS_START_SINGLE       << ADS1115_OS_FLAG_POS)        \
       |  (device.DtoAain                << ADS1115_MUX0_DAT_POS)       \
       |  (ADS1115_PGA_6_144             << ADS1115_PGA0_DAT_POS)       \
       |  (ADS1115_MODE_SINGLE           << ADS1115_MODE_FLAG_POS)      \
       |  (ADS1115_DR_128_SPS            << ADS1115_DR0_DAT_POS)        \
       |  (ADS1115_COMP_MODE_TRADITIONAL << ADS1115_COMP_MODE_FLAG_POS) \
       |  (ADS1115_COMP_POL_LOW          << ADS1115_COMP_POL_FLAG_POS)  \
       |  (ADS1115_COMP_LAT_NO_LATCH     << ADS1115_COMP_LAT_FLAG_POS)  \
       |  (ADS1115_COMP_QUE_DISABLE      << ADS1115_COMP_QUE0_DAT_POS);

    this->WriteRegister16 (loc.Port, ADS1115_CONFIG_REG_ADDR, val);
    }

//#######################################################################
void I2C_INTERFACE_C::Init4728 (I2C_LOCATION_T &loc)
    {
    static uint8_t p = 0xA0;        // value for power down
    static uint8_t r = 0x80;        // value for Vref
    static uint8_t g = 0xC0;        // value for gain
    static uint8_t d[8] = {0, 0, 0, 0, 0, 0, 0, 0 };

    this->BusMux (loc);
    if ( this->LastEndT )
        {
        ERROR ("Accessing cluster %d to enable slice %d   error: %s", loc.Cluster, loc.Slice, this->ErrorString (this->LastEndT));
        }

    this->WriteRegisterByte (loc.Port, p);
    this->WriteRegisterByte (loc.Port, r);
    this->WriteRegisterByte (loc.Port, g);

    Wire.beginTransmission (loc.Port);      // reset all D/A to zero
    Wire.write (d, 8);
    this->LastEndT = Wire.endTransmission (true);

    this->EndBusMux (loc);
    }

//#######################################################################
void I2C_INTERFACE_C::Init8575 (I2C_LOCATION_T &loc)
    {
    static uint8_t d[2] = {0, 0 };

    this->BusMux (loc);

    Wire.beginTransmission (loc.Port);      // reset all D/A to zero
    Wire.write (d, 2);
    this->LastEndT = Wire.endTransmission (true);

    this->EndBusMux (loc);
    }

//#######################################################################
bool I2C_INTERFACE_C::ValidateDevice (ushort board)
    {
    I2C_BOARD_T& brd = this->pBoard[board];
    brd.Valid = false;
    this->BusMux (brd.Board);
    Wire.beginTransmission(brd.Board.Port);

    this->LastEndT = Wire.endTransmission (true);
    if ( this->LastEndT == 0 )
        brd.Valid = true;
    else
        DBGERROR ("Validation error on port %#02.2X.  %s", brd.Board.Port, this->ErrorString (this->LastEndT));

    this->EndBusMux (brd.Board);
    return (!brd.Valid);
    }

//#######################################################################
void I2C_INTERFACE_C::Write (I2C_LOCATION_T &loc, uint8_t* buff, uint8_t length)
    {
    int rval;
    this->BusMux (loc);
    Wire.beginTransmission (loc.Port);
    Wire.write (buff, length);
    this->LastEndT = Wire.endTransmission (true);
    this->EndBusMux (loc);
    if ( this->LastEndT )
        {
        ERROR ("Result: %s   cluster: %d   slice: %d   port: 0x%#02.2X   buff[0]: 0x%#02.2X   length: %d", this->ErrorString (this->LastEndT), loc.Cluster, loc.Slice, loc.Port, *buff, length);
        }
    }

//#######################################################################
void I2C_INTERFACE_C::Write4728 (I2C_BOARD_T& board)
    {
    uint8_t buf[8];
    I2C_LOCATION_T& loc =  board.Board;

    DBGDA ("%d:%d:%#3.3x%c write  %#4.4d  %#4.4d  %#4.4d  %#4.4d  %s",
           loc.Cluster, loc.Slice, loc.Port,
           (( board.Valid ) ? ' ' : '-'),
           board.DtoA[0], board.DtoA[1], board.DtoA[2], board.DtoA[3], loc.Name);

    buf[0] = board.ByteData[1];
    buf[1] = board.ByteData[0];
    buf[2] = board.ByteData[3];
    buf[3] = board.ByteData[2];
    buf[4] = board.ByteData[5];
    buf[5] = board.ByteData[4];
    buf[6] = board.ByteData[7];
    buf[7] = board.ByteData[6];
    if ( board.Valid )
        this->Write (loc, buf, 8);
    board.LastDataDtoA = board.DataDtoA;
    }

//#######################################################################
void I2C_INTERFACE_C::Write857x (I2C_BOARD_T& board)
    {
    I2C_LOCATION_T& loc =  board.Board;

#ifdef DEBUG_SYNTH
    String str;

    if ( DebugI2C )
        {
        for (uint8_t z;  z < board.Board.NumberDigital;  z++)
            str += ( ((board.BitWord >> z) & 1) ) ? " 1" : " 0";
        }
#endif
    DBGDIG ("%d:%d:%#3.3x%c write %s  %s",
            loc.Cluster, loc.Slice, loc.Port,
            (( board.Valid ) ? ' ' : '-'),
            str.c_str (),
            loc.Name);

    if ( board.Board.NumberDigital == 8 )       // if device is a 8574
        board.ByteData[1] = board.ByteData[0];
    if ( board.Valid )
        this->Write(loc, board.ByteData, 2);
    board.LastDataDigital = board.DataDigital;
    }

//#######################################################################
// return:  0 = all good
//         -1 = Total failure
//         +X = Some interface errors
int I2C_INTERFACE_C::Begin ()
    {
    String str;

    Wire.begin ();
    Wire.setClock (800000UL);       // clock for High-speed to Ultra-fast mode
//    Wire.setClock (400000UL);     // clock for Fast mode

    uint8_t err = 0;
    for ( I2C_CLUSTERS_T* pc = this->pClusterList;  *pc != -1;  pc++ )
        {
        Wire.beginTransmission (0x70 + *pc);    // TCA9548A address
        Wire.write (0);                        // send byte to select bus
        this->LastEndT = Wire.endTransmission();
        if ( this->LastEndT )
            DBGMUX ("Return for cluster %d is %s", *pc, this->ErrorString (this->LastEndT));
        if ( this->LastEndT > err )
            err = this->LastEndT;
        }
    if ( err > 0 )
        {
        printf ("\n  ### Cluster access error \"%s\".", this->ErrorString (err));
        return (-1);
        }

    int  ecount = 0;
    for (int z = 0;  z < this->BoardCount;  z++)
        {
        I2C_LOCATION_T& board = this->pBoard[z].Board;
        if ( DebugI2C )
            printf("\t  >> Init: Cluster %d  Slice %d  Port 0x%X  %s    ", board.Cluster, board.Slice, board.Port,  board.Name);
        if ( this->ValidateDevice (z) )
            {
            printf ("\t****\tFailure to access I2C cluster %d  Slice %d  port %X  \"%s\"\n",  board.Cluster, board.Slice, board.Port, board.Name);
            ecount++;
            }
        else
            {
            if ( board.NumberDtoA == 4 )
                this->Init4728 (board);
            if ( board.NumberAtoD == 4 )
                this->Init1115 (board);
            if ( board.NumberDigital )
                this->Init8575 (board);
            if ( DebugI2C )
                printf ("Complete.\n");
            }
        }
    return (ecount);
    }

//#######################################################################
void I2C_INTERFACE_C::D2Analog (short device, ushort value)
    {
    I2C_DEVICE_T& dev = pDevice[device];
    if ( dev.pBoard->Valid )
        *(dev.pDtoA) = value;
    }

//#######################################################################
void I2C_INTERFACE_C::DigitalOut (short device, bool value)
    {
    I2C_DEVICE_T& dev = pDevice[device];
    if ( dev.pBoard->Valid )
        bitWrite(*(dev.pDigital), dev.Bit, value);
    }

//#######################################################################
void I2C_INTERFACE_C::UpdateAnalog ()
    {
    for ( int z = 0;  z < BoardCount;  z++ )
        {
        I2C_BOARD_T& brd = pBoard[z];
        if ( brd.Board.NumberDtoA && (brd.DataDtoA != brd.LastDataDtoA) )
            Write4728 (pBoard[z]);
        }
    }

//#######################################################################
void I2C_INTERFACE_C::UpdateDigital ()
    {
    for ( int z = 0;  z < BoardCount;  z++ )
        {
        I2C_BOARD_T& brd = pBoard[z];
        if ( brd.Board.NumberDigital && (brd.DataDigital != brd.LastDataDigital) )
            this->Write857x (pBoard[z]);
        }
    }

//#######################################################################
void I2C_INTERFACE_C::StartAtoD (short device)
    {
    I2C_DEVICE_T& dev = pDevice[device];
    I2C_LOCATION_T& loc = dev.pBoard->Board;

    this->BusMux (loc);
    this->Start1115 (dev);
    this->EndBusMux (loc);
    this->AtoD_loopDevice = device;
    }

//#######################################################################
void I2C_INTERFACE_C::Error ()
    {
    SystemError = true;
    }

//#######################################################################
void I2C_INTERFACE_C::Loop ()
    {
    int16_t val;

    if ( this->AtoD_loopDevice > 0 )
        {
        I2C_LOCATION_T& loc = this->pDevice[this->AtoD_loopDevice].pBoard->Board;

        this->BusMux (loc);
        val = this->ReadRegister16 (loc.Port, ADS1115_CONFIG_REG_ADDR);
        if ( val & (1 << ADS1115_OS_FLAG_POS) )
            {
            val = this->ReadRegister16 (loc.Port, ADS1115_CONVERSION_REG_ADDR);
            this->CallbackAtoD (val);
            }
        this->EndBusMux (loc);
        }
    }


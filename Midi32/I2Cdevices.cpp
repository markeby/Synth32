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
#define DBGDA(args...) {if(DebugI2C){ DebugMsg(LabelDA,DEBUG_NO_INDEX,args);}}
#define DBGAD(args...) {if(DebugI2C){ DebugMsg(LabelAD,DEBUG_NO_INDEX,args);}}
#define DBGDIG(args...) {if(DebugI2C){ DebugMsg(LabelDI,DEBUG_NO_INDEX,args);}}
#else
#define DBGDA(args...)
#define DBGAD(args...)
#define DBGDIG(args...)
#endif
static const char* LabelError = "I2C";
#define ERROR(args...) {ErrorMsg (LabelError, __FUNCTION__, args);}

//#######################################################################
I2C_INTERFACE_C::I2C_INTERFACE_C (I2C_LOCATION_T* ploc)
    {
    I2C_LOCATION_T* zploc = ploc;

    Serial.begin (115200);
    for ( this->BoardCount = 0;  zploc->Port != 0xFF;  this->BoardCount++, zploc++ );
    AtoD_Valid           = false;
    SystemFail           = false;
    this->pBoard         = new I2C_BOARD_T[this->BoardCount];
    this->DeviceCount    = 0;
    this->AnalogOutCount = 0;

    for ( int z = 0;  z < this->BoardCount;  z++ )
        {
        I2C_LOCATION_T& loc = ploc[z];
        if ( loc.NumberDtoA > MAX_ANALOG_PER_BOARD )
            {
            printf ("## ERROR: Allocating too many D to A on board #%d\n", z);
            this->Error ();
            }
        if ( loc.NumberAtoD > MAX_ANALOG_PER_BOARD )
            {
            printf ("## ERROR: Allocating too many A to D on board #%d\n", z);
            this->Error ();
            }
        this->pBoard[z].Board  = loc;
        this->pBoard[z].Valid  = false;
        this->AnalogOutCount  += loc.NumberDtoA;
        this->DeviceCount     += loc.NumberDtoA;
        this->DeviceCount     += loc.NumberAtoD;
        this->DeviceCount     += loc.NumberDigital;
        this->DigitalOutCount += loc.NumberDigital;
        }
    if ( !DeviceCount )
        {
        printf ("## ERROR: No devices allocated for I2C\n");
        this->Error ();
        return;      // FIXME tone error here
        }

    pDevice = new I2C_DEVICE_T[DeviceCount];

    int at_dev = 0;
    for ( int zb = 0;  zb < BoardCount;  zb++ )
        {
        I2C_BOARD_T& brd = this->pBoard[zb];

        if ( brd.Board.NumberDtoA )
            {
            DBGDA ("Starting at %d", at_dev);
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
            DBGDIG ("Starting at %d", at_dev);
            brd.DataDigital = 0;
            brd.LastDataDigital = 1;
            for ( int zd = 0;  zd < brd.Board.NumberDigital;  zd++, at_dev++ )
                {
                pDevice[at_dev].pBoard   = &(pBoard[zb]);
                pDevice[at_dev].pDigital = &(brd.DataDigital);
                pDevice[at_dev].Bit      = zd;
                }
            }

        if ( brd.Board.NumberAtoD )
            {
            brd.DataAtoD = 0;
            brd.LastDataAtoD = 1;
            DBGAD ("Starting at %d", at_dev);
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
void I2C_INTERFACE_C::BusMux (I2C_LOCATION_T& loc)
    {
    Wire.beginTransmission (0x70 + loc.Cluster);    // TCA9548A address
    Wire.write (1 << loc.Slice);                    // send byte to select bus
    Wire.endTransmission();
    }

//#######################################################################
void I2C_INTERFACE_C::EndBusMux (I2C_LOCATION_T& loc)
    {
    Wire.beginTransmission (0x70 + loc.Cluster);    // TCA9548A address
    Wire.write (0);                                 // send byte to deselect bus
    Wire.endTransmission();
    }

//#######################################################################
void I2C_INTERFACE_C::WriteRegisterByte (uint8_t port, uint8_t data)
    {
    Wire.beginTransmission (port);
    Wire.write (data);
    uint8_t rc = Wire.endTransmission (true);
    if ( rc )
        ERROR ("port: %#02.2X   data: %#02.2X", port, data);
    }

//#######################################################################
void I2C_INTERFACE_C::WriteRegister16 (uint8_t port, uint8_t addr, uint16_t data)
    {
    Wire.beginTransmission (port);
    Wire.write (addr);
    Wire.write ((uint8_t)(data >> 8));
    Wire.write ((uint8_t)(data &  0xFF));
    uint8_t rc = Wire.endTransmission (true);
    if ( rc )
        ERROR ("port: 0x%#02.2X   addr: %#02.2X   data: %#04.4X", port, addr, data);
    }

//#######################################################################
uint16_t I2C_INTERFACE_C::ReadRegister16 (uint8_t port, uint8_t addr)
    {
    Wire.beginTransmission (port);
    Wire.write (addr);
    uint8_t rc = Wire.endTransmission (true);
    if ( rc )
        ERROR ("Cannot issue read request to port: %#02.2X   addr: %#02.2X   error: %d", port, addr, rc);
    Wire.requestFrom(port, (uint8_t)2, true);
    if ( Wire.available () )
        {
        uint16_t data = Wire.read () << 8;
        data |= Wire.read ();
        return (data);
        }
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
    AtoD_Valid = false;
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
       |  (ADS1115_DR_8_SPS              << ADS1115_DR0_DAT_POS)        \
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

    this->BusMux (loc);
    this->WriteRegisterByte (loc.Port, p);
    this->WriteRegisterByte (loc.Port, r);
    this->WriteRegisterByte (loc.Port, g);
    this->EndBusMux (loc);
    }

//#######################################################################
bool I2C_INTERFACE_C::ValidateDevice (uint8_t board)
    {
    I2C_BOARD_T& brd = this->pBoard[board];
    brd.Valid = false;
    this->BusMux (brd.Board);
    Wire.beginTransmission(brd.Board.Port);
    if ( Wire.endTransmission (true) == 0 )
       brd.Valid = true;
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
    uint8_t rc = Wire.endTransmission (true);
    this->EndBusMux (loc);
    if ( rc )
        ERROR ("port: 0x%#02.2X   buff[0]: 0x%#02.2X   length: %d", loc.Port, *buff, length);
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
void I2C_INTERFACE_C::Zero ()
    {
    for ( int z = 0;  z < BoardCount;  z++ )
        {
        I2C_BOARD_T& bd = this->pBoard[z];
        if ( bd.Board.NumberDtoA == 4 )         // check for 4728 D/A
            {
            bd.DataDtoA = 0;
            this->Write4728 (bd);
            }
        if ( bd.Board.NumberDigital )           // Check for digital 8575 16 channel or 8574 8 channel
            {
            bd.DataDigital = 0;
            this->Write857x (bd);
            }
        }
    }

//#######################################################################
int I2C_INTERFACE_C::Begin ()
    {
    bool failthis = false;
    int  ecount = 0;

    Wire.begin ();
    Wire.setClock (800000UL);           //clock at 800kHz
    for (int z = 0;  z < BoardCount;  z++ )
        {
        I2C_LOCATION_T& board = pBoard[z].Board;
        if ( DebugI2C )
            printf("\t  >> Init: Cluster = %d  Slice = %d  Port = 0x%X  %s    ", board.Cluster, board.Slice, board.Port,  board.Name);
        if ( this->ValidateDevice (z) )
            {
            printf ("**** Failure to access I2C cluster %d\n",  board.Cluster);
            ecount++;
            }
        else
            {
            if ( board.NumberDtoA == 4 )
                {
                Init4728 (board);
                this->Zero ();
                }
            if ( board.NumberAtoD == 4 )
                Init1115 (board);
            if ( board.NumberDigital )
                this->Zero ();
            if ( DebugI2C )
                printf ("Complete.\n");
            }
        }
    return (ecount);
    }

//#######################################################################
void I2C_INTERFACE_C::D2Analog (uint16_t channel, int value)
    {
    I2C_DEVICE_T& dev = pDevice[channel];
    *(dev.pDtoA) = value;
    }

//#######################################################################
void I2C_INTERFACE_C::DigitalOut (uint16_t device, bool value)
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
void I2C_INTERFACE_C::StartAnalog ()
    {
    I2C_LOCATION_T& loc = pDevice[START_A_D].pBoard->Board;

    this->BusMux (loc);
    this->Start1115 (pDevice[START_A_D]);
    this->EndBusMux (loc);
    this->AtoD_Valid = true;
    this->AtoD_loop = START_A_D;
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
    float   fval;

    if ( this->AtoD_Valid )
        {
        I2C_LOCATION_T& loc = this->pDevice[AtoD_loop].pBoard->Board;

        this->BusMux (loc);
        val = this->ReadRegister16 (loc.Port, ADS1115_CONFIG_REG_ADDR);
        if ( val & (1 << ADS1115_OS_FLAG_POS) )
            {
            val = this->ReadRegister16 (loc.Port, ADS1115_CONVERSION_REG_ADDR);
            fval =  0.0001875 * (float)val;
            printf ("@@ 0x%04.4X  %f\n", val, fval);
            this->Start1115 (this->pDevice[START_A_D]);
            }
        this->EndBusMux (loc);
        }
    }


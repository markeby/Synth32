#pragma once

#define MAX_ANALOG_PER_BOARD  4

//#######################################################################
typedef struct
    {
    int8_t      Cluster;        // Mux chip address
    int8_t      Slice;          // Mux output select
    int8_t      Channel;        // I2C address of device
    int8_t      NumberDtoA;     // Device count on channel: 1 = MCP4725, 4 = MCP4728
    int8_t      NumberAtoD;
    int8_t      NumberDigital;  // number of digital I/O channeld: 8 / 16
    const char* Name;
    } I2C_LOCATION_T;

//#######################################################################
class I2C_INTERFACE_C
    {
private:
    typedef struct
        {
        I2C_LOCATION_T  Board;              // This board access info
        bool            Valid;              // This board is valid
        union
            {
            uint64_t        LastDataDtoA;
            uint64_t        LastDataAtoD;
            uint64_t        LastDataDigital;
            };
        union
            {
            union
                {
                uint8_t        ByteData[MAX_ANALOG_PER_BOARD * 2];
                uint16_t    BitWord;
                };
            uint16_t    DtoA[MAX_ANALOG_PER_BOARD];
            uint64_t    DataDtoA;
            uint16_t    AtoD[MAX_ANALOG_PER_BOARD];
            uint64_t    DataAtoD;
            uint16_t    DataDigital;
            };
        } I2C_BOARD_T;
    typedef struct
        {
        I2C_BOARD_T*    pBoard;
        uint16_t*       pDtoA;
        uint16_t*       pAtoD;
        uint16_t*       pDigital;
        uint8_t            Bit;
        } I2C_DEVICE_T;

    I2C_BOARD_T*    pBoard;
    I2C_DEVICE_T*   pDevice;
    int             DeviceCount;
    int             AnalogOutCount;
    int             DigitalOutCount;
    int             BoardCount;
    bool            SystemFail;

    void BusMux         (uint8_t cluster, uint8_t channel);
    void EndBusMux      (uint8_t cluster);
    void Init4728       (I2C_LOCATION_T &loc);
    void Write4728      (I2C_BOARD_T& board);
    void Write8575      (I2C_BOARD_T& board);
    void Write          (I2C_LOCATION_T& loc, uint8_t* buff, uint8_t length);
    bool ValidateDevice (uint8_t board);

public:
         I2C_INTERFACE_C (I2C_LOCATION_T* ploc);
//        ~I2C_INTERFACE_C (void);
    int  Begin           (void);
    void Zero            (void);
    void D2Analog        (uint16_t Channel, int value);
    void DigitalOut      (uint8_t device, bool value);
    void AnalogClear     (void);
    void UpdateAnalog    (void);
    void UpdateDigital   (void);
    void Error           (void);
    int  NumBoards       (void)    { return (BoardCount); }
    int  NumAnalog       (void)    { return (AnalogOutCount); }

    inline bool IsChannelValid (uint8_t device)
        {
        if ( device < DeviceCount && pDevice[device].pBoard->Valid )
            return (true);
        return (false);
        }
    };

#pragma once


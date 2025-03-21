#pragma once

#define MAX_ANALOG_PER_BOARD  4
#define START_A_D             144

//#######################################################################
typedef struct
    {
    uint8_t      Cluster;        // Mux chip address
    uint8_t      Slice;          // Mux output select
    uint8_t      Port;           // I2C address of device
    uint8_t      NumberDtoA;     // Device count on channel: 1 = MCP4725, 4 = MCP4728
    uint8_t      NumberAtoD;
    uint8_t      NumberDigital;  // number of digital I/O channeld: 8 / 16
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
            uint64_t    LastDataDtoA;
            uint64_t    LastDataAtoD;
            uint64_t    LastDataDigital;
            };
        union
            {
            union
                {
                uint8_t     ByteData[MAX_ANALOG_PER_BOARD * 2];
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
        uint8_t         DtoAain;
        uint8_t         Bit;
        } I2C_DEVICE_T;

    I2C_BOARD_T*    pBoard;
    I2C_DEVICE_T*   pDevice;
    byte            TempData[8];
    int             DeviceCount;
    int             AnalogOutCount;
    int             DigitalOutCount;
    int             BoardCount;
    bool            SystemFail;
    bool            AtoD_Valid;
    int16_t         AtoD_loop;

    void     BusMux             (I2C_LOCATION_T& loc);
    void     EndBusMux          (I2C_LOCATION_T& loc);

    void     WriteRegisterByte  (uint8_t port, uint8_t data);
    void     WriteRegister16    (uint8_t port, uint8_t addr, uint16_t data);
    uint16_t ReadRegister16     (uint8_t port, uint8_t addr);
    uint8_t  DecodeIndex1115    (uint8_t index);
    void     Init1115           (I2C_LOCATION_T &loc);
    void     Start1115          (I2C_DEVICE_T& device);
    void     Init4728           (I2C_LOCATION_T &loc);
    void     Write4728          (I2C_BOARD_T& board);
    void     Write857x          (I2C_BOARD_T& board);
    void     Write              (I2C_LOCATION_T& loc, uint8_t* buff, uint8_t length);
    bool     ValidateDevice     (uint8_t board);

public:
         I2C_INTERFACE_C (I2C_LOCATION_T* ploc);
//        ~I2C_INTERFACE_C  (void);
    int  Begin              (void);
    void Zero               (void);
    void Loop               (void);
    void D2Analog           (uint16_t Port, int value);
    void DigitalOut         (uint16_t device, bool value);
    void StartAnalog        (void);
    void AnalogClear        (void);
    void UpdateAnalog       (void);
    void UpdateDigital      (void);
    void Error              (void);

    //#######################################################################
    inline void ResetAnalog (void)
            { this->Init1115 (pDevice[START_A_D].pBoard->Board); }

    //#######################################################################
    inline bool ValidateAtoD (void)
        { return (this->AtoD_Valid); }

    //#######################################################################
    inline int  NumBoards (void)
        { return (this->BoardCount); }

    //#######################################################################
    inline int  NumAnalog (void)
        { return (this->AnalogOutCount); }

    //#######################################################################
    inline bool IsPortValid (uint8_t device)
        {
        if ( device < this->DeviceCount && this->pDevice[device].pBoard->Valid )
            return (true);
        return (false);
        }
    };

#pragma once


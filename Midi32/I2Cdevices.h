#pragma once

#define MAX_ANALOG_PER_BOARD  8

//#######################################################################
typedef int I2C_CLUSTERS_T;

typedef struct
    {
    int         Cluster;        // Mux chip address
    int         Slice;          // Mux output select
    int         Port;           // I2C address of device
    int         NumberDtoA;     // Device count on channel: 1 = MCP4725, 4 = MCP4728
    int         NumberAtoD;
    int         NumberDigital;  // number of digital I/O channeld: 8 / 16
    const char* Name;
    } I2C_LOCATION_T;

using CallbackUShort = void (*)(ushort val);

//#######################################################################
class I2C_INTERFACE_C
    {
private:
    typedef struct
        {
        I2C_LOCATION_T  Board;              // This board access info
        bool            Valid;              // This board is valid
        uint16_t        NewDataMask;        // bits that represent data updates
        union
            {
            union
                {
                uint8_t     ByteData[MAX_ANALOG_PER_BOARD * 2];
                uint16_t    BitWord;
                };
            uint16_t    DtoA[MAX_ANALOG_PER_BOARD];
            uint64_t    DataDtoA[2];
            uint16_t    AtoD[MAX_ANALOG_PER_BOARD / 2];
            uint64_t    DataAtoD;
            uint16_t    DataDigital;
            };
        } I2C_BOARD_T;
    typedef struct I2C_DEVICE_S
        {
        I2C_BOARD_T*    pBoard;
        uint16_t*       pDtoA;
        uint16_t*       pDigital;
        int             DevIndex;
        uint16_t*       pAtoD;
        uint8_t         DtoAain;
            I2C_DEVICE_S (void) : pBoard(nullptr),
                                  pDtoA(nullptr),
                                  pAtoD(nullptr),
                                  pDigital(nullptr),
                                  DevIndex(0)
                {}
        } I2C_DEVICE_T;

    I2C_BOARD_T*    _pBoard;
    I2C_DEVICE_T*   _pDevice;
    int             _DeviceCount;
    int             _BoardCount;
    ushort          _AtoD_loopDevice;
    CallbackUShort  _CallbackAtoD;
    I2C_CLUSTERS_T* _pClusterList;
    uint8_t         _LastEndT;
    bool            _DebugI2C;


    char*    ErrorString        (int err);
    void     BusMux             (I2C_LOCATION_T& loc);
    void     EndBusMux          (I2C_LOCATION_T& loc);

    void     Write              (I2C_LOCATION_T& loc, uint8_t* buff, uint8_t length);
    void     WriteRegisterByte  (uint8_t port, uint8_t data);
    void     WriteRegister16    (uint8_t port, uint8_t addr, uint16_t data);
    uint16_t ReadRegister16     (uint8_t port, uint8_t addr);
    void     Init47FEB28        (I2C_LOCATION_T &loc);
    void     Init4728           (I2C_LOCATION_T &loc);
    void     Init8575           (I2C_LOCATION_T &loc);
    void     Write47FEB28       (I2C_BOARD_T& board);
    void     Write4728          (I2C_BOARD_T& board);
    void     Write857x          (I2C_BOARD_T& board);
    uint8_t  DecodeIndex1115    (uint8_t index);
    void     Init1115           (I2C_LOCATION_T &loc);
    void     Start1115          (I2C_DEVICE_T& device);
    bool     ValidateDevice     (ushort board);

public:
         I2C_INTERFACE_C (I2C_CLUSTERS_T* pcluster, I2C_LOCATION_T* ploc);
         // return:  0 = all good
         //         -1 = Total failure
         //         +X = Some interface errors
    int  Begin              (void);
    bool IsPortValid        (short device);
    void Loop               (void);
    bool IsAnalogIn         (short device);
    bool IsAnalogOut        (short device);
    bool IsDigitalOut       (short device);
    void D2Analog           (short device, ushort value);
    void DigitalOut         (short device, bool value);
    void StartAtoD          (short device);
    void AnalogClear        (void);
    void Update             (void);
    void SetDebug           (bool state)
        { _DebugI2C = state; }

    //#######################################################################
    void ResetAnalog (short device)
        { this->Init1115(_pDevice[device].pBoard->Board); }

    //#######################################################################
    int  NumBoards (void)
        { return (this->_BoardCount); }

    //#######################################################################
    int GetDeviceCount (void)
        { return (this->_DeviceCount); }

    //#######################################################################
    void SetCallbackAtoD (CallbackUShort fptr)
        { _CallbackAtoD = fptr; }

    };

#pragma once


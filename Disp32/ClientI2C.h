//#######################################################################
// Module:     ClientI2C.h
// Descrption: Synthesizer Display I2C Client interface
// Creator:    markeby
// Date:       7/20/2024
//#######################################################################
#pragma once

#define LISTEN_ADDRESS  77

//#################################################
//    Display I2C Client interface
//#################################################
class   ClientI2C
    {
private:
    uint8_t MessageBuffer[1024];

public:
          ClientI2C         (void);                     // Initialize internal paramters
    void  Begin             (uint8_t device_addr);      // Set listening addresss and start listening
    void  Fetch             (void);                     // Called only on wire data present
    };

//#################################################
extern ClientI2C Listener;    // Message listener


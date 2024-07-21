//#######################################################################
// Module:     SyntFront.cpp
// Descrption: Synthesizer front end controller
//                 Midi->Front->SynthModules
// Creator:    markeby
// Date:       5/17/2023
//#######################################################################
#include <Arduino.h>

#include "config.h"
#include "Wire.h"
#include "ClientI2C.h"

#define I2C_DEV_ADDR 0x55

uint32_t i = 0;


static void onRequest ()
    {
    Wire.print (i++);
    Wire.print (" Packets.");
    }

static void onReceive(int len)
    {
    Listener.Fetch ();
    }







//#######################################################################
 ClientI2C::ClientI2C ()
    {
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
    }

//#####################################################################
void ClientI2C::Begin (uint8_t device_addr)
    {
    Wire.begin (device_addr);
    }

//#######################################################################
void ClientI2C::Fetch ()
    {
    int count = 0;

    while ( Wire.available () )
        {
        MessageBuffer[count++] = Wire.read ();
        }

    printf("Message in = ");

    for ( int z = 0;  z < count;  z++ )
        {
        printf("%#02X ", MessageBuffer[z]);
        }
    printf("\n");
    }


ClientI2C Listener;

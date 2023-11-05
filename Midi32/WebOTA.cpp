//#######################################################################
// Module:     WebOTA.ino
// Descrption: Web over the air software update
// Creator:    markeby
// Date:       9/4/2023
//#######################################################################
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include "WebOTA.h"

AsyncWebServer Server (80);

//#######################################################################
bool OTA_C::WaitWiFi (void)
    {
    if ( WiFi.status () == WL_CONNECTED )
        {
        IPaddressString = WiFi.localIP ().toString ();
        printf ("\t>>> OTA available. Use http:// %s\n", IPaddressString);
        WiFi_On = true;
        return (true);
        }
    return (false);
    }

//#######################################################################
void OTA_C::Setup (const char* pssid, const char* ppasswd)
    {
    WiFi_On = false;
    printf ("\t>>> Connecting to: %s\n", pssid);
    WiFi.begin (pssid, ppasswd);
    }

//#######################################################################
void OTA_C::Begin (void)
    {
    AsyncElegantOTA.begin (&Server);       // Start web updator
    Server.begin ();
    }

OTA_C UpdateOta;


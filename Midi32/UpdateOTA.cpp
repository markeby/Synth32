//#######################################################################
// Module:     WebOTA.cpp
// Descrption: Web over the air software update
// Creator:    markeby
// Date:       7/11/2024
//#######################################################################
#include <WiFi.h>
#include <ESPmDNS.h>
#include <NetworkUdp.h>
#include <ArduinoOTA.h>

#include "UpdateOTA.h"

//#######################################################################
void OTA_C::WaitWiFi ()
    {
    if ( WiFi.status () == WL_CONNECTED )
        {
        IPaddressString = WiFi.localIP ().toString ();
        printf ("\t>>> OTA available.  IP: %s\n", IPaddressString);
        WiFi_On = true;

        ArduinoOTA
            .onStart([] ()
                {
                String type;
                if ( ArduinoOTA.getCommand () == U_FLASH )
                    type = "pgm";
                else  // U_SPIFFS
                    type = "file";

                // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                printf  ("\n\nStart updating %s\n", type.c_str ());
                })
            .onEnd([] ()
                {
                printf ("\nEnd\n");
                })
            .onProgress([] (unsigned int progress, unsigned int total)
                {
                static byte z = 0;
                if ( ++z & 1 )
                    printf ("Progress: %u%%\r", (progress / (total / 100)));
                })
            .onError([] (ota_error_t error)
                {
                printf (" Error[%u]: ", error);
                if ( error == OTA_AUTH_ERROR )
                    printf (" Auth Failed");
                else if ( error == OTA_BEGIN_ERROR )
                    printf (" Begin Failed");
                else if ( error == OTA_CONNECT_ERROR )
                    printf (" Connect Failed");
                else if ( error == OTA_RECEIVE_ERROR )
                    printf (" Receive Failed");
                else if ( error == OTA_END_ERROR )
                    printf (" End Failed");
                });

        ArduinoOTA.begin ();
        }
    }

//#######################################################################
void OTA_C::Setup (const char* pssid, const char* ppasswd)
    {
    printf ("\t>>> Connecting to: %s\n", pssid);
    WiFi.mode (WIFI_STA);
    WiFi.begin (pssid, ppasswd);
    }

void OTA_C::Loop ()
    {
    ArduinoOTA.handle ();
    }

OTA_C UpdateOTA;


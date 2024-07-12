//#######################################################################
// Module:     WebOTA.cpp
// Descrption: Web over the air software update
// Creator:    markeby
// Date:       7/11/2024
//#######################################################################
#include <Arduino.h>
#include <WiFi.h>
#include <ElegantOTA.h>

#include "WebOTA.h"

AsyncWebServer Server (80);

static unsigned long ota_progress_millis = 0;

static void onOTAStart ()
    {
    Serial.println ("OTA update started!");
    // <Add your own code here>
    }

static void onOTAProgress (size_t current, size_t final)
    {

    if ( millis () - ota_progress_millis > 1000 )   // Log every 1 second
        {
        ota_progress_millis = millis ();
        Serial.printf ("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
        }
    }

static void onOTAEnd (bool success)
    {
    // Log when OTA has finished
    if ( success )
        {
        Serial.println ("OTA update finished successfully!");
        }
    else
        {
        Serial.println ("There was an error during OTA update!");
        }
    // <Add your own code here>
    }

//#######################################################################
bool OTA_C::WaitWiFi ()
    {
    if ( WiFi.status () == WL_CONNECTED )
        {
        IPaddressString = WiFi.localIP ().toString ();
        printf ("\t>>> OTA available.  IP: %s\n", IPaddressString);
        WiFi_On = true;
        return (true);
        }
    return (false);
    }

//#######################################################################
void OTA_C::Setup (const char* pssid, const char* ppasswd)
    {
    printf ("\t>>> Connecting to: %s\n", pssid);
    WiFi.mode  (WIFI_STA);
    WiFi.begin (pssid, ppasswd);
    }

//#######################################################################
void OTA_C::Begin ()
    {
    Server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Hi! This is ElegantOTA AsyncDemo.");
        });

    ElegantOTA.begin (&Server);
    ElegantOTA.onStart (onOTAStart);
    ElegantOTA.onProgress (onOTAProgress);
    ElegantOTA.onEnd (onOTAEnd);

    Server.begin ();
    }

void OTA_C::Loop ()
    {
    ElegantOTA.loop();
    }

OTA_C UpdateOTA;


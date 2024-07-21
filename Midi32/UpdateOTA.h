//#######################################################################
// Module:     WebOTA.h
// Descrption: Web over the air software update
// Creator:    markeby
// Date:       7/11/2024
//#######################################################################
#pragma once

class OTA_C
    {
private:
    bool   WiFi_On;
    String IPaddressString;
public:
    void WaitWiFi       (void);
    void Setup          (const char* pssid, const char* ppasswd);
    void Loop           (void);

    const char* GetIP   (void)
        { return (IPaddressString.c_str ()); }
    bool WiFiStatus     (void)
        { return (WiFi_On); }
    };

extern OTA_C UpdateOTA;

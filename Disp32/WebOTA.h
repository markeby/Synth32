//
//   WebOTA.h
//
//  Web Over The Air update tool
//  to load the software.
//
//  author: markeby
//
#pragma once

class OTA_C
    {
private:
    bool   WiFi_On;
    String IPaddressString;
public:
    bool WaitWiFi   (void);
    void Setup      (const char* pssid, const char* ppasswd);
    void Begin      (void);
    const char* GetIP    (void) { return (IPaddressString.c_str ()); }
    bool WiFiStatus (void) { return (WiFi_On); }
    };

extern OTA_C UpdateOta;


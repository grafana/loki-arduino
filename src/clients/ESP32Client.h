#ifndef ESP32Client_h
#define ESP32Client_h

#if defined(ESP32)

#include "LokiClient.h"
#include <WiFi.h>
#include <time.h>
#include <esp_sntp.h>
#include <SSLClient.h>

class ESP32Client : public LokiClient
{
public:
    ESP32Client();
    ~ESP32Client();

protected:
    bool _begin();
    uint64_t _getTimeNanos();
    void _checkConnection();
    

private:
    WiFiClient *_wifiClient;
    void _connect();
};

#endif // ESP32

#endif // ESP32Client_h
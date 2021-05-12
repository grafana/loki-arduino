#ifndef ESP32Client_h
#define ESP32Client_h

#if defined(ESP32)

#include "LokiClient.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <time.h>
#include <esp_sntp.h>

class ESP32Client : public LokiClient
{
public:
    ESP32Client();

protected:
    bool _begin();
    bool _send(char *entry, size_t length);
    uint64_t _getTimeNanos();
    

private:
    WiFiClientSecure *_wifiClient;
    HTTPClient *_httpClient;

    void _connect();
};

#endif // ESP32

#endif // ESP32Client_h
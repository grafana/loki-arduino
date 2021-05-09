#ifndef ESP32Client_h
#define ESP32Client_h

#if defined(ESP32)

#include "LokiClient.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

class ESP32Client : public LokiClient
{
public:
    ESP32Client();

protected:
    bool _begin();
    bool _send(String entry);
    

private:
    WiFiClientSecure *_wifiClient;
    HTTPClient *_httpClient;
};

#endif // ESP32

#endif // ESP32Client_h
#if defined(ESP32)
#include "ESP32Client.h"

ESP32Client::ESP32Client()
{
}

bool ESP32Client::_begin()
{
    WiFiClientSecure *client = new WiFiClientSecure;
    if (_cert && _cert.length() > 0)
    {
        client->setCACert(_cert.c_str());
    }
    else
    {
        client->setInsecure();
    }
    _wifiClient = client;

    _httpClient = new HTTPClient;
    _httpClient->setReuse(true);
    //TODO UserAgent
}

bool ESP32Client::_send(String entry)
{
    if (_wifiClient)
    {
        _httpClient->begin(_url);
        _httpClient->addHeader("Content-Type", "application/json");
        int httpCode = _httpClient->POST(entry);
        if (httpCode > 0)
        {
            LOKI_DEBUG_PRINTF("timefidget [HTTP] POST...  Code: %d\n", httpCode);
        }
        else
        {
            LOKI_DEBUG_PRINTF("timefidget [HTTP] POST... Error: %s\n", _httpClient->errorToString(httpCode).c_str());
        }

        _httpClient->end();
    }
};

#endif
#if defined(ESP32)
#include "ESP32Client.h"

static const char UserAgent[] PROGMEM = "loki-arduino/" LOKI_CLIENT_VERSION " (ESP32)";

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
    _httpClient->setUserAgent(UserAgent);

    _connect();
}

bool ESP32Client::_send(String entry)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.disconnect();
        yield();
        _connect();
    }
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
    else
    {
        LOKI_DEBUG_PRINTLN("Uninitialized wifi client, did you call begin()?");
    }
};

void ESP32Client::_connect()
{
    LOKI_DEBUG_PRINT("Connecting to '");
    LOKI_DEBUG_PRINT(_wifiSsid);
    LOKI_DEBUG_PRINT("' ...");

    WiFi.mode(WIFI_STA);
    WiFi.begin(_wifiSsid.c_str(), _wifiPass.c_str());
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    LOKI_DEBUG_PRINTLN("connected");

    LOKI_DEBUG_PRINT("IP address: ");
    LOKI_DEBUG_PRINTLN(WiFi.localIP());
}

#endif
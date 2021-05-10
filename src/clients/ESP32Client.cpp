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

    if (_user && _user.length() > 0 && _pass && _pass.length() > 0)
    {
        _httpClient->setAuthorization(_user.c_str(), _pass.c_str());
    }

    LOKI_SERIAL.println("Connecting wifi");
    _connect();

    LOKI_DEBUG_PRINT("Setting up sntp and setting time from pool.ntp.org");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "172.20.31.1");
    sntp_init();

    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET)
    {
        delay(1000);
        LOKI_DEBUG_PRINT(".");
    }

    LOKI_DEBUG_PRINTF("Time set succesfully, current time in nanos: %d\n", _getTimeNanos())
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
            LOKI_DEBUG_PRINTF("Loki POST...  Code: %d ", httpCode);
            if (httpCode >= 400)
            {
                _httpClient->writeToStream(&LOKI_SERIAL);
            }
            LOKI_DEBUG_PRINTLN();
        }
        else
        {
            LOKI_DEBUG_PRINTF("Loki POST... Error: %s\n", _httpClient->errorToString(httpCode).c_str());
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
        LOKI_DEBUG_PRINT(".");
    }
    LOKI_DEBUG_PRINTLN("connected");

    LOKI_DEBUG_PRINT("IP address: ");
    LOKI_DEBUG_PRINTLN(WiFi.localIP());
}

uint64_t ESP32Client::_getTimeNanos()
{
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    return (uint64_t)tv_now.tv_sec * 1000000000L + (uint64_t)tv_now.tv_usec * 1000;
};

#endif
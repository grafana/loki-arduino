#if defined(ESP32)
#include "ESP32Client.h"

static const char UserAgent[] PROGMEM = "loki-arduino/" CLIENT_VERSION " (ESP32)";

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

    SERIAL.println("Connecting wifi");
    _connect();

    DEBUG_PRINT("Setting up sntp and setting time from pool.ntp.org");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "172.20.31.1");
    sntp_init();

    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET)
    {
        delay(1000);
        DEBUG_PRINT(".");
    }

    DEBUG_PRINTF("Time set succesfully, current time in nanos: %d\n", _getTimeNanos())
}

bool ESP32Client::_send(char *entry, size_t length)
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
        _httpClient->addHeader("Content-Type", "application/x-protobuf");
        _httpClient->addHeader("Content-Encoding", "snappy");
        int httpCode = _httpClient->POST(reinterpret_cast<uint8_t *>(entry), length);
        if (httpCode > 0)
        {
            DEBUG_PRINTF("Loki POST...  Code: %d ", httpCode);
            if (httpCode >= 400)
            {
                _httpClient->writeToStream(&SERIAL);
            }
            DEBUG_PRINTLN();
        }
        else
        {
            DEBUG_PRINTF("Loki POST... Error: %s\n", _httpClient->errorToString(httpCode).c_str());
        }

        _httpClient->end();
    }
    else
    {
        DEBUG_PRINTLN("Uninitialized wifi client, did you call begin()?");
    }
};

void ESP32Client::_connect()
{
    DEBUG_PRINT("Connecting to '");
    DEBUG_PRINT(_wifiSsid);
    DEBUG_PRINT("' ...");

    WiFi.mode(WIFI_STA);
    WiFi.begin(_wifiSsid.c_str(), _wifiPass.c_str());
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        DEBUG_PRINT(".");
    }
    DEBUG_PRINTLN("connected");

    DEBUG_PRINT("IP address: ");
    DEBUG_PRINTLN(WiFi.localIP());
}

uint64_t ESP32Client::_getTimeNanos()
{
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    return (uint64_t)tv_now.tv_sec * 1000000000L + (uint64_t)tv_now.tv_usec * 1000;
};

#endif
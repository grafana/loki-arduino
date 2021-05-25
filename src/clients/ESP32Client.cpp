#if defined(ESP32)
#include "ESP32Client.h"

ESP32Client::ESP32Client() {
}

ESP32Client::~ESP32Client() {

    if (_wifiClient) {
        delete _wifiClient;
        // Because a client can be passed in only delete it if we also have a _wifiClient
        if (_client) {
            delete _client;
        }
    }
}

bool ESP32Client::_begin()
{
    WiFiClient* _wifiClient = new WiFiClient;
    if (_useTls) {
        if (_TAs && _numTAs > 0) {
            _client = new SSLClient(*_wifiClient, _TAs, _numTAs, A5, 1);
        }
        else {
            DEBUG_PRINT("setUseTls(true) was set but no certificates were provided via setCerts. Please provide certificates or setUseTls(false)");
            return false;
        }
    }
    else {
        _client = _wifiClient;
    }

    _httpClient = new HttpClient(*_client, _url, _port);

    DEBUG_PRINTLN("Connecting Wifi");

    _connect();

    DEBUG_PRINT("Setting up sntp and setting time from: ");
    DEBUG_PRINTLN(_ntpServer);

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, _ntpServer);
    sntp_init();

    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) {
        delay(1000);
        DEBUG_PRINT(".");
    }

    DEBUG_PRINTLN("Time set succesfully");
}


uint64_t ESP32Client::_getTimeNanos()
{
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    return (uint64_t)tv_now.tv_sec * 1000000000L + (uint64_t)tv_now.tv_usec * 1000;
};

void ESP32Client::_checkConnection() {
    // reconnect to WiFi if required
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.disconnect();
        yield();
        _connect();
    }
};

void ESP32Client::_connect()
{
    DEBUG_PRINT("Connecting to '");
    DEBUG_PRINT(_wifiSsid);
    DEBUG_PRINT("' ...");

    WiFi.mode(WIFI_STA);
    WiFi.begin(_wifiSsid, _wifiPass);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        DEBUG_PRINT(".");
    }
    DEBUG_PRINTLN("connected");

    DEBUG_PRINT("IP address: ");
    DEBUG_PRINTLN(WiFi.localIP());
}


#endif
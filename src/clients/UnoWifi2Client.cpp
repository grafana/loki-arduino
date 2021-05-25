#if defined(ARDUINO_AVR_UNO_WIFI_REV2)

#include "UnoWifi2Client.h"


UnoWifi2Client::UnoWifi2Client() {

};

UnoWifi2Client::~UnoWifi2Client() {

}

bool UnoWifi2Client::_begin() {
    errmsg = nullptr;
    _status = WL_IDLE_STATUS;

    if (_useTls) {
        _client = new WiFiSSLClient();
    }
    else {
        _client = new WiFiClient();
    }

    if (WiFi.status() == WL_NO_MODULE) {
        DEBUG_PRINTLN("Communication with WiFi module failed!");
        errmsg = "could not communicate with WiFi module";
        return false;
    }

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        DEBUG_PRINTLN("Please upgrade the firmware");
        errmsg = "please upgrade the firmware";
        return false;
    }

    _connect();

    return true;
};

uint64_t UnoWifi2Client::_getTimeNanos() {
    uint64_t epoch;
    int numberOfTries = 0, maxTries = 6;
    do {
        epoch = WiFi.getTime();
        numberOfTries++;
        if (epoch == 0) {
            delay(2000);
        }
    } while ((epoch == 0) && (numberOfTries < maxTries));

    if (numberOfTries >= maxTries) {
        DEBUG_PRINTLN("NTP unreachable!!");
        return 0;
    }
    else {
        return epoch * 1000 * 1000 * 1000;
    }

};

void UnoWifi2Client::_checkConnection() {
    //reconnect to WiFi if required
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.disconnect();
        _connect();
    }
};

void UnoWifi2Client::_connect() {

    while (_status != WL_CONNECTED) {

        DEBUG_PRINT("Attempting to connect to SSID: ");
        DEBUG_PRINTLN(_wifiSsid);

        _status = WiFi.begin(_wifiSsid, _wifiPass);
        // wait 5 seconds for connection:
        delay(5000);
    }

    DEBUG_PRINTLN("Connected to wifi");
    DEBUG_PRINT("IP address: ");
    DEBUG_PRINTLN(WiFi.localIP());
};

#endif
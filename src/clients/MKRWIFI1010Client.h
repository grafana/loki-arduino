#ifndef MKRWIFI1010Client_h
#define MKRWIFI1010Client_h

#if defined(ARDUINO_SAMD_MKRWIFI1010)

#include "LokiClient.h"
#include <WiFiNINA.h>
#include <SSLClient.h>


class MKRWIFI1010Client : public LokiClient
{
public:
    MKRWIFI1010Client();
    ~MKRWIFI1010Client();

protected:
    bool _begin();
    uint64_t _getTimeNanos();
    void _checkConnection();

private:
    int _status;
    Client* _wifiClient;
    bool _connected;
    void _connect();
};

#endif // ARDUINO_SAMD_MKRGSM1400

#endif // MKRWIFI1010Client_h
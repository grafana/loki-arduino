#ifndef UnoWifi2Client_h
#define UnoWifi2Client_h

#if defined(ARDUINO_AVR_UNO_WIFI_REV2)

#include "LokiClient.h"
#include <SPI.h>
#include <WiFiNINA.h>

class UnoWifi2Client : public LokiClient
{
public:
    UnoWifi2Client();
    ~UnoWifi2Client();

protected:
    bool _begin();
    uint64_t _getTimeNanos();
    void _checkConnection();
private:
    int _status;
    void _connect();
    
};

#endif // ARDUINO_AVR_UNO_WIFI_REV2

#endif // UnoWifi2Client_h
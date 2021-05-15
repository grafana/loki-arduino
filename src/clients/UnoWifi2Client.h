#ifndef UnoWifi2Client_h
#define UnoWifi2Client_h

#if defined(ARDUINO_AVR_UNO_WIFI_REV2)

#include "LokiClient.h"

class UnoWifi2Client : public LokiClient
{
public:
    UnoWifi2Client();
    ~UnoWifi2Client();

protected:
    bool _begin();
    bool _send(char *entry, size_t length);
    uint64_t _getTimeNanos();
};

#endif // ARDUINO_AVR_UNO_WIFI_REV2

#endif // UnoWifi2Client_h
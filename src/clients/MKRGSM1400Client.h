#ifndef MKRGSM1400Client_h
#define MKRGSM1400Client_h

#if defined(ARDUINO_SAMD_MKRGSM1400)

#include "LokiClient.h"
#include <MKRGSM.h>
#include <SSLClient.h>
#include <ArduinoHttpClient.h>

class MKRGSM1400Client : public LokiClient
{
public:
    MKRGSM1400Client();
    ~MKRGSM1400Client();

protected:
    bool _begin();
    uint64_t _getTimeNanos();

private:
    bool _connected;
    GPRS* _gprs;
    GSM* _gsm;
    GSMClient* _gsmClient;
};

#endif // ARDUINO_SAMD_MKRGSM1400

#endif // MKRGSM1400Client_h
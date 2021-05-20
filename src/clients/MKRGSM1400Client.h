#ifndef MKRGSM1400Client_h
#define MKRGSM1400Client_h

#if defined(ARDUINO_SAMD_MKRGSM1400)

#include "LokiClient.h"
#include <MKRGSM.h>
#include <ArduinoBearSSL.h>
#include "certificates.h"

// #define DEBUG_GSM_GENERIC_PORT       LOKI_SERIAL
// #define _GSM_GENERIC_LOGLEVEL_       5

// #define GSM_MODEM_UBLOX         true


// #include <GSM_Generic_Main.h>
#define LOGGING
#include <ArduinoHttpClient.h>

class MKRGSM1400Client : public LokiClient
{
public:
    MKRGSM1400Client();
    ~MKRGSM1400Client();

protected:
    bool _begin();
    bool _send(char* entry, size_t length);
    uint64_t _getTimeNanos();

private:
    bool _connected;
    GPRS* _gprs;
    GSM* _gsm;
    GSMClient* _gsmClient;
    Client* _arduinoClient;
    HttpClient* _client;
};

#endif // ARDUINO_SAMD_MKRGSM1400

#endif // MKRGSM1400Client_h
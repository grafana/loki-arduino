#ifndef MKRGSM1400Client_h
#define MKRGSM1400Client_h

#if defined(ARDUINO_SAMD_MKRGSM1400)

#include "LokiClient.h"

class MKRGSM1400Client : public LokiClient
{
public:
    MKRGSM1400Client();

protected:
    bool _begin();
    bool _send(String entry);
    uint64_t _getTimeNanos();
};

#endif // ARDUINO_SAMD_MKRGSM1400

#endif // MKRGSM1400Client_h
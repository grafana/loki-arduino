#ifndef Loki_H
#define Loki_H

#include <Arduino.h>
#include "LokiClient.h"

#define LOKI_DEBUG



#if defined(ESP32)

#include "clients/ESP32Client.h"
typedef ESP32Client Loki;

#elif defined(ARDUINO_SAMD_MKRGSM1400)

#include "clients/MKRGSM1400Client.h"
typedef MKRGSM1400Client Loki;

#endif // Types

#endif // Loki_H
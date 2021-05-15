#ifndef Loki_H
#define Loki_H

#include <Arduino.h>
#include "LokiClient.h"


#if defined(ESP32)

#include "clients/ESP32Client.h"
typedef ESP32Client Loki;

#elif defined(ARDUINO_SAMD_MKRGSM1400)

#include "clients/MKRGSM1400Client.h"
typedef MKRGSM1400Client Loki;

#elif defined(ARDUINO_AVR_UNO_WIFI_REV2)

#include "clients/UnoWifi2Client.h"
typedef UnoWifi2Client Loki;

#endif // Types

#endif // Loki_H
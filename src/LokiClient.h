#ifndef LokiClient_H
#define LokiClient_H

#include <Arduino.h>
#include <PromLokiTransport.h>
#include "LokiStreams.h"
#include <ArduinoHttpClient.h>
#include "LokiDebug.h"

static const char UserAgent[] PROGMEM = "loki-arduino/0.2.2";

class LokiClient {
public:
    LokiClient();
    LokiClient(PromLokiTransport& transport);
    ~LokiClient();

    enum SendResult {
        SUCCESS,
        FAILED_RETRYABLE,
        FAILED_DONT_RETRY
    };

    void setUrl(const char* url);
    void setPath(char* path);
    void setPort(uint16_t port);
    void setUser(const char* user);
    void setPass(const char* pass);

    void setDebug(Stream& stream);

    void setTransport(PromLokiTransport& transport);
    uint64_t getTimeNanos();
    uint16_t getConnectCount();

    bool begin();
    SendResult send(LokiStreams& streams);

    char* errmsg;

protected:
    Stream* _debug = nullptr;
    PromLokiTransport* _transport = nullptr;
    Client* _client = nullptr;
    HttpClient* _httpClient = nullptr;

    const char* _url;
    char* _path;
    uint16_t _port;
    const char* _user;
    const char* _pass;
    uint16_t _connectCount = 0;

    SendResult _send(uint8_t* entry, size_t length);

};

#endif
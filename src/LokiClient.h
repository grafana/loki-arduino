#ifndef LokiClient_H
#define LokiClient_H

#include <Arduino.h>
#include "Entry.h"
#include "LokiStreams.h"
#include "Util.h"

#define LOKI_CLIENT_VERSION "0.1.0"

class LokiClient
{
public:
    LokiClient();
    // LokiClient(const char *Url);
    // LokiClient(const char *Url, const char *user, const char *pass);
    // LokiClient(const char *Url, const char *user, const char *pass, const char *cert);
    ~LokiClient();

    void setUrl(const char *url);
    void setUser(const char *user);
    void setPass(const char *pass);
    void setCert(const char *cert);
    void setWifiSsid(const char *wifiSsid);
    void setWifiPass(const char *wifiPass);

    bool begin();
    bool send(Entry &entry);
    uint64_t getTimeNanos();

protected:
    String _url;
    String _user;
    String _pass;
    String _cert;
    String _wifiSsid;
    String _wifiPass;
    virtual bool _begin() = 0;
    virtual bool _send(char *entry, size_t length) = 0;
    virtual uint64_t _getTimeNanos() = 0;
};

#endif
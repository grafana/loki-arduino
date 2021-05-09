#ifndef LokiClient_H
#define LokiClient_H

#include <Arduino.h>
#include "Entry.h"
#include "Util.h"

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
    bool send(Entry *entry);

protected:
    String _url;
    String _user;
    String _pass;
    String _cert;
    String _wifiSsid;
    String _wifiPass;
    virtual bool _begin() = 0;
    virtual bool _send(String entry) = 0;
};

#endif
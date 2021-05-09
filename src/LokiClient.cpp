#include "LokiClient.h"

LokiClient::LokiClient(){

};

LokiClient::~LokiClient(){

};

void LokiClient::setUrl(const char *url)
{
    _url = url;
};
void LokiClient::setUser(const char *user)
{
    _user = user;
};
void LokiClient::setPass(const char *pass)
{
    _pass = pass;
};
void LokiClient::setCert(const char *cert)
{
    _cert = cert;
};
void LokiClient::setWifiSsid(const char *wifiSsid)
{
    _wifiSsid = wifiSsid;
}
void LokiClient::setWifiPass(const char *wifiPass)
{
    _wifiPass = wifiPass;
}

bool LokiClient::begin()
{
    return _begin();
};

bool LokiClient::send(Entry &entry)
{
    return _send(entry.toString());
};

uint64_t LokiClient::getTimeNanos()
{
    return _getTimeNanos();
}
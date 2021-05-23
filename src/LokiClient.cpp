#include "LokiClient.h"

LokiClient::LokiClient() {

};

LokiClient::~LokiClient() {

};

void LokiClient::setUrl(const char* url) {
    _url = url;
};
void LokiClient::setPath(char* path) {
    _path = path;
}
void LokiClient::setPort(uint16_t port) {
    _port = port;
}
void LokiClient::setUser(const char* user) {
    _user = user;
};
void LokiClient::setPass(const char* pass) {
    _pass = pass;
};
void LokiClient::setUseTls(bool useTls) {
    _useTls = useTls;
}
void LokiClient::setCerts(const br_x509_trust_anchor* myTAs, int myNumTAs) {
    _TAs = myTAs;
    _numTAs = myNumTAs;
};
void LokiClient::setWifiSsid(const char* wifiSsid) {
    _wifiSsid = wifiSsid;
}
void LokiClient::setWifiPass(const char* wifiPass) {
    _wifiPass = wifiPass;
}
void LokiClient::setApn(const char* apn) {
    _apn = apn;
}
void LokiClient::setApnLogin(const char* apnLogin) {
    _apnLogin = apnLogin;
}
void LokiClient::setApnPass(const char* apnPass) {
    _apnPass = apnPass;
}
void LokiClient::setNtpServer(char* ntpServer) {
    _ntpServer = ntpServer;
}

void LokiClient::setDebug(Stream& stream) {
    _debug = &stream;
}

void LokiClient::setClient(Client& client){
    _client = &client;
}
Client* LokiClient::getClient(){
    return _client;
}

bool LokiClient::begin() {
    bool res = _begin();
    if (!res) {
        errmsg = "failed to init the client, enable debug logging for more info";
        return false;
    }
    _httpClient->setTimeout(15000);
    _httpClient->setHttpResponseTimeout(15000);
    _httpClient->connectionKeepAlive();
    return true;
};

bool LokiClient::send(LokiStreams& streams) {
    uint8_t buff[streams.getBufferSize()] = { 0 };
    uint16_t len = streams.toSnappyProto(buff);
    if (len <= 0){
        errmsg = streams.errmsg;
        return false;
    }
    return _send(buff, len);
};

uint64_t LokiClient::getTimeNanos() {
    return _getTimeNanos();
}

bool LokiClient::_send(uint8_t* entry, size_t len) {
    DEBUG_PRINTLN("Sending To Loki");

    // Make a HTTP request:
    if (_client->connected()) {
        DEBUG_PRINTLN("Connection already open");
    }
    else {
        DEBUG_PRINTLN("Connecting...");
        if (!_client->connect(_url, _port)) {
            DEBUG_PRINTLN("Connection failed");
            if (_client->getWriteError()) {
                DEBUG_PRINT("Write error on client: ");
                DEBUG_PRINTLN(_client->getWriteError());
                _client->clearWriteError();
            }
            errmsg = "Failed to connect to server, enable debug logging for more info";
            return false;
        }
        else {
            DEBUG_PRINTLN("Connected.")
        }
    }

    // Do a lot of this manually to avoid sending headers and things we don't want to send
    // Use the ArduinoHttpClient to facilitate in places.
    _httpClient->beginRequest();
    _client->print("POST ");
    _client->print(_path);
    _client->println(" HTTP/1.1");
    _client->print("Host: ");
    _client->println(_url);
    _client->println("Content-Type: application/x-protobuf");
    _client->println("Content-Encoding: snappy");
    if (_user && _pass) {
        _httpClient->sendBasicAuth(_user, _pass);
    }
    _client->print("User-Agent: ");
    _client->println(UserAgent);
    _client->print("Content-Length: ");
    _client->println(len);
    _httpClient->beginBody();
    _client->write(entry, len);
    _client->println();


    DEBUG_PRINTLN("Sent, waiting for response");
    uint8_t waitAttempts = 0;
    // The default wait in responseStatusCode is 1s which is really long and can't easily be changed
    // so instead we will loop for data and make sure it's available before checking responseCode
    // 100 * 100 = 10,000 whic is a timeout of 10s and is less than the 15s timeout set in the client
    // so that the httpclient can handle actual timeouts.
    while (!_client->available() && waitAttempts < 100) {
        delay(100);
        waitAttempts++;
    }
    int statusCode = _httpClient->responseStatusCode();
    if (statusCode / 100 == 2) {
        DEBUG_PRINTLN("Loki Send Succeeded");
        // We don't use the _httpClient->responseBody() method both because it allocates a string
        // and also because it doesn't understand a 204 response code not having a content-length
        // header and will wait until a timeout for additional bytes.
        while (_client->available()) {
            char c = _client->read();
            DEBUG_PRINT(c);
        }
    }
    else {
        DEBUG_PRINT("Loki Send Failed with code: ");
        DEBUG_PRINT(statusCode);
        while (_client->available()) {
            char c = _client->read();
            DEBUG_PRINT(c);
        }
        errmsg = "Failed to send to Loki, enable debug logging for more info";
        return false;
    }
    return true;
}
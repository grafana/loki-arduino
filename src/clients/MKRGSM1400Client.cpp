#if defined(ARDUINO_SAMD_MKRGSM1400)

#include "MKRGSM1400Client.h"


MKRGSM1400Client::MKRGSM1400Client() {
    _gprs = new GPRS();
    _gsm = new GSM(false); //true to log AT commands
};

MKRGSM1400Client::~MKRGSM1400Client() {
    delete _gprs;
    delete _gsm;
    if (_arduinoClient) {
        delete _arduinoClient;
    }
    if (_client) {
        delete _client;
    }
};

bool MKRGSM1400Client::_begin() {
    while (!_connected) {
        LOKI_DEBUG_PRINTLN("Connecting to GSM Network");
        if ((_gsm->begin("") == GSM_READY) && (_gprs->attachGPRS(_apn, _apnLogin, _apnPass) == GPRS_READY)) {
            _connected = true;
            LOKI_DEBUG_PRINTLN("GSM Connected Succesfully");
        }
        else {
            LOKI_DEBUG_PRINTLN("Failed to connect to GSM Network");
            delay(2000);
        }

    }

    if (_cert && _cert.length() > 0) {
        LOKI_DEBUG_PRINTLN("Using SSL Client");
        _gsmClient = new GSMClient();
        const int rand_pin = A5;
        _arduinoClient = new SSLClient(*_gsmClient, TAs, (size_t)TAs_NUM, rand_pin, 1, SSLClient::SSL_WARN);
    }
    else {
        _arduinoClient = new GSMClient();
    }


    //FIXME port and url
    _client = new HttpClient(*_arduinoClient, _url, 443);
    _client->setTimeout(15000);
    _client->setHttpResponseTimeout(15000);
    _client->connectionKeepAlive();
};

bool MKRGSM1400Client::_send(char* entry, size_t length) {

    // TODO Verify modem is connected before attempting connection.

    // // Make a HTTP request:
    // _arduinoClient->print("POST ");  // 5
    // _arduinoClient->print("/loki/api/v1/push"); //17
    // _arduinoClient->println(" HTTP/1.1"); //9+2
    // _arduinoClient->print("Host: ");  //6
    // _arduinoClient->println(_url.c_str()); //logs-prod-us-central1.grafana.net 33+2
    // // _arduinoClient->println("Connection: keep-alive");
    // _arduinoClient->println("Content-Type: application/x-protobuf"); //36+2
    // _arduinoClient->println("Content-Encoding: snappy");  //24+2
    // _arduinoClient->println("Authorization: Basic ");  //173+2
    // _arduinoClient->println("User-Agent: loki-arduino/0.1.0"); //30+2
    // _arduinoClient->print("Content-Length: "); //16
    // _arduinoClient->println(length); //2+2
    // _arduinoClient->println(); //+2
    // _arduinoClient->println(entry); // 54+2
    // _arduinoClient->println();
    // Total 423bytes

    // Response
    // HTTP/1.1 204 No Content 23+2
    // 14:29:59.065 -> Date: Mon, 17 May 2021 18:30:00 GMT 35+2
    // 14:29:59.065 -> Via: 1.1 google 15+2
    // 14:29:59.065 -> Alt-Svc: clear 14+2
    // 14:29:59.065 -> +2
    // Total 97bytes
    // 2.5hrs 638 packets = 331,760 
    // ~15s

    LOKI_DEBUG_PRINTLN("Sending To Loki");
    _client->beginRequest();
    _client->post("/loki/api/v1/push");
    if (_user && _user.length() > 0 && _pass && _pass.length() > 0) {
        _client->sendBasicAuth(_user.c_str(), _pass.c_str());
    }
    _client->sendHeader("Content-Type", "application/x-protobuf");
    _client->sendHeader("Content-Length", length);
    _client->sendHeader("Content-Encoding", "snappy");
    _client->sendHeader("User-Agent: loki-arduino/0.1.0");
    _client->beginBody();
    _client->print(entry);
    _client->endRequest();
    LOKI_DEBUG_PRINTLN("Sent, waiting for response");
    int statusCode = _client->responseStatusCode();
    if (statusCode == 204) {
        LOKI_DEBUG_PRINTLN("Loki Send Succeeded");
        // We don't use the _client->responseBody() method both because it allocates a string
        // and also because it doesn't understand a 204 response code not having a content-length
        // header and will wait until a timeout for additional bytes.
        while (_arduinoClient->available()) {
            char c = _arduinoClient->read();
            LOKI_DEBUG_PRINT(c);
        }
    }
    else {
        LOKI_DEBUG_PRINT("Loki Send Failed with code: ");
        LOKI_DEBUG_PRINT(statusCode);
        while (_arduinoClient->available()) {
            char c = _arduinoClient->read();
            LOKI_DEBUG_PRINT(c);
        }
        return false;
    }
    return true;
};

uint64_t MKRGSM1400Client::_getTimeNanos() {
    uint64_t time = _gsm->getTime();
    return time * 1000 * 1000 * 1000;
};



#endif
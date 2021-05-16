#if defined(ARDUINO_SAMD_MKRGSM1400)

#include "MKRGSM1400Client.h"
#define LOGGING

// #ifdef __arm__
// // should use uinstd.h to define sbrk but Due causes a conflict
// extern "C" char* sbrk(int incr);
// #else  // __ARM__
// extern char* __brkval;
// #endif  // __arm__

// int freeMemory() {
//     char top;
// #ifdef __arm__
//     return &top - reinterpret_cast<char*>(sbrk(0));
// #elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
//     return &top - __brkval;
// #else  // __arm__
//     return __brkval ? &top - __brkval : &top - __malloc_heap_start;
// #endif  // __arm__
// }

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

    // FIXME Need to either PR the MKRGSM lib (https://github.com/arduino-libraries/MKRGSM/issues/132) or
    // switch to https://github.com/khoih-prog/GSM_Generic which allows actually setting root certificates
    // currently we use the provide cert to choose between http and https however we ignore the cert
    // becasue the library has no way to load it at the moment. It does load a list of default certs
    // you can see these here: https://github.com/arduino-libraries/MKRGSM/blob/master/src/utility/GSMRootCerts.h
    if (_cert && _cert.length() > 0) {
        // _arduinoClient = new GSMSSLClient();
        // ((GSMSSLClient*)_arduinoClient)->setCertificateValidationLevel(0);

        // ((GSMSSLClient*)_arduinoClient)->eraseTrustedRoot();
        // ((GSMSSLClient*)_arduinoClient)->setUserRoots(SECRET_GSM_ROOT_CERTS, SECRET_GSM_ROOT_SIZE);

        // // ((GSMSSLClient*)_arduinoClient)->setSignedCertificate(SECRET_CERT, "MKRGSM01", sizeof(SECRET_CERT));
        // // ((GSMSSLClient*)_arduinoClient)->setPrivateKey(SECRET_KEY, "MKRGSMKEY01", sizeof(SECRET_KEY));
        // // ((GSMSSLClient*)_arduinoClient)->useSignedCertificate("MKRGSM01");
        // // ((GSMSSLClient*)_arduinoClient)->usePrivateKey("MKRGSMKEY01");
        // ((GSMSSLClient*)_arduinoClient)->setTrustedRoot("Let_s_Encrypt_Authority_X3");

        // ((GSMSSLClient*)_arduinoClient)->setCertificateValidationLevel(0);
        LOKI_DEBUG_PRINTLN("Using SSL Client");
        _gsmClient = new GSMClient();
        const int rand_pin = A5;
        _arduinoClient = new SSLClient(*_gsmClient, TAs, (size_t)TAs_NUM, rand_pin);
    }
    else {
        _arduinoClient = new GSMClient();
    }

    // LOKI_DEBUG_PRINT("Free Mem:");
    // LOKI_DEBUG_PRINTLN(freeMemory());

    //FIXME port and url
    _client = new HttpClient(*_arduinoClient, _url, 443);
    _client->setTimeout(15000);
    _client->setHttpResponseTimeout(15000);
};

bool MKRGSM1400Client::_send(char* entry, size_t length) {

    //  LOKI_DEBUG_PRINT("Free Mem:");
    // LOKI_DEBUG_PRINTLN(freeMemory());
    // LOKI_DEBUG_PRINTLN("connecting...");

    // _client->beginRequest();
    // LOKI_DEBUG_PRINTLN("begin");
    // _client->get("/");
    // LOKI_DEBUG_PRINTLN("get");
    // _client->endRequest();
    // LOKI_DEBUG_PRINTLN("end");

    // int status = _client->responseStatusCode();
    // LOKI_DEBUG_PRINTLN("resp");
    // _client->responseBody();
    // LOKI_DEBUG_PRINTLN("body");
    // String response = _client->responseBody();

    // LOKI_DEBUG_PRINT("Status code: ");
    // LOKI_DEBUG_PRINTLN(status);
    // LOKI_DEBUG_PRINT("Response: ");
    // LOKI_DEBUG_PRINTLN(response);
    // LOKI_DEBUG_PRINT("Free Mem:");
    // LOKI_DEBUG_PRINTLN(freeMemory());

    // // if you get a connection, report back via serial:

    // if (_arduinoClient->connect(_url.c_str(), 443)) {
    //     Serial.println("connected");
    //     // Make a HTTP request:
    //     _arduinoClient->print("GET ");
    //     _arduinoClient->print("/");
    //     _arduinoClient->println(" HTTP/1->1");
    //     _arduinoClient->print("Host: ");
    //     _arduinoClient->println(_url.c_str());
    //     _arduinoClient->println("Connection: close");
    //     _arduinoClient->println();
    // }
    // else {
    //     // if you didn't get a connection to the server:
    //     MODEM.send("AT+USOER");
    //     Serial.println("connection failed");
    // }

    // while (_arduinoClient->available())
    // {
    //     char c = _arduinoClient->read();
    //     Serial.print(c);
    // }

    // // if the server's disconnected, stop the client:
    // if (!_arduinoClient->available() && !_arduinoClient->connected())
    // {
    //     Serial.println();
    //     Serial.println("disconnecting.");
    //     _arduinoClient->stop();

    // }

    // _client->get("/");

    // // read the status code and body of the response
    // int statusCode = _client->responseStatusCode();
    // String response = _client->responseBody();

    // Serial.print("Status code: ");
    // Serial.println(statusCode);
    // Serial.print("Response: ");
    // Serial.println(response);
    // LOKI_DEBUG_PRINTLN("Disable Cert Validation");  
    // ((GSMSSLClient*)_arduinoClient)->setCertificateValidationLevel(0);
    // MODEM.sendf("AT+USECPRF=0,0,%d",0);
    // delay(100);


    LOKI_DEBUG_PRINTLN("Sending To Loki");
    _client->beginRequest();
    LOKI_DEBUG_PRINTLN("1");
    _client->post("/loki/api/v1/push");
    LOKI_DEBUG_PRINTLN("2");
    if (_user && _user.length() > 0 && _pass && _pass.length() > 0)
    {
        _client->sendBasicAuth(_user.c_str(), _pass.c_str());
        LOKI_DEBUG_PRINTLN("3");
    }
    _client->sendHeader("Content-Type", "application/x-protobuf");
    _client->sendHeader("Content-Length", length);
    LOKI_DEBUG_PRINTLN("4");
    _client->sendHeader("Content-Encoding", "snappy");
    LOKI_DEBUG_PRINTLN("5");
    _client->beginBody();
    LOKI_DEBUG_PRINTLN("6");
    _client->print(entry);
    LOKI_DEBUG_PRINTLN("7");
    _client->endRequest();
    LOKI_DEBUG_PRINTLN("8");
    int statusCode = _client->responseStatusCode();
    // String body = _client->responseBody();
    if (statusCode == 204) {
        LOKI_DEBUG_PRINTLN("Loki Send Succeeded");
    }
    else {
        LOKI_DEBUG_PRINT("Loki Send Failed with code: ");
        LOKI_DEBUG_PRINT(statusCode);
        LOKI_DEBUG_PRINT("; message: ");
        LOKI_DEBUG_PRINTLN(_client->responseBody());
        return false;
    }
    return true;
};

uint64_t MKRGSM1400Client::_getTimeNanos() {
    uint64_t time = _gsm->getTime();
    return time * 1000 * 1000 * 1000;
};



#endif
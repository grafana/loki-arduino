
#include "config.h"
#include "certificates.h"
#include <PromLokiTransport.h>
#include <GrafanaLoki.h>
#include <PrometheusArduino.h>


// Create a client object for sending our data.
PromLokiTransport transport;
LokiClient lokiClient(transport);
PromClient promClient(transport);

// See other examples for a better explanation of these definitions, also look at https://github.com/grafana/prometheus-arduino for better prometheus information.

// Create Loki Streams
LokiStreams streams(2);
LokiStream uptime(2, 20, "{job=\"esp32\",type=\"uptime\"}");
LokiStream logger(3, 100, "{job=\"esp32\",type=\"log\"}");


// Create Prometheus Series
WriteRequest req(2);
TimeSeries ts1(5, "uptime_milliseconds_total", "{job=\"esp32\"}");
TimeSeries ts2(5, "heap_free_bytes", "{job=\"esp32\"}");

int loopCounter = 0;

void setup() {
    Serial.begin(115200);
    //Serial.begin(9600);
    
    // Wait 5s for serial connection or continue without it
    // some boards like the esp32 will run whether or not the 
    // serial port is connected, others like the MKR boards will wait
    // for ever if you don't break the loop.
    uint8_t serialTimeout;
    while (!Serial && serialTimeout < 50) {
        delay(100);
        serialTimeout++;
    }

    Serial.println("Running Setup");
    Serial.print("Free Mem Before Setup: ");
    Serial.println(freeMemory());

    transport.setUseTls(true);
    transport.setCerts(grafanaCert, strlen(grafanaCert));
    transport.setWifiSsid(WIFI_SSID);
    transport.setWifiPass(WIFI_PASSWORD);
    transport.setDebug(Serial);
    if (!transport.begin()) {
        Serial.println(transport.errmsg);
        while (true) {};
    }

    // Configure the Loki client
    lokiClient.setUrl(GC_LOKI_URL);
    lokiClient.setPath(GC_LOKI_PATH);
    lokiClient.setPort(GC_PORT);
    lokiClient.setUser(GC_LOKI_USER);
    lokiClient.setPass(GC_LOKI_PASS);

    lokiClient.setDebug(Serial);
    if (!lokiClient.begin()) {
        Serial.println(lokiClient.errmsg);
        while (true) {};
    }

    // Configure the Prom client
    promClient.setUrl(GC_PROM_URL);
    promClient.setPath(GC_PROM_PATH);
    promClient.setPort(GC_PORT);
    promClient.setUser(GC_PROM_USER);
    promClient.setPass(GC_PROM_PASS);

    promClient.setDebug(Serial);
    if (!promClient.begin()) {
        Serial.println(promClient.errmsg);
        while (true) {};
    }

    // Add our stream objects to the streams object
    streams.addStream(uptime);
    streams.addStream(logger);
    streams.setDebug(Serial);  // Remove this line to disable debug logging of the write request serialization and compression.

    // Add our TimeSeries to the WriteRequest
    req.addTimeSeries(ts1);
    req.addTimeSeries(ts2);
    req.setDebug(Serial);  // Remove this line to disable debug logging of the write request serialization and compression.

    Serial.print("Free Mem After Setup: ");
    Serial.println(freeMemory());
}

void loop() {
    uint64_t time;
    time = lokiClient.getTimeNanos();
    Serial.println(time);

    if (loopCounter < 2) {
        // Add log entry
        // Using a C style string is the best case for Arduino as it avoid heap fragmentation but it's also more complicated.
        // See the other Loke examples for using a String object instead.
        char str1[20];
        snprintf(str1, 20, "uptime %d", millis());
        if (!uptime.addEntry(time, str1, strlen(str1))) {
            Serial.println(uptime.errmsg);
        }

        // Add samples
        // Note Prom expects time in milliseconds and Loki getTime is nanoseconds so divide down to millis
        if (!ts1.addSample(time / 1000 / 1000, millis())) {
            Serial.println(ts1.errmsg);
        }
        // Note Prom expects time in milliseconds and Loki getTime is nanoseconds so divide down to millis
        if (!ts2.addSample(time / 1000 / 1000, freeMemory())) {
            Serial.println(ts2.errmsg);
        }

        loopCounter++;
    }
    else {
        // Send our data
        uint64_t start = millis();

        // Logger max entry size is 100
        char promResult[100];
        char lokiResult[100];
        // Send Prom
        PromClient::SendResult lres = promClient.send(req);
        if (!lres == PromClient::SendResult::SUCCESS) {
            Serial.println(promClient.errmsg);
            snprintf(promResult, 100, "Last Prom send failed: %s", promClient.errmsg);
        }
        else {
            uint32_t diff = millis() - start;
            snprintf(promResult, 100, "Last Prom send succeded in %dms", diff);
        }
        // Reset Batches
        ts1.resetSamples();
        ts2.resetSamples();

        start = millis();
        // Send Loki
        LokiClient::SendResult pres = lokiClient.send(streams);
        if (pres != LokiClient::SendResult::SUCCESS) {
            Serial.println(lokiClient.errmsg);
            snprintf(lokiResult, 100, "Last Loki send failed: %s", lokiClient.errmsg);
        }
        else {
            uint32_t diff = millis() - start;
            snprintf(lokiResult, 100, "Last Loki send succeded in %dms", diff);
        }

        // Reset log entries
        uptime.resetEntries();
        logger.resetEntries();

        // Add results from these sends to next batch
        if (!logger.addEntry(lokiClient.getTimeNanos(), promResult, strlen(promResult))) {
            Serial.println(logger.errmsg);
        }
        if (!logger.addEntry(lokiClient.getTimeNanos(), lokiResult, strlen(lokiResult))) {
            Serial.println(logger.errmsg);
        }

        loopCounter = 0;
    }

    delay(1000);
}
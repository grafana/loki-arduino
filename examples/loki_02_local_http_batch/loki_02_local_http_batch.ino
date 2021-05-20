
#include <Loki.h>
#include "config.h"

Loki client;
LokiStreams streams(3);
LokiStream stream1(10, 20, "{job=\"esp32\",stream=\"1\"}");
LokiStream stream2(10, 10, "{job=\"esp32\",stream=\"2\"}");
LokiStream stream3(10, 10, "{job=\"esp32\",stream=\"3\"}");

int loopCounter = 0;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        delay(10); // will pause Zero, Leonardo, etc until serial console opens

    Serial.println("Running Setup");
    client.setUrl("http://172.20.70.20:8080/loki/api/v1/push");
    client.setWifiSsid(WIFI_SSID);
    client.setWifiPass(WIFI_PASSWORD);
    client.begin();
    streams.addStream(&stream1);
    streams.addStream(&stream2);
    streams.addStream(&stream3);


    // stream2.addEntry(time, "stream2 ");
    // stream3.addEntry(time, "stream3 ");
    Serial.println("Running Setup5");
}

void loop()
{
    uint64_t time;
    time = client.getTimeNanos();
    Serial.println(time);

    if (loopCounter > 2)
    {
        //send
        loopCounter = 0;
        client.send(streams);
        stream1.resetEntries();
        stream2.resetEntries();
        stream3.resetEntries();
    }
    else
    {
        char str1[20];
        snprintf(str1, 20, "uptime %d", millis());
        if (!stream1.addEntry(time, str1, strlen(str1))) {
            Serial.println(stream1.errmsg);
        }
        char str2[10];
        snprintf(str2, 10, "stream2 %d", loopCounter);
        if (!stream2.addEntry(time, str2, strlen(str2))) {
            Serial.println(stream2.errmsg);
        }
        char str3[10];
        snprintf(str3, 10, "stream3 %d", loopCounter);
        if (!stream3.addEntry(time, str3, strlen(str3))) {
            Serial.println(stream3.errmsg);
        }
        loopCounter++;
    }
    
    delay(500);
}
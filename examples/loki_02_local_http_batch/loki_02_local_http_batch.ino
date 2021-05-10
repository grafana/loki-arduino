
#include <Loki.h>
#include "config.h"

Loki client;
LokiStreams streams(3);
LokiStream stream1(10, 2);
LokiStream stream2(10, 3);
LokiStream stream3(10, 2);

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

    
    stream1.addLabel("job", "esp32");
    stream1.addLabel("stream", "1");
    streams.addStream(stream1);

    stream2.addLabel("job", "esp32");
    stream2.addLabel("stream", "2");
    stream2.addLabel("foo", "bar");
    streams.addStream(stream2);

    stream3.addLabel("job", "esp32");
    stream3.addLabel("stream", "3");
    streams.addStream(stream3);

}

void loop()
{
    uint64_t time;
    time = client.getTimeNanos();
    Serial.println(time);
    
    if (loopCounter >= 9){
        //send
        loopCounter = 0;
    } else {
        stream1.addEntry(time, "stream1 " + loopCounter);
        stream2.addEntry(time, "stream2 " + loopCounter);
        stream3.addEntry(time, "stream3 " + loopCounter);
    }
    delay(1000);
}
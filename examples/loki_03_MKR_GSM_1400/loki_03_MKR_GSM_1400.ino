
#include <Loki.h>
#include "config.h"

Loki client;
LokiStreams streams(1);
LokiStream stream1(10, 2, 20, "{job=\"mkr\",stream=\"1\"}");

int loopCounter = 0;

void setup()
{
    Serial.begin(115200);
    //Serial.begin(9600);
    while (!Serial)
        delay(10); // will pause Zero, Leonardo, etc until serial console opens

    Serial.println("Running Setup");
    // delay(5000);
    // Loki client;
    client.setUrl("logs-prod-us-central1.grafana.net");
    client.setCert("dummy_val");
    client.setApn(APN);
    client.setApnLogin(APN_LOGIN);
    client.setApnPass(APN_PASS);
    client.setUser(GC_USER);
    client.setPass(GC_PASS);
    client.begin();
    streams.addStream(&stream1);


    // stream2.addEntry(time, "stream2 ");
    // stream3.addEntry(time, "stream3 ");
    Serial.println("Running Setup5");
}

void loop()
{
    uint64_t time;
    time = client.getTimeNanos();
    Serial.println(time);

    if (loopCounter > 0)
    {
        //send
        loopCounter = 0;
        client.send(streams);
        stream1.resetEntries();
    }
    else
    {
        char str1[20];
        snprintf(str1, 20, "uptime %d", millis());
        if (!stream1.addEntry(time, str1, strlen(str1))) {
            Serial.println(stream1.errmsg);
        }
        loopCounter++;
    }

    // Serial.println("Running...");
    
    delay(5000);
}
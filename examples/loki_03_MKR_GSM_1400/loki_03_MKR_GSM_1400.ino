
#include <bearssl_x509.h>
#include "certificates.h"
#include "config_test.h"
#include <Loki.h>


Loki client;
// Create a streams object for holding streams.
LokiStreams streams(2);
// Define the stream with a batch size of 10, and a max line length of 20 chars and the label set for the stream.
LokiStream stream1(10, 20, "{job=\"mkr\",stream=\"1\"}");

int loopCounter = 0;

void setup()
{
    Serial.begin(115200);
    //Serial.begin(9600);
    while (!Serial)
        delay(10); // will pause Zero, Leonardo, etc until serial console opens

    Serial.println("Running Setup");
 
    client.setUrl(GC_URL);
    client.setPort(GC_PORT);
    client.setPath(GC_PATH);
    client.setUseTls(true);
    client.setCerts(TAs, TAs_NUM);
    client.setApn(APN);
    client.setApnLogin(APN_LOGIN);
    client.setApnPass(APN_PASS);
    client.setUser(GC_USER);
    client.setPass(GC_PASS);
    client.setDebug(Serial);
    client.begin();
    streams.addStream(&stream1);
    
    streams.setDebug(Serial);


    // stream2.addEntry(time, "stream2 ");
    // stream3.addEntry(time, "stream3 ");
    
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
    
    delay(5000);
}
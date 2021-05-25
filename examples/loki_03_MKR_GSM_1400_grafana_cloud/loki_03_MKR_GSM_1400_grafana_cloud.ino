#include <bearssl_x509.h>
#include "config.h"
#include "certificates.h"
#include <Loki.h>

// Create a client object for sending our data.
Loki client;


// Define stream 1 'uptime' with a batch size of 2, and a max line length of 20 chars and the label set for the stream.
LokiStream uptime(2, 20, "{job=\"mkrgsm\",type=\"uptime\"}"); // Est bytes for full batch and max length line: (batch*maxLength+labels) 2*20+31 = 71 

// Define stream 2 'logger' with a batch size of 3, and a max line length of 100 chars and the label set for the stream.
// Note, to store more error messages if your device has enough RAM feel free to increase the batch size!
LokiStream logger(3, 100, "{job=\"mkrgsm\",type=\"log\"}");  //Est bytes for full batch and max length line: (batch*maxLength+labels) 3*100+28 = 328

// Create a streams object for holding streams, for this example we will make 2 streams, 
// 71+328 = 399 bytes for payload (plus timestamps), default buffer of 512 bytes should be enough.
LokiStreams streams(2);
// For a bigger buffer, See: https://github.com/grafana/loki-arduino#buffers for more info.
// LokiStreams streams(2,1024);


int loopCounter = 0;

void setup() {
    Serial.begin(115200);
    //Serial.begin(9600);
    while (!Serial)
        delay(10); // will pause Zero, Leonardo, etc until serial console opens

    Serial.println("Running Setup");

    // Configure the client
    client.setUrl(GC_URL);
    client.setPath(GC_PATH);
    client.setPort(GC_PORT);
    client.setUser(GC_USER);
    client.setPass(GC_PASS);
    client.setUseTls(true);
    client.setCerts(TAs, TAs_NUM);
    client.setApn(APN);
    client.setApnLogin(APN_LOGIN);
    client.setApnPass(APN_PASS);
    
    client.setDebug(Serial); // Remove this line to disable debug logging of the client.
    client.begin();

    // Add our stream objects to the streams object
    streams.addStream(uptime);
    streams.addStream(logger);
    streams.setDebug(Serial);  // Remove this line to disable debug logging of the write request serialization and compression.

}

void loop() {
    uint64_t time;
    time = client.getTimeNanos();
    Serial.println(time);

    if (loopCounter < 2) {
        // Using a C style string is the best case for Arduino as it avoid heap fragmentation but it's also more complicated.
        char str1[20];
        snprintf(str1, 20, "uptime %d", millis());
        if (!uptime.addEntry(time, str1, strlen(str1))) {
            Serial.println(uptime.errmsg);
        }

        loopCounter++;
    }
    else {
        // Send our data
        uint64_t start = millis();
        LokiClient::SendResult res = client.send(streams);
        if (res != LokiClient::SendResult::SUCCESS) {
            //Failed to send

            if (res == LokiClient::SendResult::FAILED_DONT_RETRY) {
                // If we get a non retryable error clear everything out and start from scratch.
                Serial.println("Failed to send with a 400 error that can't be retried");
                uptime.resetEntries();
                logger.resetEntries();
                loopCounter = 0;
            }
            else {
                // You can also use the String object which is easier to manage but allocates on the heap which can lead to heap fragmentation
                // https://cpp4arduino.com/2018/11/06/what-is-heap-fragmentation.html is a great resource for better understaind Heap Fragmentation
                // https://cpp4arduino.com/2018/11/21/eight-tips-to-use-the-string-class-efficiently.html also helps better explain safer ways to use String
                String err;
                err.reserve(100); // As mentioned in the above articles, reserving a size helps to reuse this space in the heap in the future.
                err += String("Send failed: ");

                if (client.errmsg) {
                    // Longest errmsg is 63 bytes + `Send failed: ` which is 13 bytes for a total of 76 bytes +1 for null terminator = 77bytes
                    // This will fit in our 100 byte max line length.
                    err += client.errmsg;
                }
                else {
                    err += "unknown error";
                }
                // The addEntry method copies the string to the heap so it doesn't matter that the String object is local scoped.
                // Only the first entries up to the batchSize will be accepted and then everything else will be ignored, increase the batch size
                // when creating the stream object to store more lines (and use more heap).
                if (!logger.addEntry(client.getTimeNanos(), err.c_str(), strlen(err.c_str()))) {
                    Serial.println(logger.errmsg);
                }

                // We don't reset loopCounter so we will keep retrying this until it succeeds.
            }
        }
        else {
            // Sucessful send, reset streams and loopCounter.
            uptime.resetEntries();
            // reset the log stream to remove sent entries, then add the output of the last send to the next batch.
            logger.resetEntries();

            uint32_t diff = millis() - start;
            char str1[50];
            snprintf(str1, 50, "Send succeded in %dms", diff);
            if (!logger.addEntry(client.getTimeNanos(), str1, strlen(str1))) {
                Serial.println(logger.errmsg);
            }

            loopCounter = 0;

        }
    }

    delay(5000);
}
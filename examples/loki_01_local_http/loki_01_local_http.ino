
#include <Loki.h>
#include "config.h"

Loki client;
Entry e;



void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Running Setup");
  client.setUrl("http://172.20.70.20:8080/loki/api/v1/push");
  client.setWifiSsid(WIFI_SSID);
  client.setWifiPass(WIFI_PASSWORD);
  client.begin();

  e.addLabel("job", "esp32test");
}



void loop() {
  uint64_t time;
  time = client.getTimeNanos();
  Serial.println(time);
  e.setTimeNanos(time);
  e.setVal("This is my test entry!");
  client.send(e);
  delay(1000);
  
}
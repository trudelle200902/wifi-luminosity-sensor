#include <Arduino.h>
#include "wifi_config.h"

#if !defined(WIFI_SSID) || !defined(WIFI_PASSWORD)
#error need to define WIFI_SSID and WIFI_PASSWORD in wifi_config.h
#endif

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  Serial.println("hello_world");
  Serial.println(WIFI_SSID);
  Serial.println(WIFI_PASSWORD);
  delay(1000);
}

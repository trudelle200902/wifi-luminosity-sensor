#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  Serial.println("hello_world");
  delay(1000);
}

#include <Arduino.h>
#include "wifi_config.h"
#include <WiFi.h>
// #include <ESPmDNS.h>
// #include <WiFiUdp.h>
// #include <ArduinoOTA.h>

#define LED_PIN (32)
#define PHOTORESISTOR_PIN (33)
#define LED_TIMEOUT (500)
#define SYNC_CODE (0xAA55AA55)
#define MSG_REQUESTED (1)

#if !defined(WIFI_SSID) || !defined(WIFI_PASSWORD)
#error need to define WIFI_SSID and WIFI_PASSWORD in wifi_config.h
#endif

const char* hostname = "esp32";
uint32_t led_toggle_timestamp = 0;
uint8_t led_state = LOW;
WiFiServer server(6666);
WiFiClient client;

struct __attribute__((packed)) s_message_rx
{
  uint32_t sync;
  uint8_t request;
};

struct __attribute__((packed)) s_message_tx
{
  uint32_t sync;
  uint16_t luminosity;
};

void setup() {
  Serial.begin(115200);

  // setup IO
  pinMode(LED_PIN, OUTPUT);
  pinMode(PHOTORESISTOR_PIN, OUTPUT);

  // Connect to wifi
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(hostname);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // if (!MDNS.begin("esp32")) {   // Set the hostname to "esp32.local"
  //   Serial.println("Error setting up MDNS responder!");
  //   while(1) {
  //     delay(1000);
  //   }
  // }
  // Serial.println("mDNS responder started");

  // ArduinoOTA
  //   .onStart([]() {
  //     String type;
  //     if (ArduinoOTA.getCommand() == U_FLASH)
  //       type = "sketch";
  //     else // U_SPIFFS
  //       type = "filesystem";

  //     // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
  //     Serial.println("Start updating " + type);
  //   })
  //   .onEnd([]() {
  //     Serial.println("\nEnd");
  //   })
  //   .onProgress([](unsigned int progress, unsigned int total) {
  //     Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  //   })
  //   .onError([](ota_error_t error) {
  //     Serial.printf("Error[%u]: ", error);
  //     if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
  //     else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
  //     else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
  //     else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
  //     else if (error == OTA_END_ERROR) Serial.println("End Failed");
  //   });

  // ArduinoOTA.begin();

  // Create esp32 server
  server.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // ArduinoOTA.handle();

  // Toggle LED (only for debugging)
  if(millis()>led_toggle_timestamp){
    led_state = !led_state;
    digitalWrite(LED_PIN, led_state);
    led_toggle_timestamp = millis()+LED_TIMEOUT;
    Serial.println("toggle");
  }

  if (!client || !client.connected()){ //search client if none are connected
    client = server.available();
  }
  if (client && client.connected()) { // If there is a client

    // read message request
    s_message_rx msg;
    client.read((uint8_t*)&msg, sizeof(msg));
    
    if (msg.sync == SYNC_CODE && msg.request == MSG_REQUESTED){ //msg request valid
      // build msg
      s_message_tx rsp_msg;
      rsp_msg.sync = SYNC_CODE;
      rsp_msg.luminosity = analogRead(PHOTORESISTOR_PIN);
      // send msg
      client.write((uint8_t*)&rsp_msg, sizeof(rsp_msg));
      Serial.print("message_sent ");
      Serial.println(rsp_msg.luminosity);
    }
  }
}

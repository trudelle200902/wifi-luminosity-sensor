#include <Arduino.h>
#include "wifi_config.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define PHOTORESISTOR_PIN (33)
#define SYNC_CODE (0xAA55AA55)
#define MSG_REQUESTED (1)

#if !defined(WIFI_SSID) || !defined(WIFI_PASSWORD)
#error need to define WIFI_SSID and WIFI_PASSWORD in wifi_config.h
#endif

const char* hostname = "esp32";
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

uint16_t getLuminosity();
void connectWifi();
void setupOTA();

void setup() {
  Serial.begin(115200);

  // setup IO
  pinMode(PHOTORESISTOR_PIN, OUTPUT);

  // Connect to wifi
  connectWifi();

  // Setup OTA to be able to update code via WiFi
  setupOTA();

  // Create esp32 server
  server.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();

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
      rsp_msg.luminosity = getLuminosity();
      // send msg
      client.write((uint8_t*)&rsp_msg, sizeof(rsp_msg));
      // Serial.print("message_sent ");
      // Serial.println(rsp_msg.luminosity);
    }
  }
}

uint16_t getLuminosity(){
  // now just a photoresistor connected to the esp ADC but can be changed in future for more precise measure
  return analogRead(PHOTORESISTOR_PIN);
}

void connectWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(hostname);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
}

void setupOTA(){
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else
        type = "filesystem";

      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}
#define ESP8266
#undef ESP32
#ifndef STASSID
#define STASSID "TellMyWiFiFoundSomeoneWhos24"
#define STAPSK  "TwinTurbo2.7"
#endif
#define SSD1306_128_32
#ifdef ESP32
#include <WiFi.h>
#endif
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#include "parse.h"
#include "sensors.h"
#include "structs.h"

// WiFi
const char *ssid = STASSID; // Enter your WiFi name
const char *password = STAPSK;  // Enter WiFi password
// MQTT Broker
const char *mqtt_broker = "192.168.0.133";
const char *topic = "device/unnamed";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

struct deviceConfiguration device {
  "un-named device", "8c:aa:b5:85:ef:94", 0
};


void setup() {
  char msgbuf[256];
  // Set software serial baud to 115200;
  Serial.begin(115200);
  #ifdef ESP32
  Wire.begin();
  #endif
  #ifdef ESP8266
  Wire.begin(2,0);
  #endif
  // connecting to a WiFi network
  
  Serial.println("Initializing display..");
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }

  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  sprintf((char *) &msgbuf, "Starting up!\nSSID: %s.", ssid);
  drawTextLn((char *) &msgbuf, strlen(msgbuf));

  WiFi.begin(STASSID, STAPSK);
  sprintf((char *) &msgbuf, ".");
  while (WiFi.status() != WL_CONNECTED) {
      drawText((char *) &msgbuf, strlen(msgbuf));
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  sprintf((char *) &msgbuf, "\nConnected!");
  drawText((char *) &msgbuf, strlen(msgbuf));
  
  delay(2000);

  display.clearDisplay();
  display.setCursor(0, 0); 
  sprintf((char *) &msgbuf, "MQTT: %s:%d", mqtt_broker, mqtt_port);
  drawText((char *) &msgbuf, strlen(msgbuf));

  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  sprintf((char *) &msgbuf, ".");
  while (!client.connected()) {
      drawText((char *) &msgbuf, strlen(msgbuf));    
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("Public emqx mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }

  delay(2000);
  
  // publish and subscribe
  client.publish(topic, "hello, emqx!");
  client.subscribe("system/#");
  client.subscribe(topic);
}


void drawText(char *str, unsigned int length) {

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
//  *(str + length) = '\0';
  //display.write(str);
  for(unsigned int i=0; i<length; i++) {
    display.write(str[i]);
  }
  display.display();
}

void drawTextLn(char *str, unsigned int length) {
  drawText(str, length);
  display.write("\n");
}


void callback(char *topic, byte *payload, unsigned int length) {
  display.clearDisplay();
  display.setCursor(0,0);
  drawTextLn(topic, strlen(topic));
  drawTextLn((char *) payload, length);
  parse_message(topic, (char *) payload, length);
}


uint64_t last_run_time = 0;

void loop() {
  client.loop();
  if (millis() - last_run_time >= 2000) {
    Serial.print("Current: "); Serial.print(millis()); Serial.print(", last: "); Serial.print(last_run_time); Serial.print(", diff: "); Serial.println(millis() - last_run_time);
    task_update_sensors(NULL);
    last_run_time = millis();
  }
  yield();
  
}

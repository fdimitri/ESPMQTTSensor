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

#include "structs.h"
#include "config.h"
#include "parse.h"
#include "sensors.h"
#include "display.h"
#include "mqtt.h"

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  char msgbuf[256];
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
  mqtt_connect();
  delay(2000);
  
  // publish and subscribe
  client.publish(topic, "hello, emqx!");
  client.subscribe("system/#");
  client.subscribe(topic);
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

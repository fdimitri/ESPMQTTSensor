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
#include "serial.h"
#include "tasks.h"

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
 
  Serial.println("Initializing display..");
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }

  oled_init();

  oled_printf("Starting up!\nWiFi SSD:\n%s", ssid);

  WiFi.begin(STASSID, STAPSK);
  serial_printf("Connecting to %s with PSK %s", STASSID, STAPSK);
  
  while (WiFi.status() != WL_CONNECTED) {
      oled_print(".");
      Serial.print(".");
      delay(500);
      
  }
  
  Serial.println("Connected to the WiFi network");
  oled_printf("\nConnected!");
  
  delay(2000);

  display.clearDisplay();
  display.setCursor(0, 0); 
  
  oled_printf("MQTT on port %d\n%s", mqtt_port, mqtt_broker);

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  mqtt_connect();
  delay(2000);
  
  client.publish(topic, "hello, emqx!");
  client.subscribe("system/#");
  client.subscribe(topic);
}


uint64_t last_run_time = 0;



void loop() {
  client.loop(); // MQTT Client Loop in library
  for (unsigned int i = 0; tasks[i].func != NULL; i++) {
    if (millis() - tasks[i].last_run_time >= tasks[i].run_every_millis) {
      tasks[i].func();
    }
  }
  yield();
}

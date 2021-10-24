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
#include <EEPROM.h>

#include "structs.h"
#include "config.h"
#include "parse.h"
#include "sensors.h"
#include "display.h"
#include "mqtt.h"
#include "serial.h"
#include "eeprom.h"
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

  eeprom_init();
  eeprom_load_config();
  
  if (device.config_version == 0xFFFF) {
    Serial.println("Device not configured!");
    while (device.wifi_ssid[0] == 0xFFF) {
      task_read_serial();
    }
  }
  else {
    eeprom_dump_config(eeprom_get_config());
  }
  
  Serial.println("Initializing display..");
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }

  oled_init();

  oled_printf("Starting up!\nWiFi SSD:\n%s", device.wifi_ssid);

  WiFi.begin(device.wifi_ssid, device.wifi_psk);
  serial_printf("Connecting to %s with PSK %s", device.wifi_ssid, device.wifi_psk);
  
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
  
  oled_printf("MQTT on port %d\n%s", device.mqtt_port, device.mqtt_broker);

  client.setServer(device.mqtt_broker, device.mqtt_port);
  client.setCallback(callback);

  mqtt_connect();
  delay(2000);
  
  client.subscribe("system/#");
  client.subscribe(topic);
}

void loop() {
  client.loop(); // MQTT Client Loop in library
  for (unsigned int i = 0; tasks[i].func != NULL; i++) {
    if (!tasks[i].run_every_millis || millis() - tasks[i].last_run_time >= tasks[i].run_every_millis) {
      if (tasks[i].run_every_millis > 100) {
        serial_printf("Running task %s\n", tasks[i].name);
      }
      tasks[i].last_run_time = millis();
      tasks[i].func();
    }
  }
  yield();
}

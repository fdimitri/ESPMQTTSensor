#include <Arduino.h>
#include <stdint.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <CRC32.h>

#include "errors.h"
#include "structs.h"
#include "config.h"
#include "parse.h"
#include "sensors.h"
#include "display.h"
#include "mqtt.h"
#include "serial.h"
#include "project_eeprom.h"
#include "tasks.h"
#include "sensor_htu31.h"
#include "sensor_bme280.h"

#ifdef ESP32
#include <WiFi.h>
#endif
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif


WiFiClient espClient;
PubSubClient client(espClient);

void wifi_connect();

void setup() {
  Serial.begin(115200);
  #ifdef ESP32
  Wire.begin();
  #endif
  #ifdef ESP8266
  Wire.begin(2,0);
  #endif

  Wire.setClock(CONFIG_IIC_SPEED);

  eeprom_init();
  int config_load_result = eeprom_load_config();
  eeprom_dump_config(eeprom_get_config());

  if (config_load_result < 0 || device.wifi_ssid[1] == 0xFF || device.wifi_ssid[0] == 0x00) {
    serial_printf("Device not configured or CRC32 invalid, loading default configuration");
    eeprom_save_default_config();

  }

  Serial.println("Initializing display..");
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  oled_init();
  sensors_init();

  oled_printf("Starting up!\nWiFi SSD:\n%s", device.wifi_ssid);

  wifi_connect();

  display.clearDisplay();
  display.setCursor(0, 0); 

  mqtt_connect();
}

void loop() {
  client.loop(); // MQTT Client Loop in library
  tasks_loop();
  yield();
}

void wifi_connect() {
  if (WiFi.status() == WL_CONNECTED) return;
  WiFi.begin(device.wifi_ssid, device.wifi_psk);
  WiFi.mode(WIFI_STA);
  serial_printf("Connecting to %s with PSK %s", device.wifi_ssid, device.wifi_psk);

  while (WiFi.status() != WL_CONNECTED) {
      oled_print(".");
      Serial.print(".");
      delay(500);
      task_read_serial();
  }
  
  Serial.println("Connected to the WiFi network");
  oled_printf("\nConnected!");
  return;
}

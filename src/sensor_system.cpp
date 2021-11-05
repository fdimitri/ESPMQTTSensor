#include <Arduino.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <CRC32.h>

#include <stdint.h>
#include <stdio.h>

#include "errors.h"
#include "structs.h"
#include "config.h"
#include "parse.h"
#include "display.h"
#include "mqtt.h"
#include "serial.h"
#include "tasks.h"
#include "sensor_htu31.h"
#include "sensor_bme280.h"
#include "main.h"
#include "sensors.h"

void syssensor_get_free_heap(void *customData, char *buf);
void syssensor_get_total_heap(void *customData, char *buf);
void syssensor_get_uptime(void *customData, char *buf);
void syssensor_get_rssi(void *customData, char *buf);
int syssensor_register_functions();

int syssensor_init() {
  syssensor_register_functions();
  return(0);
}

int syssensor_register_functions() {
  struct sensorControlData *cptr;

  cptr = sensor_definition_allocate("system.wifi.RSSI", "esp", syssensor_get_rssi, NULL, NULL, 0x0, NULL);
  sensor_definition_register(cptr);

  cptr = sensor_definition_allocate("system.uptime", "esp", syssensor_get_uptime, NULL, NULL, 0x0, NULL);
  sensor_definition_register(cptr);

  cptr = sensor_definition_allocate("system.memory.free", "esp", syssensor_get_free_heap, NULL, NULL, 0x0, NULL);
  sensor_definition_register(cptr);

#ifdef ESP32
  cptr = sensor_definition_allocate("system.memory.total", "esp", syssensor_get_total_heap, NULL, NULL, 0x0, NULL);
  sensor_definition_register(cptr);
#endif

  return(0);
}

void syssensor_get_uptime(void *customData, char *buf) {
  #ifdef ESP32
  sprintf(buf, "%lld", esp_timer_get_time());
  #endif
  #ifdef ESP8266
  sprintf(buf, "%ud", system_get_time());
  #endif
}

void syssensor_get_free_heap(void *customData, char *buf) {
  sprintf(buf, "%d", ESP.getFreeHeap());
}

void syssensor_get_total_heap(void *customData, char *buf) {
  #ifdef ESP32
  sprintf(buf, "%d", ESP.getHeapSize());
  #endif
}

void syssensor_get_rssi(void *customData, char *buf) {
  long rssi = WiFi.RSSI();
  sprintf(buf, "%ld", rssi);
}
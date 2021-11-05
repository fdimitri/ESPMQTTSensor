#include "config.h"
#ifdef CONFIG_HARDWARE_BME280
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

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "errors.h"
#include "structs.h"
// #include "config.h"
#include "parse.h"
#include "sensors.h"
#include "display.h"
#include "mqtt.h"
#include "serial.h"
#include "tasks.h"
#include "sensor_htu31.h"

#define SEALEVELPRESSURE_HPA (1013.25)

int bme280_init();
int bme280_register_functions(int idx);
void bme280_read_pressure(void *customData, char *buf);
void bme280_read_temp(void *customData, char *buf);
void bme280_read_altitude(void *customData, char *buf);
void bme280_read_humidity(void *customData, char *buf);

Adafruit_BME280 bme[2]; // I2C

int bme280_init() {
  unsigned status[2];

  serial_printf("bme280_init(): Called\n");

  for (int i = 0; i < 2; i++) {
    status[i] = bme[i].begin(0x76 + i);
    if (!status[i]) {
      serial_printf("Could not find BME280 sensor on I2C at address 0x%x\n", 0x76 + i);
    }
    else {
      serial_printf("Found BME280 sensor on I2C at address 0x%x\n", 0x76 + i);
      bme280_register_functions(i);  
    }
  }
 
  return(0);
}

int bme280_register_functions(int idx) {
  struct sensorControlData *cptr;
  int *s;
  
  s = (int *) malloc(sizeof(int));
  *s = idx;

  serial_printf("Allocating ram for cptr\n");
  cptr = sensor_definition_allocate("comfort.temperature", "bme280", bme280_read_temp, NULL, NULL, 0x0, s);
  sensor_definition_register(cptr);

  cptr = sensor_definition_allocate("comfort.relativeHumidity", "bme280", bme280_read_humidity, NULL, NULL, 0x0, s);
  sensor_definition_register(cptr); 

  cptr = sensor_definition_allocate("comfort.barometricPressure", "bme280", bme280_read_pressure, NULL, NULL, 0x0, s);
  sensor_definition_register(cptr);

  cptr = sensor_definition_allocate("comfort.altitude", "bme280", bme280_read_altitude, NULL, NULL, 0x0, s);
  sensor_definition_register(cptr); 

  return(0);
}

void bme280_read_pressure(void *customData, char *buf) {
  int *i = (int *) customData;
  sprintf(buf, "%f", bme[*i].readPressure() / 100.0F);
}

void bme280_read_temp(void *customData, char *buf) {
  int *i = (int *) customData;
  sprintf(buf, "%f", bme[*i].readTemperature());
}

void bme280_read_altitude(void *customData, char *buf) {
  int *i = (int *) customData;
  sprintf(buf, "%f", bme[*i].readAltitude(SEALEVELPRESSURE_HPA));
}

void bme280_read_humidity(void *customData, char *buf) {
  int *i = (int *) customData;
  sprintf(buf, "%f", bme[*i].readHumidity());
}

#endif

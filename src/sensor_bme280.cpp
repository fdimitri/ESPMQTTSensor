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
#include "eeprom.h"
#include "tasks.h"
#include "sensor_htu31.h"
// #include "sensor_bme280.h"

#define SEALEVELPRESSURE_HPA (1013.25)

int bme280_init();
int bme280_register_functions();
void bme280_read_pressure(char *buf);
void bme280_read_temp(char *buf);
void bme280_read_altitude(char *buf);
void bme280_read_humidity(char *buf);

Adafruit_BME280 bme; // I2C

int bme280_init() {
  unsigned status;

  serial_printf("bme280_init(): Called\n");

  status = bme.begin();  
  if (!status) {
    serial_printf("Could not find BME280 sensor on I2C\n");
  }
  bme280_register_functions();
  return(0);
}

int bme280_register_functions() {
  struct sensorControlData *cptr;
  
  serial_printf("Allocating ram for cptr\n");
  cptr = sensor_definition_allocate("comfort.temperature", "bme280", bme280_read_temp, NULL, NULL, 0x0);
  sensor_definition_register(cptr);

  cptr = sensor_definition_allocate("comfort.relativeHumidity", "bme280", bme280_read_humidity, NULL, NULL, 0x0);
  sensor_definition_register(cptr); 

  cptr = sensor_definition_allocate("comfort.barometricPressure", "bme280", bme280_read_pressure, NULL, NULL, 0x0);
  sensor_definition_register(cptr);

  cptr = sensor_definition_allocate("comfort.altitude", "bme280", bme280_read_altitude, NULL, NULL, 0x0);
  sensor_definition_register(cptr); 

  return(0);
}

void bme280_read_pressure(char *buf) {
  sprintf(buf, "%f", bme.readPressure() / 100.0F);
}

void bme280_read_temp(char *buf) {
  sprintf(buf, "%f", bme.readTemperature());
}

void bme280_read_altitude(char *buf) {
  sprintf(buf, "%f", bme.readAltitude(SEALEVELPRESSURE_HPA));
}


void bme280_read_humidity(char *buf) {
  sprintf(buf, "%f", bme.readHumidity());
}

#endif

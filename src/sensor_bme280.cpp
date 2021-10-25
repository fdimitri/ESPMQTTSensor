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
  const char *temp_hardware_name = "bme280_temp";
  const char *humidity_hardware_name = "bme280_humidity";
  const char *altitude_hardware_name = "bme280_altitude";
  const char *pressure_hardware_name = "bme280_pressure";
  struct sensorControlData *cptr;


  serial_printf("Allocating ram for cptr\n");
  
  cptr = (struct sensorControlData *) malloc(sizeof(struct sensorControlData));
  memset((void *) cptr, 0, sizeof(struct sensorControlData));

  serial_printf("Copying names into sensorName, sensorHardware\n");

  strcpy(cptr->sensorName, "comfort.temperature\0");
  strcpy(cptr->sensorHardware, temp_hardware_name);
  cptr->readSensor = bme280_read_temp;
  cptr->getState = cptr->setState = NULL;
  sensor_definition_register_functions(cptr);

  strcpy(cptr->sensorName, "comfort.relativeHumidity\0");
  strcpy(cptr->sensorHardware, humidity_hardware_name);
  cptr->readSensor = bme280_read_humidity;
  cptr->getState = cptr->setState = NULL;
  sensor_definition_register_functions(cptr);

  strcpy(cptr->sensorName, "comfort.barometricPressure\0");
  strcpy(cptr->sensorHardware, temp_hardware_name);
  cptr->readSensor = bme280_read_pressure;
  cptr->getState = cptr->setState = NULL;
  sensor_definition_register_functions(cptr);

  strcpy(cptr->sensorName, "comfort.approxAltitude\0");
  strcpy(cptr->sensorHardware, temp_hardware_name);
  cptr->readSensor = bme280_read_altitude;
  cptr->getState = cptr->setState = NULL;
  sensor_definition_register_functions(cptr);

  free(cptr);
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

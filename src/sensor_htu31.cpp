#include "config.h"
#ifdef CONFIG_HARDWARE_HTU31
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
#include "tasks.h"
#include "sensor_bme280.h"

Adafruit_HTU31D htu = Adafruit_HTU31D();

void htu31_read_temp(void *customData, char *buf);
void htu31_read_humidity(void *customData, char *buf);
int htu31_init();
int htu31_register_functions();

int htu31_init() {
  serial_printf("htu31_init(): Called\n");
  if (!(htu.begin(0x40) || htu.begin(0x41))) {
    serial_printf("htu31_init(): Sensor not found\n");
    return(-ESENSOR_NOT_FOUND);
  }
  serial_printf("Calling htu31_register_functions()\n");
  htu31_register_functions();
  serial_printf("Exiting htu31_init()\n");
  return(0);
}

int htu31_register_functions() {
  struct sensorControlData *cptr;

  serial_printf("Allocating ram for cptr\n");
  cptr = sensor_definition_allocate("comfort.temperature", "htu31d", htu31_read_temp, NULL, NULL, 0x0, NULL);
  sensor_definition_register(cptr);

  cptr = sensor_definition_allocate("comfort.relativeHumidity", "htu31d", htu31_read_humidity, NULL, NULL, 0x0, NULL);
  sensor_definition_register(cptr); 

  return(0);
}

void htu31_read_humidity(void *customData, char *buf) {
  sensors_event_t humidity, temp;
  htu.getEvent(&humidity, &temp);
  sprintf(buf, "%f", humidity.relative_humidity);
}

void htu31_read_temp(void *customData, char *buf) {
  sensors_event_t humidity, temp;
  htu.getEvent(&humidity, &temp);
  sprintf(buf, "%f", temp.temperature);
}
#endif

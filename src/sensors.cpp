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
#include "eeprom.h"
#include "tasks.h"
#include "sensor_htu31.h"
#include "sensor_bme280.h"
#include "sensor_system.h"
#include "sensor_esp8266adc.h"
#include "main.h"

void sensor_get_stub(char *);
int sensor_definition_register_functions(struct sensorControlData *scd);
int sensors_init();
int sensors_dump_scd_entry(struct sensorControlData *d);
int sensors_dump_scd_list();
struct sensorControlData *sensor_get_by_name(char *name);
struct sensorControlData *scd_head = NULL;
void sensor_definition_register(struct sensorControlData *scd);
struct sensorControlData *sensor_definition_allocate(const char *sensorName, const char *sensorHardware, scdCallback readSensor, scdCallback getState, scdCallback setState, uint32_t sensorFlags, void *customData);
void sensors_publish_all();
void sensors_read_all();
int sensor_get_highest_index_by_name(char *name);
void sensor_read(struct sensorControlData *cptr);

// struct sensorControlData sensors[] = {
//   { false, "comfort.temperature", "", "", 0, sensor_get_stub, NULL, NULL },
//   { false, "comfort.relativeHumidity", "", "", 0, sensor_get_stub, NULL, NULL },
//   { false, "comfort.barometricPressure", "", "", 0, sensor_get_stub, NULL, NULL },
//   { false, "comfort.approxAltitude", "", "", 0, sensor_get_stub, NULL, NULL },
//   { false, "comfort.lightLevel", "", "", 0, sensor_get_stub, NULL, NULL },
//   { false, "comfort.noiseLevel", "", "", 0, sensor_get_stub, NULL, NULL },
//   { true, "system.wifi.RSSI", "system", "", 0, sensor_get_rssi, NULL, NULL, },
//   { true, "system.uptime", "", "system", 0, sensor_get_uptime, NULL, NULL, },
//   { true, "system.battery.voltage", "", "system", 0, sensor_get_stub, NULL, NULL, },
//   { true, "system.memory.free", "", "system", 0, sensor_get_free_heap, NULL, NULL },
//   { true, "system.memory.total", "", "system", 0, sensor_get_total_heap, NULL, NULL },
//   { false, "comfort.fan1", "", "", 1, NULL, sensor_get_stub, sensor_get_stub },
//   { false, "comfort.fan2", "", "", 1, NULL, sensor_get_stub, sensor_get_stub },
//   { false, "comfort.dimmer", "", "", 1, NULL, sensor_get_stub, sensor_get_stub },
//   { false, "comfort.relay", "", "", 1, NULL, sensor_get_stub, sensor_get_stub },
//   { false, "system.battery.coulumbCounter", "", "", 0, sensor_get_stub, NULL, NULL },
//   { false, "comfort.pirMotionDetector", "", "", 0, sensor_get_stub, NULL, NULL },
//   { false, "", "", "", 0x8000, NULL, NULL, NULL },
// };

void sensor_get_stub(char *buf) {
  sprintf(buf, "%f", 3.2);
}

struct sensorControlData *sensor_definition_allocate(const char *sensorName, const char *sensorHardware, scdCallback readSensor = NULL, scdCallback getState = NULL, scdCallback setState = NULL, uint32_t sensorFlags = 0x0, void *customData = NULL) {
  struct sensorControlData *scd;

  scd = (struct sensorControlData *) malloc(sizeof(sensorControlData));
  memset((void *) scd, 0, sizeof(sensorControlData));

  strcpy(scd->sensorName, sensorName);
  strcpy(scd->sensorHardware, sensorHardware);
  scd->readSensor = readSensor;
  scd->getState = getState;
  scd->setState = setState;
  scd->isEnabled = true;
  scd->customData = customData;
  return(scd);
}

void sensors_read_all() {
  struct sensorControlData *cptr = scd_head;
  while (cptr) {
    if (cptr->isEnabled) {
      sensor_read(cptr);
    }
    cptr = cptr->next;
  }
}

void sensor_read(struct sensorControlData *cptr) {
  if (cptr->readSensor) cptr->readSensor(cptr->customData, (char *) &(cptr->currentData[0]));
  if (cptr->getState) cptr->getState(cptr->customData, (char *) &(cptr->currentData[0])); 
}

void sensors_publish_all() {
  struct sensorControlData *cptr = scd_head;
  while (cptr) {
    if (cptr->isEnabled) {
      publish_sensor(cptr);
    }
    cptr = cptr->next;
  }
}

int sensor_get_highest_index_by_name(char *name) {
  struct sensorControlData *cptr = scd_head;
  int idx = -1, cidx = 0;
  while (cptr) {
    if (!strncmp(cptr->sensorName, name, strlen(name))) {
      cidx = cptr->sensorName[strlen(cptr->sensorName) - 1] - '0';
      if (cidx > idx) idx = cidx;
    }
    cptr = cptr->next;
  }
  return(++idx);
}

void sensor_definition_register(struct sensorControlData *scd) {
  struct sensorControlData *cptr = scd_head;

  int maxidx = sensor_get_highest_index_by_name(scd->sensorName);

  snprintf(scd->sensorName, 63, "%s%d", scd->sensorName, maxidx);

  if (scd_head == NULL) {
    scd_head = scd;
    return;
  }

  while (cptr->next) {
    cptr = cptr->next;
  }

  cptr->next = scd;

  return;
}

int sensor_definition_register_functions(struct sensorControlData *newscd) {
  struct sensorControlData *scd;
  serial_printf("sensor_definition_register_functions() called\n");
  scd = sensor_get_by_name(newscd->sensorName);
  if (!scd) {
    serial_printf("sensor_definition.. not found %s\n", newscd->sensorName);
    return(-ESENSOR_NOT_FOUND_IN_SCD);  
  }
  serial_printf("Found scd: %p\n", scd);
  sensors_dump_scd_entry(scd);

  scd->isEnabled = true;
  serial_printf("Assigning functions..\n");
  scd->readSensor = newscd->readSensor;
  scd->getState = newscd->getState;
  scd->setState = newscd->setState;
 
  serial_printf("sizeof(scd->sensorHardware): %d\n", sizeof(scd->sensorHardware));
  memcpy(newscd->sensorHardware, scd->sensorHardware, sizeof(scd->sensorHardware));
  serial_printf("sensor_definition_register_functions() completed\n");
  return(0);
}

struct sensorControlData *sensor_get_by_name(char *name) {
  struct sensorControlData *cptr = scd_head;
  while (cptr) {
    if (!strcmp(cptr->sensorName, name)) {
      return(cptr);
    }
    cptr = cptr->next;
  }
  return(NULL);
}

int sensors_init() {
  syssensor_init();

  #ifdef CONFIG_HARDWARE_BME280
  bme280_init();
  #endif

  #ifdef CONFIG_HARDWARE_HTU31
  htu31_init();
  #endif

  #ifdef CONFIG_HARDWARE_ESP8266ADC
  adc8266_init();
  #endif

  return(0);
}

int sensors_dump_scd_entry(struct sensorControlData *d) {
  serial_printf("-------------------------\n");
  serial_printf("Enabled: %d, Name: %s, hardware: %s\n", d->isEnabled, d->sensorName, d->sensorHardware);
  serial_printf("Currentdata: %s, flags: %ld\n", d->currentData, d->sensorFlags);
  serial_printf("readSensor: %p, getState: %p, setState: %p\n", d->readSensor, d->getState, d->setState);
  serial_printf("next: %p\n", d->next);
  return(0);
}

int sensors_dump_scd_list() {
  struct sensorControlData *cptr = scd_head;

  while (cptr) {
    sensors_dump_scd_entry(cptr);
    cptr = cptr->next;
  }
  serial_printf("-------------------------\n");
  return(0);
}

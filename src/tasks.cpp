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
#include "eeprom.h"
//#include "tasks.h"
#include "sensor_htu31.h"
#include "sensor_bme280.h"
#include "main.h"

void task_update_sensors();
void task_read_serial();

struct taskRun tasks[] = {
   { "updateSensors", task_update_sensors, 2000, 0 },
   { "readSerial", task_read_serial, 0, 0 },
   { NULL, NULL, 0, 0 },
};

void task_update_sensors() {
  //char buf[256];
  for (unsigned int i = 0; sensors[i].sensorFlags != 0x8000; i++) {
//    sprintf((char *) &buf, "Going through sensor %d - %s, flags %d - %s", i, sensors[i].sensorName, sensors[i].sensorFlags, sensors[i].currentData);
    if (!sensors[i].isEnabled) {
      continue;
    }
    if (sensors[i].sensorFlags == 0 && sensors[i].readSensor) {
      sensors[i].readSensor((char *) &sensors[i].currentData);
    }
    if (sensors[i].sensorFlags == 1 && sensors[i].getState) {
      sensors[i].getState((char *) &sensors[i].currentData);
    }
    
    publish_sensor((struct sensorControlData *) &sensors[i]);
  }
}

void task_publish_sensors() {
  //char buf[256];
  for (unsigned int i = 0; sensors[i].sensorFlags != 0x8000; i++) {
//    sprintf((char *) &buf, "Going through sensor %d - %s, flags %d - %s", i, sensors[i].sensorName, sensors[i].sensorFlags, sensors[i].currentData);
    if (!sensors[i].isEnabled) {
      continue;
    }
    publish_sensor((struct sensorControlData *) &sensors[i]);
  }
}

void task_read_serial() {
  serial_read();
}

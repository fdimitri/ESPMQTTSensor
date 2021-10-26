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
  sensors_read_all();
}

void task_publish_sensors() {
  sensors_publish_all();
}

void task_read_serial() {
  serial_read();
}

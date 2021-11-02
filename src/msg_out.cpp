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
#include "sensors.h"
#include "display.h"
#include "mqtt.h"
#include "serial.h"
#include "project_eeprom.h"
#include "tasks.h"
#include "sensor_htu31.h"
#include "sensor_bme280.h"
#include "main.h"

void msg_to_system(unsigned int msg_id, ...);
void msg_to_serial(unsigned int msg_id, ...);

struct msgSystemList *get_system_message_by_id(unsigned int id);

struct msgSystemList system_messages[] = {
  { MSG_DEVICE_IDENTIFY, "DEVICE.IDENTIFY name:%s location:%s version:%s build_date:%s mac:%s" },
  { MSG_UPDATE_SENSOR, "UPDATE.SENSOR %s %s"},
  { MSG_ERROR_INCORRECT_PARAM_COUNT, "DEVICE.ERROR.PARAMS want:%d got:%d args:%s"},
  { MSG_ERROR_SENSOR_NOT_FOUND, "DEVICE.ERROR.SENSOR.NOTFOUND name:%s" },
  { 0, NULL }
};

void msg_to_serial(unsigned int msg_id, ...) {
  char *buffer = (char *) malloc(BUFFER_SIZE);
  char *topicbuf = (char *) malloc(BUFFER_SIZE);
  struct msgSystemList *msg;
  
  memset((void *) buffer, 0, BUFFER_SIZE);
  memset((void *) topicbuf, 0, BUFFER_SIZE);

  msg = get_system_message_by_id(msg_id);
  if (!msg) {
      return;
  }

  va_list args;
  va_start(args,  msg_id);
  vsnprintf(buffer, BUFFER_SIZE - 1, msg->fmt, args);
  va_end(args);

  serial_printf("%s", buffer);

  free(buffer);
  free(topicbuf);
}

void msg_to_system(unsigned int msg_id, ...) {
  char *buffer = (char *) malloc(BUFFER_SIZE);
  char *topicbuf = (char *) malloc(BUFFER_SIZE);
  struct msgSystemList *msg;
  
  memset((void *) buffer, 0, BUFFER_SIZE);
  memset((void *) topicbuf, 0, BUFFER_SIZE);

  msg = get_system_message_by_id(msg_id);
  if (!msg) {
      return;
  }

  va_list args;
  va_start(args,  msg_id);
  vsnprintf(buffer, BUFFER_SIZE - 1, msg->fmt, args);
  va_end(args);

  client.publish(mqtt_build_topic(topicbuf), buffer, strlen(buffer) + 1);

  free(buffer);
  free(topicbuf);
}

struct msgSystemList *get_system_message_by_id(unsigned int id) {
  for (unsigned int i = 0; system_messages[i].fmt != NULL; i++) {
    if (system_messages[i].msg_id == id) return(&system_messages[i]);
  }
  return(NULL);
}
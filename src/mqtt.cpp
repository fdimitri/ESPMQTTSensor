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
// #include "mqtt.h"
#include "serial.h"
#include "project_eeprom.h"
#include "tasks.h"
#include "sensor_htu31.h"
#include "sensor_bme280.h"
#include "main.h"

void callback(char *topic, byte *payload, unsigned int length);
void mqtt_connect();
char *mqtt_build_topic(char *topic);
void publish_sensor(struct sensorControlData *sensor);
void msg_to_system(unsigned int msg_id, ...);
struct msgSystemList *get_system_message_by_id(unsigned int id);

struct msgSystemList system_messages[] = {
  { MSG_DEVICE_IDENTIFY, "DEVICE.IDENTIFY name:%s location:%s version:%s build_date:%s mac:%s" },
  { MSG_UPDATE_SENSOR, "UPDATE.SENSOR %s %s"},
  { 0, NULL }
};

void callback(char *topic, byte *payload, unsigned int length) {
  display.clearDisplay();
  display.setCursor(0,0);
  oled_msgLn(topic, strlen(topic));
  oled_msgLn((char *) payload, length);
  parse_message(topic, (char *) payload, length);
}

void mqtt_connect() {
  char msgbuf[256];
  uint8_t retrycount = 64;
  
  if (client.connected()) return;

  serial_printf("Connecting to MQTT..\n");
  oled_printf("MQTT on port %d\n%s\n", device.mqtt_port, device.mqtt_broker);
  
  client.setServer(device.mqtt_broker, device.mqtt_port);
  client.setCallback(callback);

  while (!client.connected() && retrycount--) {
      task_read_serial();
      oled_msg((char *) &msgbuf, strlen(msgbuf));    
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("Client %s is connecting to the mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), device.mqtt_user, device.mqtt_pass)) {
          serial_printf("Connected to MQTT server %s:%d with %s:%s\n", device.mqtt_broker, device.mqtt_port, device.mqtt_user, device.mqtt_pass);
      } else {
          Serial.print("failed with state ");
          oled_printf("MQTT failed with state: %d\n", client.state());
          Serial.print(client.state());
          delay(2000);
      }
      delay(500);
  }
  
  char topicbuf[128];
  client.subscribe("system/#");
  client.subscribe(mqtt_build_topic((char *) &topicbuf));
}

char *mqtt_build_topic(char *topic) {
  sprintf(topic, "location/%s/device/%s", device.location, device.name);
  return(topic);
}

void publish_sensor(struct sensorControlData *sensor) {
  msg_to_system(MSG_UPDATE_SENSOR, sensor->sensorName, sensor->currentData);
}

void msg_to_system(unsigned int msg_id, ...) {
  char *buffer = (char *) malloc(BUFFER_SIZE);
  char *topicbuf = (char *) malloc(BUFFER_SIZE);
  struct msgSystemList *msg;
  memset((void *) buffer, 0, BUFFER_SIZE);
  memset((void *) topicbuf, 0, BUFFER_SIZE);

  msg = get_system_message_by_id(msg_id);
  
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
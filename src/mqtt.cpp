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
// #include "mqtt_h"
#include "serial.h"
#include "tasks.h"
#include "sensor_htu31.h"
#include "sensor_bme280.h"
#include "main.h"
#include "msg_out.h"

extern DeviceConfigurator deviceConfig;

void callback(char *topic, byte *payload, unsigned int length);
void mqtt_connect();
char *mqtt_build_topic(char *topic);
void publish_sensor(struct sensorControlData *sensor);


void callback(char *topic, byte *payload, unsigned int length) {
  display.clearDisplay();
  display.setCursor(0,0);
  oled_msgLn(topic, strlen(topic));
  oled_msgLn((char *) payload, length);
  parse_message(topic, (char *) payload, length);
}

void mqtt_connect() {
  char msgbuf[256];
  uint8_t retrycount = 4;
  
  if (client.connected()) return;

  String client_id = "esp8266-client-";
  client_id += String(WiFi.macAddress());

  msg_to_serial(MSG_INFO_MQTT_CONNECTING, deviceConfig.getMQTTBroker(), deviceConfig.getMQTTPort(), deviceConfig.getMQTTUser(), deviceConfig.getMQTTPass(), client_id.c_str());
  oled_printf("MQTT on port %d\n%s\n", deviceConfig.getMQTTPort(), deviceConfig.getMQTTBroker());
  
  client.setServer(deviceConfig.getMQTTBroker(), deviceConfig.getMQTTPort());
  client.setCallback(callback);

  while (!client.connected() && retrycount--) {
      task_read_serial();
      oled_msg((char *) &msgbuf, strlen(msgbuf));    

      if (client.connect(client_id.c_str(), deviceConfig.getMQTTUser(), deviceConfig.getMQTTPass())) {
          msg_to_serial(MSG_INFO_MQTT_CONNECTED);
      } else {
          oled_printf("MQTT failed with state: %d\n", client.state());
          msg_to_serial(MSG_INFO_MQTT_FAILED, client.state());
          task_read_serial();
          delay(100);
      }
  }

  char topicbuf[128];
  client.subscribe("system/#");
  client.subscribe(mqtt_build_topic((char *) &topicbuf));
}

char *mqtt_build_topic(char *topic) {
  sprintf(topic, "location/%s/device/%s", deviceConfig.getDeviceLocation(), deviceConfig.getDeviceName());
  return(topic);
}

void publish_sensor(struct sensorControlData *sensor) {
  msg_to_system(MSG_UPDATE_SENSOR, sensor->sensorName, sensor->currentData);
}


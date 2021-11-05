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
#include "macros.h"
#include "structs.h"
#include "config.h"
// #include "parse.h"
#include "sensors.h"
#include "display.h"
#include "mqtt.h"
#include "serial.h"
#include "tasks.h"
#include "sensor_htu31.h"
#include "sensor_bme280.h"
#include "main.h"
#include "msg_out.h"

void parse_config_stub(char *, char *[], unsigned int);
void parse_device_config_name(char *, char *[], unsigned int);
void parse_device_config_location(char *, char *[], unsigned int);
void parse_device_reboot(char *, char *[], unsigned int);
void parse_device_config_wifi(char *topic, char *argv[], unsigned int argc);
void parse_device_config_mqtt(char *topic, char *argv[], unsigned int argc);
void parse_device_config_clear(char *topic, char *argv[], unsigned int argc);
void parse_device_mqtt_subscribe(char *topic, char *argv[], unsigned int argc);
void parse_debug_dump_config(char *topic, char *argv[], unsigned int argc);
void parse_debug_dump_runningconfig(char *topic, char *argv[], unsigned int argc);
void parse_get_sensor(char *topic, char *argv[], unsigned int argc);
void parse_debug_dump_scd(char *topic, char *argv[], unsigned int argc);
void parse_system_identify_devices(char *topic, char *argv[], unsigned int argc);
void parse_dump_wifi(char *topic, char *argv[], unsigned int argc);
void parse_debug_dump_mqtt(char *topic, char *argv[], unsigned int argc);
void parse_debug_kill_mqtt(char *topic, char *argv[], unsigned int argc);
void parse_debug_kill_wifi(char *topic, char *argv[], unsigned int argc);

void parse_message(char *topic, char *omsg, unsigned int msgLength);

extern DeviceConfigurator deviceConfig;



struct msgCallbackList msgs[] = {
  { "SYSTEM.IDENTIFY.DEVICES", parse_system_identify_devices },

  { "DEVICE.CONFIG.WIFI", parse_device_config_wifi },
  { "DEVICE.CONFIG.NAME", parse_device_config_name },
  { "DEVICE.CONFIG.LOCATION", parse_device_config_location },
  { "DEVICE.CONFIG.MQTT", parse_device_config_mqtt },
  { "DEVICE.CONFIG.CLEAR", parse_device_config_clear },

  { "DEVICE.REBOOT", parse_device_reboot },

  { "DEVICE.MQTT.SUBSCRIBE", parse_device_mqtt_subscribe },

  { "DEBUG.DUMP.CONFIG", parse_debug_dump_config },
  { "DEBUG.DUMP.RUNNINGCONFIG", parse_debug_dump_runningconfig },
  { "DEBUG.DUMP.SCD", parse_debug_dump_scd },
  { "DEBUG.DUMP.WIFI", parse_dump_wifi },
  { "DEBUG.DUMP.MQTT", parse_debug_dump_mqtt },

  { "DEBUG.GET.SENSOR", parse_get_sensor },

  { "DEBUG.KILL.WIFI", parse_debug_kill_wifi },
  { "DEBUG.KILL.MQTT", parse_debug_kill_mqtt },

  { "GET.SENSOR", parse_get_sensor },
  { "GET.STATE", parse_config_stub },
  { "SET.STATE", parse_config_stub },

  { NULL, NULL },
};

void parse_debug_kill_mqtt(char *topic, char *argv[], unsigned int argc) {
  serial_printf("Killing MQTT!\n");
  client.disconnect();
}

void parse_debug_kill_wifi(char *topic, char *argv[], unsigned int argc) {
  serial_printf("Killing WiFi!\n");
  WiFi.disconnect();
}

void parse_debug_dump_mqtt(char *topic, char *argv[], unsigned int argc) {
  msg_to_serial(MSG_INFO_MQTT_STATUS, deviceConfig.getMQTTBroker(), deviceConfig.getMQTTPort(), deviceConfig.getMQTTUser(), deviceConfig.getMQTTPass(), client.state());
}

void parse_system_identify_devices(char *topic, char *argv[], unsigned int argc) {
  msg_to_system(MSG_DEVICE_IDENTIFY, deviceConfig.getDeviceName(), deviceConfig.getDeviceLocation(), VERSION, BUILD_DATE, WiFi.macAddress().c_str());
}

void parse_config_stub(char *topic, char *argv[], unsigned int argc) {

}
void parse_debug_dump_scd(char *topic, char *argv[], unsigned int argc) {
  sensors_dump_scd_list();
}

void parse_device_reboot(char *topic, char *argv[], unsigned int argc) {
  ESP.restart();
}

void parse_dump_wifi(char *topic, char *argv[], unsigned int argc) {
  serial_printf("WiFi status %d, IP %s\n", WiFi.status(), WiFi.localIP().toString().c_str());
}

void parse_debug_dump_config(char *topic, char *argv[], unsigned int argc) {
  deviceConfig.dumpConfiguration(deviceConfig.readStartupConfiguration());
}

void parse_debug_dump_runningconfig(char *topic, char *argv[], unsigned int argc) {
  deviceConfig.dumpConfiguration();
}

void parse_get_sensor(char *topic, char *argv[], unsigned int argc) {
  if (argc != 2) {
    msg_to_serial(MSG_ERROR_INCORRECT_PARAM_COUNT, 2, argc, "DEVICE.SENSOR.GET [SENSOR_NAME]");
    return;
  }

  struct sensorControlData *scd = sensor_get_by_name(argv[1]);

  if (!scd) {
    msg_to_serial(MSG_ERROR_SENSOR_NOT_FOUND, argv[1]);
  }

  if (scd->isEnabled) sensor_read(scd);

  publish_sensor(scd);

  if (IS_SERIAL_TOPIC(topic)) {
    msg_to_serial(MSG_UPDATE_SENSOR, argv[1], scd->currentData);
  } 

  return;
}

void parse_device_config_clear(char *topic, char *argv[], unsigned int argc) {
  deviceConfig.saveDefaultConfiguration();

  return;
}

void parse_device_mqtt_subscribe(char *topic, char *argv[], unsigned int argc) {
  if (argc != 2) {
    msg_to_serial(MSG_ERROR_INCORRECT_PARAM_COUNT, 3, argc, "DEVICE.MQTT.SUBSCRIBE [TOPIC]");
    return;
  }

  client.subscribe(argv[1]);

  return;
}

void parse_device_config_wifi(char *topic, char *argv[], unsigned int argc) {
  if (argc != 3) {
    msg_to_serial(MSG_ERROR_INCORRECT_PARAM_COUNT, 3, argc, "DEVICE.CONFIG.WIFI [SSID] [PSK]");
    return;
  }

  deviceConfig.setWiFiParameters(argv[1], argv[2]);
  deviceConfig.saveConfiguration();

  wifi_connect();

  return;
}

void parse_device_config_mqtt(char *topic, char *argv[], unsigned int argc) {
  // DEVICE.CONFIG.MQTT
  if (argc != 5) {
    msg_to_serial(MSG_ERROR_INCORRECT_PARAM_COUNT, 5, argc, argv[argc-1]);//"DEVICE.CONFIG.MQTT [BROKER] [PORT] [USER] [PASS]");
    return;
  }

  deviceConfig.setMQTTParameters(argv[1], atoi(argv[2]), argv[3], argv[4]);
  deviceConfig.saveConfiguration();

  mqtt_connect();

  return;
}

void parse_device_config_name(char *topic, char *argv[], unsigned int argc) {
  if (argc != 2) {
    msg_to_serial(MSG_ERROR_INCORRECT_PARAM_COUNT, 2, argc, "DEVICE.CONFIG.NAME [NAME]");
    return;
  }

  deviceConfig.setDeviceName(argv[1]);  
  deviceConfig.saveConfiguration();

  return;
}

void parse_device_config_location(char *topic, char *argv[], unsigned int argc) {
  if (argc != 2) {
    msg_to_serial(MSG_ERROR_INCORRECT_PARAM_COUNT, 2, argc, "DEVICE.CONFIG.LOCATION [LOCATION]");
    return;
  }

  deviceConfig.setDeviceLocation(argv[1]);  
  deviceConfig.saveConfiguration();

  return;
}

void parse_message(char *topic, char *omsg, unsigned int msgLength) {
  char *argv[32];
  unsigned int argc = 0;
  char *msg, *msgstart;

  msgstart = msg = (char *) malloc(msgLength + 1);
  memcpy(msg, omsg, msgLength);
  msg[msgLength] = '\0';

  argv[0] = strtok(msg, " ");
  msg = strtok(NULL, " ");

  while (msg != NULL && argc < 32) {
    argv[++argc] = msg;
    msg = strtok(NULL, " ");
  }
  
  for (unsigned int i = 0; msgs[i].command != NULL; i++) {
    if (!strcmp(msgs[i].command, argv[0])) {
      msgs[i].callback(topic, argv, argc + 1);
      free(msgstart);
      return;
    }
  }

  if (IS_SERIAL_TOPIC(topic)) {
    msg_to_serial(MSG_ERROR_COMMAND_NOT_FOUND, argv[0]);
  }

  free(msgstart);
  return;
}

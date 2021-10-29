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
//#include "eeprom.h"
#include "tasks.h"
#include "sensor_htu31.h"
#include "sensor_bme280.h"

void eeprom_save_config();
int eeprom_load_config();
void eeprom_init();
struct deviceConfiguration *eeprom_get_config();
uint32_t eeprom_get_crc32(struct deviceConfiguration *d);
void eeprom_dump_config(struct deviceConfiguration *d);

CRC32 crc32;
struct deviceConfiguration device;

struct deviceConfiguration device_default_config = {
  VERSION, 
  CONFIG_VERSION, 
  "unnamed",          // name
  "nolocation",       // location
  "deviceconfig",     // wifi_ssid
  "deviceconfig",     // wifi_psk
  "mqtt.sensorconfig.local",  // mqtt_broker
  "default",                  // mqtt_user
  "default",                  // mqtt_pass
  1833,                       // mqtt_port
  0
};


void eeprom_init() {
  EEPROM.begin(EEPROM_SIZE);
  eeprom_load_config();
}

void eeprom_save_default_config() {
  memcpy((void *) &device, (void *) &device_default_config, sizeof(device));
  eeprom_save_config();
}

void eeprom_save_config() {
  uint32_t newcrc;
  strcpy((char *) &device.version, VERSION);
  device.config_version = CONFIG_VERSION;
  newcrc = eeprom_get_crc32(&device);
  device.crc32 = newcrc;
  
  EEPROM.put(EEPROM_DEVICE_CONFIG_OFFSET, device);

  if (!EEPROM.commit()) {
    serial_printf("EEPROM.commit() failed!\n");
    return;
  }
  serial_printf("Saved configuration\n");
}

int eeprom_load_config() {
  EEPROM.get(EEPROM_DEVICE_CONFIG_OFFSET, device);
  uint32_t crc = eeprom_get_crc32(&device);
  if (crc != device.crc32) {
    serial_printf("Error: CRC32 from device %d does not match calculated crc %d\n", device.crc32, crc);
    return(-ECONFIG_INVALID_CRC);
  }
  else {
    serial_printf("CRC32 matches from EEPROM %d %d\n", device.crc32, crc);
  }
  return(0);
}

deviceConfiguration *eeprom_get_config() {
  static deviceConfiguration d;
  EEPROM.get(EEPROM_DEVICE_CONFIG_OFFSET, d);
  uint32_t crc = eeprom_get_crc32(&d);
  if (crc != d.crc32) {
    serial_printf("Error: CRC32 from device %d does not match calculated crc %d\n", d.crc32, crc);
  }
  else {
    serial_printf("CRC32 matches from EEPROM %d %d\n", d.crc32, crc);
  }
  return(&d);
}

void eeprom_update_config() {
  deviceConfiguration *d = eeprom_get_config();
  uint8_t *o, *n;
  o = (byte *) d;
  n = (byte *) &device;
  for (unsigned int i = 0; i < sizeof(device); i++) {
    if (*o != *n) {
      EEPROM.write(EEPROM_DEVICE_CONFIG_OFFSET + i, *n);
    }
    ++o;++n;
  }
  if (!EEPROM.commit()) {
    serial_printf("EEPROM.commit() failed!\n");
    return;
  }

}

void eeprom_dump_config(struct deviceConfiguration *d) {
  serial_printf("Version: %s, %d\n", d->version, d->config_version);
  serial_printf("Name: %s, Location: %s\n", d->name, d->location);
  serial_printf("SSID: %s, PSK: %s\n", d->wifi_ssid, d->wifi_psk);
  serial_printf("MQTT Server: %s:%d\n", d->mqtt_broker, d->mqtt_port);
  serial_printf("MQTT User/pass: %s %s\n", d->mqtt_user, d->mqtt_pass);
  serial_printf("CRC32: %d\n", d->crc32);
}

uint32_t eeprom_get_crc32(struct deviceConfiguration *d) {
  uint32_t crc;
  serial_printf("Calculate on %p, %d - %d (%d)\n", &device, sizeof(deviceConfiguration), sizeof(device.crc32), sizeof(deviceConfiguration) - sizeof(device.crc32));
  crc = CRC32::calculate((uint8_t *) &device, sizeof(deviceConfiguration) - sizeof(device.crc32));
  return(crc);
}

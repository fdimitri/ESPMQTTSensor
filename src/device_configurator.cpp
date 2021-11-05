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
#include "sensor_htu31.h"
#include "sensor_bme280.h"


using namespace std;

deviceConfiguration device_default_config = {
  0x434f4e46,
  VERSION, 
  CONFIG_VERSION, 
  "unnamed", 
  "nolocation",       // location
  {
    "deviceconfig", 
    "deviceconfig" 
  },
  { 
    "mqtt.sensorconfig.local",  // mqtt.broker
    "default",                  // mqtt.user
    "default",                  // mqtt.pass
    1833                       // mqtt.port
  },
  0
};

DeviceConfigurator::DeviceConfigurator() {
    this->device = (deviceConfiguration *) malloc(sizeof(deviceConfiguration));
}

DeviceConfigurator::~DeviceConfigurator(void) {
    free(this->device);
}

void DeviceConfigurator::begin() {
  int e;
  if ((e = loadStartupConfiguration()) < 0) {
      serial_printf("Return from loadStartupConfig was %d\n", e);
      isValidConfig = false;
      memset(this->device, 0, sizeof(deviceConfiguration));
  }
  else {
      isValidConfig = true;
  }
}

void DeviceConfigurator::saveDefaultConfiguration() {
    memcpy((void *) this->device, (void *) &device_default_config, sizeof(device));
    saveConfiguration();
}

void DeviceConfigurator::saveConfiguration() {
  strcpy((char *) this->device->version, VERSION);
  this->device->config_version = CONFIG_VERSION;

  uint32_t newcrc = getCRC(this->device);

  this->device->crc32 = newcrc;
#ifdef ESP32  
  EEPROM.writeBytes(EEPROM_DEVICE_CONFIG_OFFSET, this->device, sizeof(this->device));
#endif
#ifdef ESP8266
  EEPROM.put(EEPROM_DEVICE_CONFIG_OFFSET, (deviceConfiguration) *this->device);
#endif
  if (!EEPROM.commit()) {
    serial_printf("EEPROM.commit() failed!\n");
    return;
  }
  serial_printf("Saved configuration\n");  
}

uint32_t DeviceConfigurator::getCRC(deviceConfiguration *d) {
  uint32_t crc;

  crc = CRC32::calculate((uint8_t *) d, sizeof(deviceConfiguration) - sizeof(d->crc32));

  return(crc);
}

deviceConfiguration *DeviceConfigurator::readStartupConfiguration() {
  static deviceConfiguration d;
#ifdef ESP32
  EEPROM.readBytes(EEPROM_DEVICE_CONFIG_OFFSET, &d, sizeof(deviceConfiguration));
#endif
#ifdef ESP8266
  EEPROM.get(EEPROM_DEVICE_CONFIG_OFFSET, d);
#endif
  uint32_t crc = getCRC(&d);

  if (crc != d.crc32) {
    serial_printf("Error: CRC32 from device %x does not match calculated crc %x\n", d.crc32, crc);
  }
  else {
    serial_printf("CRC32 matches from EEPROM %x %x\n", d.crc32, crc);
  }

  return(&d);
}

int DeviceConfigurator::loadStartupConfiguration() {
    deviceConfiguration *d = readStartupConfiguration();
  
    if (getCRC(d) != d->crc32) {
        return(-ECONFIG_INVALID_CRC);
    }
  
    memcpy((void *) this->device, (void *) readStartupConfiguration(), sizeof(deviceConfiguration));

    return(0);
}

void DeviceConfigurator::setMQTTParameters(char *broker, unsigned int port, char *user, char *pass) {
  this->device->mqtt.port = port;

  memset((void *) this->device->mqtt.broker, 0, sizeof(this->device->mqtt.broker));
  memset((void *) this->device->mqtt.user, 0, sizeof(this->device->mqtt.user));
  memset((void *) this->device->mqtt.pass, 0, sizeof(this->device->mqtt.pass));

  strncpy((char *) this->device->mqtt.broker, broker, sizeof(this->device->mqtt.broker) - 1);
  strncpy((char *) this->device->mqtt.user, user, sizeof(this->device->mqtt.user) - 1);
  strncpy((char *) this->device->mqtt.pass, pass, sizeof(this->device->mqtt.pass) - 1);
}

void DeviceConfigurator::setWiFiParameters(char *ssid, char *psk) {
  memset((void *) this->device->wifi.ssid, 0, sizeof(this->device->wifi.ssid));
  memset((void *) this->device->wifi.psk, 0, sizeof(this->device->wifi.psk));
  
  strncpy((char *) this->device->wifi.ssid, ssid, sizeof(this->device->wifi.ssid) - 1);
  strncpy((char *) this->device->wifi.psk, psk, sizeof(this->device->wifi.psk) - 1);
}

const char *DeviceConfigurator::getWifiSSID() {
    return((const char *) &this->device->wifi.ssid[0]);
}

const char *DeviceConfigurator::getWifiPSK() {
    return((const char *) &this->device->wifi.psk[0]);
}

const char *DeviceConfigurator::getMQTTBroker() {
    return((const char *) &this->device->mqtt.broker[0]);
}

const char *DeviceConfigurator::getMQTTUser() {
    return((const char *) &this->device->mqtt.user[0]);
}

const char *DeviceConfigurator::getMQTTPass() {
    return((const char *) &this->device->mqtt.pass[0]);
}

const char *DeviceConfigurator::getDeviceLocation() {
    return((const char *) &this->device->location[0]);
}

const char *DeviceConfigurator::getDeviceName() {
    return((const char *) &this->device->name[0]);
}

int DeviceConfigurator::getMQTTPort() {
    return(this->device->mqtt.port);
}

void DeviceConfigurator::setDeviceLocation(char *location) {
    memset((void *) this->device->location, 0, sizeof(this->device->location));
    strncpy(this->device->location, location, sizeof(this->device->location) - 1);
}

void DeviceConfigurator::setDeviceName(char *name) {
    memset((void *) this->device->name, 0, sizeof(this->device->name));
    strncpy(this->device->name, name, sizeof(this->device->name) - 1);
}

void DeviceConfigurator::dumpConfiguration(deviceConfiguration *d) {
  serial_printf("Version: %s, %d\n", d->version, d->config_version);
  serial_printf("Name: %s, Location: %s\n", d->name, d->location);
  serial_printf("SSID: %s, PSK: %s\n", d->wifi.ssid, d->wifi.psk);
  serial_printf("MQTT Server: %s:%d\n", d->mqtt.broker, d->mqtt.port);
  serial_printf("MQTT User/pass: %s %s\n", d->mqtt.user, d->mqtt.pass);
  serial_printf("CRC32: %x\n", d->crc32);
}


void DeviceConfigurator::dumpConfiguration() {
  deviceConfiguration *d = this->device;
  dumpConfiguration(d);
}


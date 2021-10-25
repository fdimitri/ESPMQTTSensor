# ESP MQTT Sensors Project

## Overview
* All communication over MQTT
* ESP-8266/ESP-32 boards for microcontroller related tasks
* Raspberry Pi Zero W or equivalent for things requiring more horsepower (door camera, etc.)
* Sensors
 * Temp/Humidity (out/indoor)
 * Sound level
 * Light level (out/indoor)
 * Door open
 * Window open
 * Pressure switch (bed)
 * AC voltage/current
 * BLE device distance (fob detection)
* Control Modules
 * Thermostat zone control
 * Window fan control
 * AC Switch
 * Light dimming switch
 * Door ring camera
 * Door lock
 * Garage door activation


## Communications
All primary communications are done over MQTT. Devices shall be configured over serial initially if the default configuration does not work.

The serial port also parses messages as if they came in on MQTT. All serial messages are parsed as if coming from the server (Device to Server).

JSON was decided to be too expensive for an ESP8266.

Parameters in [] are mandatory, parameters in [[]] are optional.

### Topic Structure
Devices shall publish sensors to home/[LOCATION]/devices/[DEVICENAME]
Devices shall subscribe to topic system/[DEVICENAME] and home/[LOCATION]/system

### Server to Device
DEVICE.CONFIG.WIFI [[wifiSSID]] [[wifiPassword]] [deviceMAC]
Configures WiFi parameters

DEVICE.CONFIG.MQTT [[Broker]] [[BrokerPort]] [[Username]] [[Password]] [deviceMAC]
Configures MQTT parameters

DEVICE.CONFIG.NAME [[newDeviceName]]
Sets device’s friendly name

DEVICE.CONFIG.LOCATION [[deviceLocation]]
Sets device’s location

DEVICE.CONFIG.SENSOR.PUBLISHINTERVAL [[sensorName]] [[milliSeconds]]

DEVICE.CONFIG.SENSOR.UPDATEINTERVAL [[sensorName]] [[milliSeconds]]

GET.SENSORS [[deviceName]] [sensorName]

SET.STATE [[deviceName]] [[controlName]]

GET.STATE [[deviceName]] [[controlName]] [[state]]

### Debug Information - Serial Only
Any of these commands will dump information to the serial console, and NOT over MQTT.

DEBUG.CONFIG.DUMP
Dumps configuration

DEBUG.GET.SENSOR
Prints the current value of the sensor

### Device to Server

UPDATE.SENSOR [[deviceName]] [[sensorName]] [[sensorValue]]

SYSTEM.INIT.DEVICE [[deviceName]] [[deviceMAC]] [[deviceIP]]
Registers device to server

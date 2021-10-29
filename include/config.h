#undef ESP8266
#undef ESP32

#define ESP32

#define SSD1306_128_32

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define BUFFER_SIZE 256

#define EEPROM_SIZE 3072
#define EEPROM_DEVICE_CONFIG_OFFSET 0x0

#define CONFIG_VERSION 2
#define BUILD_DATE "202110290041"
#define VERSION "0.0.4"


#define CONFIG_HARDWARE_HTU31
#define CONFIG_HARDWARE_BME280
#define CONFIG_HARDWARE_ESP8266ADC

#ifdef CONFIG_HARDWARE_HTU31
#include <Adafruit_HTU31D.h>
#endif

#ifdef CONFIG_HARDWARE_BME280
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#endif

#ifdef ESP32
#include <WiFi.h>
#endif
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif

#ifdef ESP8266
#include <ESP_EEPROM.h>
#endif

#define CONFIG_IIC_SPEED 1000000L
#define CONFIG_SERIAL_COMMANDBUFFER_SIZE 256
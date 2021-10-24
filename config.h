#define ESP8266
#undef ESP32
#ifndef STASSID
#define STASSID "TellMyWiFiFoundSomeoneWhos24"
#define STAPSK  "TwinTurbo2.7"
#endif
#define SSD1306_128_32

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define BUFFER_SIZE 256

#define EEPROM_SIZE 4096
#define EEPROM_DEVICE_CONFIG_OFFSET 0x0

#define CONFIG_VERSION 1
#define VERSION "202110231938"

// MQTT Broker
const char *topic = "device/unnamed";

struct deviceConfiguration device;

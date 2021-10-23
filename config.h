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

// WiFi
const char *ssid = STASSID; // Enter your WiFi name
const char *password = STAPSK;  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "192.168.0.133";
const char *topic = "device/unnamed";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

struct deviceConfiguration device {
  "un-named device", "8c:aa:b5:85:ef:94", 0
};

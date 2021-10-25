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
#include "eeprom.h"
#include "tasks.h"
#include "sensor_htu31.h"
#include "sensor_bme280.h"

#ifdef ESP32
#include <WiFi.h>
#endif
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
CRC32 crc32;

void setup() {
  char msgbuf[256];
  Serial.begin(115200);
  #ifdef ESP32
  Wire.begin();
  #endif
  #ifdef ESP8266
  Wire.begin(2,0);
  #endif

  eeprom_init();
  int config_load_result = eeprom_load_config();
  eeprom_dump_config(eeprom_get_config());

  if (config_load_result < 0 || device.wifi_ssid[1] == 0xFF) {
    serial_printf("Device not configured or CRC32 invalid, loading default configuration");
    memcpy((void *) &device, (void *) &device_default_config, sizeof(device));
    eeprom_save_config();
  }

  Serial.println("Initializing display..");
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  oled_init();

  oled_printf("Starting up!\nWiFi SSD:\n%s", device.wifi_ssid);

  WiFi.begin(device.wifi_ssid, device.wifi_psk);
  serial_printf("Connecting to %s with PSK %s", device.wifi_ssid, device.wifi_psk);

  unsigned int i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 100) {
      oled_print(".");
      Serial.print(".");
      delay(500);
      task_read_serial();
  }
  
  Serial.println("Connected to the WiFi network");
  oled_printf("\nConnected!");
  
  delay(2000);

  display.clearDisplay();
  display.setCursor(0, 0); 
  
 
  sensors_init();

  serial_printf("Connecting to MQTT..\n");
  oled_printf("MQTT on port %d\n%s", device.mqtt_port, device.mqtt_broker);

  mqtt_connect();
  delay(2000);
  char topicbuf[128];
  client.subscribe("system/#");
  client.subscribe(mqtt_build_topic((char *) &topicbuf));
}

void loop() {
  client.loop(); // MQTT Client Loop in library
  for (unsigned int i = 0; tasks[i].func != NULL; i++) {
    if (!tasks[i].run_every_millis || millis() - tasks[i].last_run_time >= tasks[i].run_every_millis) {
      tasks[i].last_run_time = millis();
      tasks[i].func();
    }
  }
  yield();
}

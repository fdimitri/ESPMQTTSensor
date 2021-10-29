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
// #include "display.h"
#include "mqtt.h"
#include "serial.h"
#include "project_eeprom.h"
#include "tasks.h"
#include "sensor_htu31.h"
#include "sensor_bme280.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
void oled_msgLn(char *str, unsigned int length);
void oled_msg(const char *str, unsigned int length);
void oled_printf(const char *fmt, ...);
void oled_init(void);
void oled_print(const char *msg);

void oled_msg(const char *str, unsigned int length) {
  for(unsigned int i=0; i<length; i++) {
    display.write(str[i]);
  }
  display.display();
}

void oled_msgLn(char *str, unsigned int length) {
  oled_msg(str, length);
  display.write("\n");
}

void oled_init(void) {
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
}

void oled_printf(const char *fmt, ...) {
  char buf[256];
  memset((void *) buf, 0, sizeof(buf));
  va_list args;
  va_start(args, fmt);
  vsprintf(buf, fmt, args);
  va_end(args);
  oled_msg(buf, strlen(buf));
  return;
}


void oled_print(const char *msg) {
  oled_msg(msg, strlen(msg));
  return;
}

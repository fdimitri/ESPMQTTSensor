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
#include "mqtt.h"
// #include "serial.h"
#include "tasks.h"
#include "sensor_htu31.h"
#include "sensor_bme280.h"

void serial_read(void);
void serial_printf(const char *fmt, ...);

char serial_command_buffer[CONFIG_SERIAL_COMMANDBUFFER_SIZE];
uint8_t serial_command_buffer_ptr = 0;

void serial_read(void) {
  while (Serial.available() > 0) {
    uint8_t c = Serial.read();
    Serial.print((char) c);
    if (serial_command_buffer_ptr > CONFIG_SERIAL_COMMANDBUFFER_SIZE -1) {
      serial_printf("Serial input exceeded length!\n");
      serial_command_buffer_ptr = 0;
    }
    switch (c) {
      case '\r':
      case '\n':
        if (strlen(serial_command_buffer)) {
          parse_message("serial", (char *) &serial_command_buffer, strlen(serial_command_buffer));
          serial_command_buffer_ptr = 0;
        }
        memset((void *) &serial_command_buffer, 0, sizeof(serial_command_buffer));
        break;
      case '\b':
        serial_command_buffer_ptr--;
        serial_command_buffer[serial_command_buffer_ptr] = 0;
        break;
      default:
        serial_command_buffer[serial_command_buffer_ptr++] = c;  
    }
  }
}

void serial_printf(const char *fmt, ...) {
  char buf[BUFFER_SIZE];
  memset((void *) buf, 0, sizeof(buf));
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, BUFFER_SIZE, fmt, args);
  va_end(args);
  Serial.print(buf);
  Serial.flush();
  return;
}

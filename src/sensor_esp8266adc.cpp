#include "config.h"
#ifdef CONFIG_HARDWARE_ESP8266ADC
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
#include "parse.h"
#include "sensors.h"
#include "display.h"
#include "mqtt.h"
#include "serial.h"
#include "tasks.h"
#include "sensor_htu31.h"
#include "sensor_bme280.h"

int adc8266_init();
int adc8266_register_functions();
void adc8266_read_voltage(void *customData, char *buf);

#define ESP8266ADC_R1 1000000
#define ESP8266ADC_R2 100000

// ADC_MODE(ADC_VCC);

int adc8266_init() {
    adc8266_register_functions();  
    return(0);
}

int adc8266_register_functions() {
    struct sensorControlData *cptr;

    serial_printf("Allocating ram for cptr\n");
    cptr = sensor_definition_allocate("system.batteryVoltage", "adc8266", adc8266_read_voltage, NULL, NULL, 0x0, NULL);
    sensor_definition_register(cptr);

    return(0);
}


void adc8266_read_voltage(void *customData, char *buf) {
    float sensor_value;
    float sensor_divider = (ESP8266ADC_R1 + ESP8266ADC_R2) / ESP8266ADC_R2;
    sensor_value = sensor_divider * 3 * analogRead(A0);
    
    //sensor_value = sensor_value * sensor_divider;
    //sensor_value *= sensor_divider;

    sprintf(buf, "%f", sensor_value);
}

#endif

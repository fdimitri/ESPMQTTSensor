void sensor_get_stub(char *buf) {
  sprintf(buf, "%f", 3.2);
}

void sensor_get_uptime(char *buf) {
  #ifdef ESP32
  sprintf(buf, "%lld", esp_timer_get_time());
  #endif
  #ifdef ESP8266
  sprintf(buf, "%lld", system_get_time());
  #endif
}

void sensor_get_free_heap(char *buf) {
  sprintf(buf, "%d", ESP.getFreeHeap());
}

void sensor_get_total_heap(char *buf) {
  #ifdef ESP32
  sprintf(buf, "%d", ESP.getHeapSize());
  #endif
}

void sensor_get_rssi(char *buf) {
  long rssi = WiFi.RSSI();
  sprintf(buf, "%ld", rssi);
}

int sensor_definition_register_functions(struct sensorControlData *newscd) {
  struct sensorControlData *scd;
  serial_printf("sensor_definition_register_functions() called\n");
  scd = sensor_get_by_name(newscd->sensorName);
  if (!scd) {
    serial_printf("sensor_definition.. not found %s\n", newscd->sensorName);
    return(-ESENSOR_NOT_FOUND_IN_SCD);  
  }
  serial_printf("Found scd: %p", scd);
  sensors_dump_scd_entry(scd);

  scd->isEnabled = true;
  serial_printf("Assigning functions..\n");
  scd->readSensor = newscd->readSensor;
  scd->getState = newscd->getState;
  scd->setState = newscd->setState;
 
  serial_printf("sizeof(scd->sensorHardware): %d\n", sizeof(scd->sensorHardware));
  memcpy(newscd->sensorHardware, scd->sensorHardware, sizeof(scd->sensorHardware));
  serial_printf("sensor_definition_register_functions() completed\n");
  return(0);
}

struct sensorControlData *sensor_get_by_name(char *name) {
  for (unsigned int i = 0; sensors[i].sensorFlags != 0x8000; i++) {
    if (!strcmp(sensors[i].sensorName, name)) {
      return(&sensors[i]);
    }
  }
  return(NULL);
}

int sensors_init() {
  #ifdef CONFIG_HARDWARE_HTU31
  htu31_init();
  #endif

  #ifdef CONFIG_HARDWARE_BME280
  bme280_init();
  #endif
  return(0);
}

int sensors_dump_scd_entry(struct sensorControlData *d) {
  serial_printf("-------------------------\n");
  serial_printf("Enabled: %d, Name: %s, hardware: %s\n", d->isEnabled, d->sensorName, d->sensorHardware);
  serial_printf("Currentdata: %s, flags: %ld\n", d->currentData, d->sensorFlags);
  serial_printf("readSensor: %p, getState: %p, setState: %p\n", d->readSensor, d->getState, d->setState);
  serial_printf("-------------------------\n");
  return(0);
}

int sensors_dump_scd_list() {
  char buf[1024];
  for (unsigned int i = 0; sensors[i].sensorFlags != 0x8000; i++) {
    serial_printf("Sensor infor for %d:\n", i);
    sensors_dump_scd_entry(&sensors[i]);
  }
  return(0);
}

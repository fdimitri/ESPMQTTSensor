
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

void task_update_sensors(void *pvParameters) {
  (void) pvParameters;
  char buf[256];
  for (unsigned int i = 0; sensors[i].sensorName != NULL; i++) {
//    sprintf((char *) &buf, "Going through sensor %d - %s, flags %d - %s", i, sensors[i].sensorName, sensors[i].sensorFlags, sensors[i].currentData);
//    Serial.println(buf);
    if (!sensors[i].isEnabled) {
      continue;
    }
    if (sensors[i].sensorFlags == 0 && sensors[i].readSensor) {
      sensors[i].readSensor((char *) &sensors[i].currentData);
    }
    if (sensors[i].sensorFlags == 1 && sensors[i].getState) {
      sensors[i].getState((char *) &sensors[i].currentData);
    }
    
    publish_sensor((struct sensorControlData *) &sensors[i]);
  }
}

void publish_sensor(struct sensorControlData *sensor) {
  char buffer[128];
  sprintf((char *) &buffer, "UPDATE.SENSOR %s %s", sensor->sensorName, sensor->currentData);
  client.publish(topic, (char *) &buffer);
}

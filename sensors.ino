
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

void publish_sensor(struct sensorControlData *sensor) {
  char buffer[128];
  sprintf((char *) &buffer, "UPDATE.SENSOR %s %s", sensor->sensorName, sensor->currentData);
  client.publish(topic, (char *) &buffer);
}

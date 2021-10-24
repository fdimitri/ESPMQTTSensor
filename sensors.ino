
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

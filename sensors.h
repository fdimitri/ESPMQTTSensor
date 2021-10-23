void sensor_get_stub(char *);
void sensor_get_free_heap(char *);
void sensor_get_total_heap(char *);
void sensor_get_uptime(char *);
void publish_sensor(struct sensorControlData *sensor);
void task_update_sensors(void *pvParameters);

struct sensorControlData sensors[] = {
  { false, "comfort.temperature", "", 0, sensor_get_stub, NULL, NULL },
  { false, "comfort.relativeHumidity", "", 0, sensor_get_stub, NULL, NULL },
  { false, "comfort.lightLevel", "", 0, sensor_get_stub, NULL, NULL },
  { false, "comfort.noiseLevel", "", 0, sensor_get_stub, NULL, NULL },
  { true, "system.wifi.RSSI", "", 0, sensor_get_stub, NULL, NULL, },
  { true, "system.uptime", "", 0, sensor_get_uptime, NULL, NULL, },
  { true, "system.battery.voltage", "", 0, sensor_get_stub, NULL, NULL, },
  { true, "system.memory.free", "", 0, sensor_get_free_heap, NULL, NULL },
  { true, "system.memory.total", "", 0, sensor_get_total_heap, NULL, NULL },
  { false, "comfort.fan1", "", 1, NULL, sensor_get_stub, sensor_get_stub },
  { false, "comfort.fan2", "", 1, NULL, sensor_get_stub, sensor_get_stub },
  { false, "comfort.dimmer", "", 1, NULL, sensor_get_stub, sensor_get_stub },
  { false, "comfort.relay", "", 1, NULL, sensor_get_stub, sensor_get_stub },
  { false, "system.battery.coulumbCounter", "", 0, sensor_get_stub, NULL, NULL },
  { false, "comfort.pirMotionDetector", "", 0, sensor_get_stub, NULL, NULL },
  { false, NULL, "", 0xF000, NULL, NULL, NULL },
};

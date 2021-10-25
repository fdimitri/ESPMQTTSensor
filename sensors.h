void sensor_get_stub(char *);
void sensor_get_free_heap(char *);
void sensor_get_total_heap(char *);
void sensor_get_uptime(char *);
void sensor_get_rssi(char *buf);
int sensor_definition_register_functions(struct sensorControlData *scd);
int sensors_init();
int sensors_dump_scd_entry(struct sensorControlData *d);
int sensors_dump_scd_list();
struct sensorControlData *sensor_get_by_name(char *name);

struct sensorControlData sensors[] = {
  { false, "comfort.temperature", "", "", 0, sensor_get_stub, NULL, NULL },
  { false, "comfort.relativeHumidity", "", "", 0, sensor_get_stub, NULL, NULL },
  { false, "comfort.lightLevel", "", "", 0, sensor_get_stub, NULL, NULL },
  { false, "comfort.noiseLevel", "", "", 0, sensor_get_stub, NULL, NULL },
  { true, "system.wifi.RSSI", "system", "", 0, sensor_get_rssi, NULL, NULL, },
  { true, "system.uptime", "", "system", 0, sensor_get_uptime, NULL, NULL, },
  { true, "system.battery.voltage", "", "system", 0, sensor_get_stub, NULL, NULL, },
  { true, "system.memory.free", "", "system", 0, sensor_get_free_heap, NULL, NULL },
  { true, "system.memory.total", "", "system", 0, sensor_get_total_heap, NULL, NULL },
  { false, "comfort.fan1", "", "", 1, NULL, sensor_get_stub, sensor_get_stub },
  { false, "comfort.fan2", "", "", 1, NULL, sensor_get_stub, sensor_get_stub },
  { false, "comfort.dimmer", "", "", 1, NULL, sensor_get_stub, sensor_get_stub },
  { false, "comfort.relay", "", "", 1, NULL, sensor_get_stub, sensor_get_stub },
  { false, "system.battery.coulumbCounter", "", "", 0, sensor_get_stub, NULL, NULL },
  { false, "comfort.pirMotionDetector", "", "", 0, sensor_get_stub, NULL, NULL },
  { false, "", "", "", 0x8000, NULL, NULL, NULL },
};

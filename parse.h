void parse_system_init_device(char *, char *[], unsigned int);
void parse_config_stub(char *, char *[], unsigned int);
void parse_device_config_name(char *, char *[], unsigned int);
void parse_device_config_location(char *, char *[], unsigned int);
void parse_device_reboot(char *, char *[], unsigned int);
void parse_device_config_dump(char *topic, char *argv[], unsigned int argc);
void parse_device_rconfig_dump(char *topic, char *argv[], unsigned int argc);
void parse_device_config_wifi(char *topic, char *argv[], unsigned int argc);
void parse_device_config_mqtt(char *topic, char *argv[], unsigned int argc);
void parse_device_config_clear(char *topic, char *argv[], unsigned int argc);
void parse_device_mqtt_subscribe(char *topic, char *argv[], unsigned int argc);

struct msgCallbackList msgs[] = {
  { "SYSTEM.INIT.DEVICE", parse_system_init_device },
  { "SYSTEM.INIT.DEVICE.SENSOR", parse_system_init_device },
  { "SYSTEM.INIT.DEVICE.CONTROL", parse_system_init_device },
  { "DEVICE.CONFIG.WIFI", parse_device_config_wifi },
  { "DEVICE.CONFIG.NAME", parse_device_config_name },
  { "DEVICE.CONFIG.LOCATION", parse_device_config_location },
  { "DEVICE.CONFIG.MQTT", parse_device_config_mqtt },
  { "DEVICE.REBOOT", parse_device_reboot },
  { "DEVICE.CONFIG.DUMP", parse_device_config_dump },
  { "DEVICE.CONFIG.RDUMP", parse_device_config_dump },
  { "DEVICE.CONFIG.CLEAR", parse_device_config_clear },
  { "DEVICE.MQTT.SUBSCRIBE", parse_device_mqtt_subscribe },
  { "GET.SENSOR", parse_config_stub },
  { "UPDATE.SENSOR", parse_config_stub },
  { "GET.STATE", parse_config_stub },
  { "SET.STATE", parse_config_stub },
  { NULL, NULL },
};

extern void parse_system_init_device(char *, char *[], unsigned int);
extern void parse_config_stub(char *, char *[], unsigned int);
extern void parse_device_config_name(char *, char *[], unsigned int);
extern void parse_device_config_location(char *, char *[], unsigned int);
extern void parse_device_reboot(char *, char *[], unsigned int);
extern void parse_device_config_wifi(char *topic, char *argv[], unsigned int argc);
extern void parse_device_config_mqtt(char *topic, char *argv[], unsigned int argc);
extern void parse_device_config_clear(char *topic, char *argv[], unsigned int argc);
extern void parse_device_mqtt_subscribe(char *topic, char *argv[], unsigned int argc);
extern void parse_debug_config_dump(char *topic, char *argv[], unsigned int argc);
extern void parse_debug_rconfig_dump(char *topic, char *argv[], unsigned int argc);
extern void parse_debug_get_sensor(char *topic, char *argv[], unsigned int argc);

extern struct msgCallbackList msgs[];

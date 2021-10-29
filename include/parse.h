extern void parse_system_init_device(char *topic, char *argv[], unsigned int argc);
extern void parse_config_stub(char *topic, char *argv[], unsigned int argc);
extern void parse_device_config_name(char *topic, char *argv[], unsigned int argc);
extern void parse_device_config_location(char *topic, char *argv[], unsigned int argc);
extern void parse_device_reboot(char *topic, char *argv[], unsigned int argc);
extern void parse_device_config_wifi(char *topic, char *argv[], unsigned int argc);
extern void parse_device_config_mqtt(char *topic, char *argv[], unsigned int argc);
extern void parse_device_config_clear(char *topic, char *argv[], unsigned int argc);
extern void parse_device_mqtt_subscribe(char *topic, char *argv[], unsigned int argc);
extern void parse_debug_dump_config(char *topic, char *argv[], unsigned int argc);
extern void parse_debug_dump_runningconfig(char *topic, char *argv[], unsigned int argc);
extern void parse_get_sensor(char *topic, char *argv[], unsigned int argc);
extern void parse_debug_dump_scd(char *topic, char *argv[], unsigned int argc);
extern void parse_message(char *topic, char *omsg, unsigned int msgLength);


extern struct msgCallbackList msgs[];

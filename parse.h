void parse_system_init_device(char *, char *[], unsigned int);
void parse_config_stub(char *, char *[], unsigned int);
void parse_config_name(char *, char *[], unsigned int);
void parse_config_location(char *, char *[], unsigned int);

struct msgCallbackList msgs[] = {
  { "SYSTEM.INIT.DEVICE", parse_system_init_device },
  { "SYSTEM.INIT.DEVICE.SENSOR", parse_system_init_device },
  { "SYSTEM.INIT.DEVICE.CONTROL", parse_system_init_device },
  { "DEVICE.CONFIG.WIFI", parse_config_stub },
  { "DEVICE.CONFIG.NAME", parse_config_name },
  { "DEVICE.CONFIG.LOCATION", parse_config_location },
  { "GET.SENSORS", parse_config_stub },
  { "UPDATE.SENSOR", parse_config_stub },
  { "GET.STATE", parse_config_stub },
  { "SET.STATE", parse_config_stub },
  { NULL, NULL },
};

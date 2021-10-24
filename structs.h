struct sensorControlData {
  bool isEnabled;
  const char *sensorName;
  char currentData[16];
  uint32_t sensorFlags;
  void (*readSensor)(char *buf);
  void (*getState)(char *buf);
  void (*setState)(char *buf);
};

struct msgCallbackList {
  const char *command;
  void (*callback)(char *topic, char *argv[], unsigned int argc);
};

struct deviceConfiguration {
  char version[16];
  uint16_t config_version;
  
  char name[64];
  char location[64];
  
  char wifi_ssid[32];
  char wifi_psk[32];
  
  char mqtt_broker[32];
  char mqtt_user[32];
  char mqtt_pass[32];
  uint16_t mqtt_port;
  
  uint32_t crc32;
};

struct taskRun {
  const char *name;
  void (*func)();
  uint16_t run_every_millis;
  unsigned long last_run_time;
};

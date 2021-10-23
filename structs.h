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
  char name[64];
  char wifimac[18];
  char location[64];
  uint32_t ip;
};

struct taskRun {
  void (*func)();
  uint16_t run_every_millis;
  unsigned long last_run_time;
};

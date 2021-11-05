typedef void (*msgCallback)(char *topic, char *argv[], unsigned int argc);
typedef void (*scdCallback)(void *customData, char *buf);

struct sensorControlData {
  bool isEnabled;
  char sensorName[64];
  char sensorHardware[16];
  char currentData[16];
  uint32_t sensorFlags;
  scdCallback readSensor;
  scdCallback getState;
  scdCallback setState;
  void *customData;
  struct sensorControlData *next;
};

struct msgCallbackList {
  const char *command;
  msgCallback callback;
};

struct deviceConfigurationMQTT_v2 {
  char broker[32];
  char user[32];
  char pass[32];
  uint16_t port;
};

struct deviceConfigurationWiFi_v2 {
  char ssid[32];
  char psk[32];
};

struct deviceConfiguration_v3 {
  uint32_t magic;
  char version[16];
  uint16_t config_version;
  
  char name[64];
  char location[64];
  
  deviceConfigurationWiFi_v2 wifi;
  deviceConfigurationMQTT_v2 mqtt;

  uint32_t crc32;
};

struct deviceConfiguration_v2 {
  char version[16];
  uint16_t config_version;
  
  char name[64];
  char location[64];
  
  deviceConfigurationWiFi_v2 wifi;
  deviceConfigurationMQTT_v2 mqtt;

  uint32_t crc32;
};

typedef struct deviceConfiguration_v3 deviceConfiguration;

struct taskRun {
  const char *name;
  void (*func)();
  uint16_t run_every_millis;
  unsigned long last_run_time;
};

enum msg_to_system { 
  MSG_DEVICE_IDENTIFY,
  MSG_UPDATE_SENSOR,
  MSG_ERROR_INCORRECT_PARAM_COUNT = 0x8000,
  MSG_ERROR_SENSOR_NOT_FOUND,
  MSG_ERROR_COMMAND_NOT_FOUND,
  MSG_INFO_DEVICE_CONFIG_INVALID = 0x9000,
  MSG_INFO_MQTT_CONNECTING,
  MSG_INFO_MQTT_CONNECTED,
  MSG_INFO_MQTT_FAILED,
  MSG_INFO_MQTT_STATUS,
  MSG_INFO_WIFI_CONNECTING,
  MSG_INFO_WIFI_CONNECTED,
  MSG_INFO_WIFI_FAILED,
  MSG_INFO_WIFI_STATUS,
  };
  
struct msgSystemList {
  unsigned int msg_id;
  const char *fmt;
};

class DeviceConfigurator {
  private:
    deviceConfiguration *device;
    CRC32 crc32;
        
  public:
    bool isValidConfig;
    DeviceConfigurator();
    ~DeviceConfigurator(void);
    uint32_t getCRC(deviceConfiguration *d);

    deviceConfiguration *readStartupConfiguration();
    int loadStartupConfiguration();
    
    void saveDefaultConfiguration();
    void saveConfiguration();

    void dumpConfiguration(deviceConfiguration *d);

    void setMQTTParameters(char *broker, unsigned int port, char *user, char *pass);
    void setWiFiParameters(char *ssid, char *psk);
    void setDeviceLocation(char *location);
    void setDeviceName(char *name);

    const char *getWifiSSID();
    const char *getWifiPSK();

    const char *getMQTTBroker();
    const char *getMQTTUser();
    const char *getMQTTPass();
    int getMQTTPort();

    const char *getDeviceName();
    const char *getDeviceLocation();
};
extern void callback(char *topic, byte *payload, unsigned int length);
extern void mqtt_connect();
extern char *mqtt_build_topic(char *topic);
extern void publish_sensor(struct sensorControlData *sensor);

extern void callback(char *topic, byte *payload, unsigned int length);
extern void mqtt_connect();
extern char *mqtt_build_topic(char *topic);
extern void publish_sensor(struct sensorControlData *sensor);
extern void msg_to_system(unsigned int msg_id, ...);
extern struct msgSystemList system_messages[];

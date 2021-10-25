extern void sensor_get_stub(char *);
extern void sensor_get_free_heap(char *);
extern void sensor_get_total_heap(char *);
extern void sensor_get_uptime(char *);
extern void sensor_get_rssi(char *buf);
extern int sensor_definition_register_functions(struct sensorControlData *scd);
extern int sensors_init();
extern int sensors_dump_scd_entry(struct sensorControlData *d);
extern int sensors_dump_scd_list();
extern struct sensorControlData *sensor_get_by_name(char *name);

extern struct sensorControlData sensors[];
#ifdef CONFIG_HARDWARE_BME280
extern int bme280_init();
extern int bme280_register_functions();
extern void bme280_read_pressure(void *customData, char *buf);
extern void bme280_read_temp(void *customData, char *buf);
extern void bme280_read_altitude(void *customData, char *buf);
extern void bme280_read_humidity(void *customData, char *buf);
extern Adafruit_BME280 bme[2];
#endif

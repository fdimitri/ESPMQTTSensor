#ifdef CONFIG_HARDWARE_BME280
extern int bme280_init();
extern int bme280_register_functions();
extern void bme280_read_pressure(char *buf);
extern void bme280_read_temp(char *buf);
extern void bme280_read_altitude(char *buf);
extern void bme280_read_humidity(char *buf);
extern Adafruit_BME280 bme;
#endif

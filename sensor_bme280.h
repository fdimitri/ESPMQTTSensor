#ifdef CONFIG_HARDWARE_BME280
#define SEALEVELPRESSURE_HPA (1013.25)

int bme280_init();
int bme280_register_functions();
void bme280_read_pressure(char *buf);
void bme280_read_temp(char *buf);
void bme280_read_altitude(char *buf);
void bme280_read_humidity(char *buf);
#endif

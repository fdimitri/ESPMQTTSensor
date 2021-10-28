#ifdef CONFIG_HARDWARE_ESP8266ADC
extern int adc8266_init();
extern int adc8266_register_functions();
extern void adc8266_read_voltage(void *customData, char *buf);
#endif
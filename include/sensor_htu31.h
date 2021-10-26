#ifdef CONFIG_HARDWARE_HTU31
extern int htu31_register_functions();
extern int htu31_init();
extern void htu31_read_humidity(void *customData, char *buf);
extern void htu31_read_temp(void *customData, char *buf);
extern Adafruit_HTU31D htu;
#endif

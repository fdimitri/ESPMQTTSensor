#ifdef CONFIG_HARDWARE_HTU31
int htu31_register_functions();
int htu31_init();
void htu31_read_humidity(char *buf);
void htu31_read_temp(char *buf);
#endif

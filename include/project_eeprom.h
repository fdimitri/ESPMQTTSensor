extern void eeprom_save_config();
extern void eeprom_dump_config(struct deviceConfiguration *d);
extern int eeprom_load_config();
extern void eeprom_init();
extern struct deviceConfiguration *eeprom_get_config();
extern uint32_t eeprom_get_crc32(deviceConfiguration *d);
extern struct deviceConfiguration device;
extern void eeprom_save_default_config();

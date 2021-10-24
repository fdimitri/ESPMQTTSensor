void eeprom_save_config();
int eeprom_load_config();
void eeprom_init();
deviceConfiguration *eeprom_get_config();
uint32_t eeprom_get_crc32(deviceConfiguration *d);
 

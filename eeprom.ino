void eeprom_init() {
  EEPROM.begin(EEPROM_SIZE);
  eeprom_load_config();
}

void eeprom_save_config() {
//  byte *b = (byte *) &device;
//  for (unsigned int i = 0; i < sizeof(device); i++) {
//    EEPROM.update(i, *(b++));
//  }
  strcpy((char *) &device.version, VERSION);
  device.config_version = CONFIG_VERSION;
  EEPROM.put(EEPROM_DEVICE_CONFIG_OFFSET, device);
  EEPROM.commit();
  serial_printf("Saved configuration\n");
}

void eeprom_load_config() {
  EEPROM.get(EEPROM_DEVICE_CONFIG_OFFSET, device);
}

deviceConfiguration *eeprom_get_config() {
  static deviceConfiguration d;
  char buf[EEPROM_SIZE];
  EEPROM.get(EEPROM_DEVICE_CONFIG_OFFSET, d);
  return(&d);
}

void eeprom_update_config() {
  deviceConfiguration *d = eeprom_get_config();
  uint8_t *o, *n;
  o = (byte *) d;
  n = (byte *) &device;
  for (unsigned int i = 0; i < sizeof(device); i++) {
    if (*o != *n) {
      EEPROM.write(EEPROM_DEVICE_CONFIG_OFFSET + i, *n);
    }
    ++o;++n;
  }
  EEPROM.commit();
}

void eeprom_dump_config(deviceConfiguration *d) {
  serial_printf("Version: %s, %d\n", d->version, d->config_version);
  serial_printf("Name: %s, Location: %s\n", d->name, d->location);
  serial_printf("SSID: %s, PSK: %s\n", d->wifi_ssid, d->wifi_psk);
  serial_printf("MQTT Server: %s:%d\n", d->mqtt_broker, d->mqtt_port);
  serial_printf("MQTT User/pass: %s %s\n", d->mqtt_user, d->mqtt_pass);
}

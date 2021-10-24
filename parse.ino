
void parse_system_init_device(char *topic, char *argv[], unsigned int argc) {
  serial_printf("\nEntered parsing function for %s\n", argv[0]);
  return;
}

void parse_config_stub(char *topic, char *argv[], unsigned int argc) {

}

void parse_device_reboot(char *topic, char *argv[], unsigned int argc) {
  ESP.restart();
}

void parse_device_config_dump(char *topic, char *argv[], unsigned int argc) {
  eeprom_dump_config(eeprom_get_config());
}

void parse_device_rconfig_dump(char *topic, char *argv[], unsigned int argc) {
  eeprom_dump_config(&device);
}

void parse_device_config_wifi(char *topic, char *argv[], unsigned int argc) {
  // DEVICE.CONFIG.WIFI
  if (argc != 3) {
    // incorrect number of params
    return;
  }
  serial_printf("parse_device_config_wifi, argc: %d\n", argc);
  for (int i = 0; i < argc; i++) {
    serial_printf("%s ", argv[i]);
  }
  serial_printf("\n");
  
  memset((void *) &device.wifi_ssid, 0, 32);
  memset((void *) &device.wifi_psk, 0, 32);
  strcpy((char *) &device.wifi_ssid, argv[1]);
  strcpy((char *) &device.wifi_psk, argv[2]);
  eeprom_save_config();
}

void parse_device_config_mqtt(char *topic, char *argv[], unsigned int argc) {
  // DEVICE.CONFIG.WIFI
  if (argc != 5) {
    // incorrect number of params
    return;
  }

  memset((void *) &device.mqtt_broker, 0, sizeof(device.mqtt_broker));
  memset((void *) &device.mqtt_port, 0, sizeof(device.mqtt_port));
  memset((void *) &device.mqtt_user, 0, sizeof(device.mqtt_user));
  memset((void *) &device.mqtt_pass, 0, sizeof(device.mqtt_pass));

  
  strcpy((char *) &device.mqtt_broker, argv[1]);
  device.mqtt_port = atoi(argv[2]);
  strcpy((char *) &device.mqtt_user, argv[3]);
  strcpy((char *) &device.mqtt_pass, argv[4]);
  eeprom_save_config();
}

void parse_device_config_name(char *topic, char *argv[], unsigned int argc) {
  if (argc != 2) {
    // incorrect number of params
    return;
  }
  
  memset((void *) &device.name, 0, 64);
  memcpy(device.name, argv[1], strlen(argv[1]));
  eeprom_save_config();
}

void parse_device_config_location(char *topic, char *argv[], unsigned int argc) {
  if (argc != 2) {
    // incorrect number of params
    return;
  }
  memset((void *) &device.location, 0, 64);
  memcpy(device.location, argv[1], strlen(argv[1]));
  eeprom_save_config();
}

void parse_message(char *topic, char *omsg, unsigned int msgLength) {
  char *argv[32];
  unsigned int argc = 0;
  char *msg, *msgstart;
  msgstart = msg = (char *) malloc(msgLength + 1);
  memcpy(msg, omsg, msgLength);
  msg[msgLength] = '\0';

  argv[0] = strtok(msg, " ");
  msg = strtok(NULL, " ");

  while (msg != NULL && argc < 32) {
    argv[++argc] = msg;
    msg = strtok(NULL, " ");
  }
  
  for (unsigned int i = 0; msgs[i].command != NULL; i++) {
    if (strlen(msgs[i].command) == strlen(argv[0]) && !strcmp(msgs[i].command, argv[0])) {
      msgs[i].callback(topic, argv, argc + 1);
      free(msgstart);
      return;
    }
  }
  if (!strcmp("serial", topic)) {
    serial_printf("Unable to find parsing function for message:\n%s\n", omsg);
    serial_printf("argv[0]: %s\n", argv[0]);
  }
  free(msgstart);
  return;
}

#ifdef CONFIG_HARDWARE_HTU31
int htu31_init() {
  serial_printf("htu31_init(): Called\n");
  if (!(htu.begin(0x40) || htu.begin(0x41))) {
    serial_printf("htu31_init(): Sensor not found\n");
    return(-ESENSOR_NOT_FOUND);
  }
  serial_printf("Calling htu31_register_functions()\n");
  htu31_register_functions();
  serial_printf("Exiting htu31_init()\n");
  return(0);
}

int htu31_register_functions() {
  const char *temp_hardware_name = "htu31_temp\0";
  const char *hum_hardware_name = "htu31_humidity\0";
  struct sensorControlData *cptr;


  serial_printf("Allocating ram for cptr\n");
  cptr = (struct sensorControlData *) malloc(sizeof(struct sensorControlData));
  memset((void *) cptr, 0, sizeof(struct sensorControlData));

  serial_printf("Copying names into sensorName, sensorHardware\n");
  strcpy(cptr->sensorName, "comfort.temperature\0");
  strcpy(cptr->sensorHardware, temp_hardware_name);

  serial_printf("Assigning functions..\n");
  cptr->readSensor = htu31_read_temp;
  cptr->getState = cptr->setState = NULL;

  serial_printf("SCD Data to register:\n");
  sensors_dump_scd_entry(cptr);

  serial_printf("Calling sensor_definition_register_functions()\n");
  sensor_definition_register_functions(cptr);

  strcpy(cptr->sensorName, "comfort.relativeHumidity\0");
  strcpy(cptr->sensorHardware, hum_hardware_name);
  cptr->readSensor = htu31_read_humidity;

  sensor_definition_register_functions(cptr);

  serial_printf("htu31_register_functions(): Freeing cptr..\n");
  free(cptr);
  serial_printf("htu31_register_functions(): Returning..\n");
  return(0);
}

void htu31_read_humidity(char *buf) {
  sensors_event_t humidity, temp;
  htu.getEvent(&humidity, &temp);
  sprintf(buf, "%f", humidity.relative_humidity);
}

void htu31_read_temp(char *buf) {
  sensors_event_t humidity, temp;
  htu.getEvent(&humidity, &temp);
  sprintf(buf, "%f", temp.temperature);
}
#endif
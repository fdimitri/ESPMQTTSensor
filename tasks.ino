void task_update_sensors() {
  char buf[256];
  for (unsigned int i = 0; sensors[i].sensorFlags != 0x8000; i++) {
//    sprintf((char *) &buf, "Going through sensor %d - %s, flags %d - %s", i, sensors[i].sensorName, sensors[i].sensorFlags, sensors[i].currentData);
    if (!sensors[i].isEnabled) {
      continue;
    }
    if (sensors[i].sensorFlags == 0 && sensors[i].readSensor) {
      sensors[i].readSensor((char *) &sensors[i].currentData);
    }
    if (sensors[i].sensorFlags == 1 && sensors[i].getState) {
      sensors[i].getState((char *) &sensors[i].currentData);
    }
    
    publish_sensor((struct sensorControlData *) &sensors[i]);
  }
}

void task_read_serial() {
  serial_read();
}

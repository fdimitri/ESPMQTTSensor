void task_update_sensors();
void task_read_serial();

struct taskRun tasks[] = {
   { "updateSensors", task_update_sensors, 2000, 0 },
   { "readSerial", task_read_serial, 0, 0 },
   { NULL, NULL, -1, 0 },
};

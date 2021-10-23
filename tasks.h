void task_update_sensors();

struct taskRun tasks[] = {
   { task_update_sensors, 2000, 0 },
   { NULL, -1, 0 },
};

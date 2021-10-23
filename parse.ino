
void parse_system_init_device(char *topic, char *argv[], unsigned int argc) {
  
  return;
}

void parse_config_stub(char *topic, char *argv[], unsigned int argc) {

}

void parse_config_name(char *topic, char *argv[], unsigned int argc) {
  if (argc != 2) {
    // incorrect number of params
    return;
  }
  memcpy(device.name, argv[1], strlen(argv[1]));
}

void parse_config_location(char *topic, char *argv[], unsigned int argc) {
  if (argc != 2) {
    // incorrect number of params
    return;
  }
  memcpy(device.location, argv[1], strlen(argv[1]));
}

void parse_message(char *topic, char *omsg, unsigned int msgLength) {
  char *argv[32];
  unsigned int argc = 0;
  char *msg, *msgstart;
  msgstart = msg = (char *) malloc(msgLength + 1);
  memcpy(msg, omsg, msgLength);
  msg[msgLength] = '\0';
  argv[0] = strtok(msg, " ");
  while (msg != NULL && argc < 32) {
    argv[++argc] = msg;
    msg = strtok(NULL, " ");
  }
  free(msgstart);
  for (unsigned int i = 0; msgs[i].command != NULL; i++) {
    if (strlen(msgs[i].command) == strlen(argv[0]) && !strcmp(msgs[i].command, argv[0])) {
      msgs[i].callback(topic, argv, argc + 1);
    }
  }
  
}

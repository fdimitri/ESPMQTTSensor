void serial_read(void) {
  while (Serial.available() > 0) {
    uint8_t c = Serial.read();
    Serial.print((char) c);
    switch (c) {
      case '\r':
      case '\n':
        if (strlen(serial_command_buffer)) {
          parse_message("serial", (char *) &serial_command_buffer, strlen(serial_command_buffer));
          serial_command_buffer_ptr = 0;
        }
        memset((void *) &serial_command_buffer, 0, sizeof(serial_command_buffer));
        break;
      default:
        serial_command_buffer[serial_command_buffer_ptr++] = c;  
    }
  }
}

void serial_printf(const char *fmt, ...) {
  char buf[BUFFER_SIZE];
  memset((void *) buf, 0, sizeof(buf));
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, BUFFER_SIZE, fmt, args);
  va_end(args);
  Serial.print(buf);
  Serial.flush();
  return;
}

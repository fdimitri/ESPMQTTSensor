void serial_read(void) {
  while (Serial.available() > 0) {
    uint8_t c = Serial.read();
    switch (c) {
      case '\n':
        serial_command_buffer_ptr = 0;
        parse_message("serial", (char *) &serial_command_buffer, strlen(serial_command_buffer));
        break;
      default:
        serial_command_buffer[serial_command_buffer_ptr++] = c;  
    }
  }
}

void serial_printf(const char *fmt, ...) {
  char buf[256];
  memset((void *) buf, 0, sizeof(buf));
  va_list args;
  va_start(args, fmt);
  vsprintf(buf, fmt, args);
  va_end(args);
  Serial.print(buf);
  return;
}

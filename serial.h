char serial_command_buffer[256];
uint8_t serial_command_buffer_ptr = 0;

void serial_read(void);
void serial_printf(const char *fmt, ...);

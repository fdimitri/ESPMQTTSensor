extern void oled_msgLn(char *str, unsigned int length);
extern void oled_msg(const char *str, unsigned int length);
extern void oled_printf(const char *fmt, ...);
extern void oled_init(void);
extern void oled_print(const char *msg);
extern Adafruit_SSD1306 display;

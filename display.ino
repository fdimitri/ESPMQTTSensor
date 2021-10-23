void drawText(char *str, unsigned int length) {
  for(unsigned int i=0; i<length; i++) {
    display.write(str[i]);
  }
  display.display();
}

void drawTextLn(char *str, unsigned int length) {
  drawText(str, length);
  display.write("\n");
}

void callback(char *topic, byte *payload, unsigned int length) {
  display.clearDisplay();
  display.setCursor(0,0);
  drawTextLn(topic, strlen(topic));
  drawTextLn((char *) payload, length);
  parse_message(topic, (char *) payload, length);
}

void mqtt_connect() {
  char msgbuf[256];
  while (!client.connected()) {
      drawText((char *) &msgbuf, strlen(msgbuf));    
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("Public emqx mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
}

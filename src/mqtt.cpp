void callback(char *topic, byte *payload, unsigned int length) {
  display.clearDisplay();
  display.setCursor(0,0);
  oled_msgLn(topic, strlen(topic));
  oled_msgLn((char *) payload, length);
  parse_message(topic, (char *) payload, length);
}

void mqtt_connect() {
  char msgbuf[256];
  uint8_t retrycount = 16;

  serial_printf("Connecting to MQTT..\n");
  oled_printf("MQTT on port %d\n%s", device.mqtt_port, device.mqtt_broker);

  client.setServer(device.mqtt_broker, device.mqtt_port);
  client.setCallback(callback);

  while (!client.connected() && retrycount--) {
      task_read_serial();
      oled_msg((char *) &msgbuf, strlen(msgbuf));    
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), device.mqtt_user, device.mqtt_pass)) {
          serial_printf("Connected to MQTT server %s:%d with %s:%s\n", device.mqtt_broker, device.mqtt_port, device.mqtt_user, device.mqtt_pass);
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
}

char *mqtt_build_topic(char *topic) {
  sprintf(topic, "location/%s/device/%s", device.location, device.name);
  return(topic);
}

void publish_sensor(struct sensorControlData *sensor) {
  char buffer[128];
  char topicbuf[128];
  sprintf((char *) &buffer, "UPDATE.SENSOR %s %s", sensor->sensorName, sensor->currentData);
  client.publish(mqtt_build_topic((char *) &topicbuf), (char *) &buffer);
}


#ifndef STASSID
#define STASSID "TellMyWiFiFoundSomeoneWhos24"
#define STAPSK  "TwinTurbo2.7"
#endif
//#include <ESP8266WiFi.h>

#define SSD1306_128_32

#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// WiFi
const char *ssid = STASSID; // Enter your WiFi name
const char *password = STAPSK;  // Enter WiFi password
// MQTT Broker
const char *mqtt_broker = "192.168.0.133";
const char *topic = "device/unnamed";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);


//GxEPD2_BW<GxEPD2_270, GxEPD2_270::HEIGHT> display(GxEPD2_270(/*CS=5*/ 15, /*DC=*/ 4, /*RST=*/ 5, /*BUSY=*/ 16)); // GDEW027W3
//GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display(GxEPD2_290(15,4,5,16)); // GDEH029A1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void parse_systemInitDevice(char *, char *[], unsigned int);
void parse_configStub(char *, char *[], unsigned int);
void sensor_get_stub(char *);
struct msgCallbackList {
  const char *command;
  void (*callback)(char *topic, char *argv[], unsigned int argc);
};

struct msgCallbackList msgs[] = {
  { "SYSTEM.INIT.DEVICE", parse_systemInitDevice },
  { "SYSTEM.INIT.DEVICE.SENSOR", parse_systemInitDevice },
  { "SYSTEM.INIT.DEVICE.CONTROL", parse_systemInitDevice },
  { "DEVICE.CONFIG.WIFI", parse_configStub },
  { "DEVICE.CONFIG.NAME", parse_configStub },
  { "DEVICE.CONFIG.LOCATION", parse_configStub },
  { "GET.SENSORS", parse_configStub },
  { "UPDATE.SENSOR", parse_configStub },
  { "GET.STATE", parse_configStub },
  { "SET.STATE", parse_configStub },
  { NULL, NULL },
};

struct deviceConfiguration {
  char name[64];
  char wifimac[18];
  char location[64];
  uint32_t ip;
};

struct deviceConfiguration device {
  "un-named device", "8c:aa:b5:85:ef:94", 0
};

struct sensorControlData {
  const char *sensorName;
  char currentData[16];
  uint32_t sensorFlags;
  void (*readSensor)(char *buf);
  void (*getState)(char *buf);
  void (*setState)(char *buf);
};

struct sensorControlData sensors[] = {
  { "comfort.temperature", "", 0, sensor_get_stub, NULL, NULL },
  { "comfort.relativeHumidity", "", 0, sensor_get_stub, NULL, NULL },
  { "comfort.lightLevel", "", 0, sensor_get_stub, NULL, NULL },
  { "comfort.noiseLevel", "", 0, sensor_get_stub, NULL, NULL },
  { "system.wifi.RSSI", "", 0, sensor_get_stub, NULL, NULL, },
  { "system.uptime", "", 0, sensor_get_stub, NULL, NULL, },
  { "system.battery.voltage", "", 0, sensor_get_stub, NULL, NULL, },
  { "system.memory.free", "", 0, sensor_get_stub, NULL, NULL },
  { "system.memory.total", "", 0, sensor_get_stub, NULL, NULL },
  { "comfort.fan1", "", 1, NULL, sensor_get_stub, sensor_get_stub },
  { "comfort.fan2", "", 1, NULL, sensor_get_stub, sensor_get_stub },
  { NULL, "", 0, NULL, NULL, NULL },
};


void sensor_get_stub(char *buf) {
}

void setup() {

  // Set software serial baud to 115200;
  Serial.begin(115200);
  Wire.begin();
  
  // connecting to a WiFi network
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  Serial.println("Initializing display..");
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      //client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("Public emqx mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
  // publish and subscribe
  client.publish(topic, "hello, emqx!");
  client.subscribe("system/#");
  client.subscribe(topic);


  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  const char *msg = "Starting up!";
  drawTextLn((char *) msg, strlen(msg));
}



void drawText(char *str, unsigned int length) {

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for(unsigned int i=0; i<length; i++) {
    display.write(str[i]);
  }
  display.display();
}

void drawTextLn(char *str, unsigned int length) {
  drawText(str, length);
  display.write("\n");
}


void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
      Serial.print((char) payload[i]);
  }

  Serial.println();
  Serial.println("-----------------------");
  display.clearDisplay();
  display.setCursor(0,0);
  drawTextLn(topic, strlen(topic));
  drawTextLn((char *) payload, length);
  parseMessage(topic, (char *) payload, length);
}




void loop() {
  client.loop();
  client.publish(topic, "SYSTEM.DEVICE.INIT name MAC IPaddress");
  client.publish(topic, "UPDATE.SENSOR ccTemp 70.1");
  client.publish(topic, "UPDATE.SENSOR ccRelativeHumidity 50.0");
  delay(2000);
  yield();
}


void parse_systemInitDevice(char *topic, char *argv[], unsigned int argc) {
  
  return;
}

void parse_configStub(char *topic, char *argv[], unsigned int argc) {

}

void parse_configName(char *topic, char *argv[], unsigned int argc) {
  if (argc != 2) {
    // incorrect number of params
    return;
  }
  memcpy(device.name, argv[1], strlen(argv[1]));
}

void parse_configLocation(char *topic, char *argv[], unsigned int argc) {
  if (argc != 2) {
    // incorrect number of params
    return;
  }
  memcpy(device.location, argv[1], strlen(argv[1]));
}

void parseMessage(char *topic, char *omsg, unsigned int msgLength) {
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
  Serial.print("Free heap: "); Serial.println(ESP.getFreeHeap());
  Serial.println("Seeing if message has callback");
  for (unsigned int i = 0; msgs[i].command != NULL; i++) {
    if (strlen(msgs[i].command) == strlen(argv[0]) && !strcmp(msgs[i].command, argv[0])) {
      Serial.print("Found message callback "); Serial.print(msgs[i].command); Serial.println(" .. executing callback");
      msgs[i].callback(topic, argv, argc + 1);
    }
  }
  
}

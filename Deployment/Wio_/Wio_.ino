/*
   MQTT Exmple for SeeedStudio Wio Terminal
   Author: Salman Faris
   Date: 31/07/2020
   Last Updates: 02/08/2020

   MQTT Broker broker.mqtt-dashboard.com
   Subscribe Topic Name: WTIn
   Publish Topic Name: WTout
  - publishes "hello world" to the topic "WTout"
  - subscribes to the topic "WTin", printing out any messages

*/

#include <rpcWiFi.h>
#include"TFT_eSPI.h"
#include <PubSubClient.h>


// Update these with values suitable for your network.
const char* ssid = "KasuleJT";
const char* password = "kasulejohn"; // WiFi Password
// const char* ssid = "PONI 8398";
// const char* password = "1234567890"; 

// const char* mqtt_server = "broker.mqtt-dashboard.com";  // MQTT Broker URL
// IPAddress mqtt_server(192,168,137,133);
IPAddress mqtt_server(192,168,86,252);

TFT_eSPI tft;

WiFiClient wioClient;
PubSubClient client(wioClient);
long lastMsg = 0;
char msg[50];
int value = 0;


void setup_wifi() {

  delay(10);

  tft.setTextSize(2);
  tft.setCursor((320 - tft.textWidth("Connecting to Wi-Fi..")) / 2, 120);
  tft.print("Connecting to Wi-Fi..");

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); // Connecting WiFi

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");

  tft.fillScreen(TFT_BLACK);
  tft.setCursor((320 - tft.textWidth("Connected!")) / 2, 120);
  tft.print("Connected!");

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); // Display Local IP Address
  delay(2000);
}

void callback(char* topic, byte* payload, unsigned int length) {
  //tft.fillScreen(TFT_BLACK);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char buff_p[length];
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    buff_p[i] = (char)payload[i];
  }
  Serial.println();
  buff_p[length] = '\0';
  String msg_p = String(buff_p);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor((320 - tft.textWidth("MQTT Message")) / 2, 90);
  tft.print("MQTT Message: " );
  tft.setCursor((320 - tft.textWidth(msg_p)) / 2, 120);
  tft.print(msg_p); // Print receved payload

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {

  tft.fillScreen(TFT_BLACK);
  Serial.print("Attempting MQTT connection...");
  tft.setCursor((320 - tft.textWidth("Attempting MQTT connection...")) / 2, 90);
  tft.print("Attempting MQTT connection..." );
    // Create a random client ID
    String clientId = "WioTerminal-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
        tft.fillScreen(TFT_BLACK);
        tft.setCursor((320 - tft.textWidth("MQTT connected...")) / 2, 120);
        tft.print("MQTT connected!..");
      
      // Once connected, publish an announcement...
      client.publish("WTout", "hello world");
      // ... and resubscribe
      // client.subscribe("WTin");
                // Subscribe to topics here
      client.subscribe("rpi/broadcast");
      // client.subscribe("alarm");
      // client.subscribe("door");
      // client.subscribe("lights");
      // client.subscribe("speaker");
      // client.subscribe("fridge");
      // client.subscribe("camera");
      // client.subscribe("fan");
      // client.subscribe("tv");
      // client.subscribe("status");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {

  tft.begin();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(3);


  Serial.println();
  Serial.begin(115200);
  pinMode(WIO_KEY_A, INPUT_PULLUP);  
  pinMode(WIO_KEY_B, INPUT_PULLUP);  
  setup_wifi();
  client.setServer(mqtt_server, 1883); // Connect the MQTT Server
  client.setCallback(callback);
}

void loop() {


  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "Wio Terminal #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("WTout", msg);
  }

  if (digitalRead(WIO_KEY_A) == LOW)
  {
    Serial.println("Alarm will be put off");
    client.publish("rpi/broadcast","alarm off"); 
    delay(500);   
  }
    if (digitalRead(WIO_KEY_B) == LOW)
  {
    Serial.println("All will be put off");
    client.publish("rpi/broadcast","all off"); 
    delay(500);   
  }
}


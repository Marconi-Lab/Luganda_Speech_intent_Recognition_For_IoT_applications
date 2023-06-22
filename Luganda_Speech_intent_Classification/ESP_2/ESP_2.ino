#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

#define ledPin 2
Servo myservo;

// // Replace the SSID/Password details as per your wifi router
const char* ssid = "KasuleJT";
const char* password = "kasulejohn";

// const char* ssid = "kl";
// const char* password = "9999999980";

// const char* ssid = "CYBERSPACE 4070";
// const char* password = "Tonny123";

// const char* ssid = "Tracy Mukula";
// const char* password = "01020304";
// const char* ssid = "Elvis";
// const char* password = "1234567890";


// Replace your MQTT Broker IP address here:
// const char* mqtt_server = "broker.mqtt-dashboard.com";
// const char* mqtt_server = "192.168.137.187";
const char* mqtt_server = "192.168.86.252";

// const char* mqtt_server = "192.168.137.8";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;

// Define the buzzer pin
// int buzzerPin = 22;
int camera = 21;
int light = 4 ;
int tv = 32;
int fridge = 14;

void cameraOn(){
    digitalWrite(camera,1);
}

void cameraOff(){
    digitalWrite(camera,0);
}

void lightOn(){
  digitalWrite(light,0); 
}

void lightOff(){
  digitalWrite(light,1);
}

void tvOn(){
  digitalWrite(tv,1);
}

void tvOff(){
  digitalWrite(tv,0);
}
void fridgeOn(){
  digitalWrite(fridge,1);
}
void fridgeOff(){
  digitalWrite(fridge,0);  
}




void blink_led(unsigned int times, unsigned int duration){
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(duration);
    digitalWrite(ledPin, LOW); 
    delay(200);
  }
}


void setup_wifi() {
  delay(50); 
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int c=0;
  while (WiFi.status() != WL_CONNECTED) {
    blink_led(2,200); //blink LED twice (for 200ms ON time) to indicate that wifi not connected
    delay(1000); //
    Serial.print(".");
    c=c+1;
    if(c>10){
        ESP.restart(); //restart ESP after 10 seconds
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}


void connect_mqttServer() {
  // Loop until we're reconnected
  while (!client.connected()) {

        //first check if connected to wifi
        if(WiFi.status() != WL_CONNECTED){
          //if not connected, then first connect to wifi
          setup_wifi();
        }

        //now attemt to connect to MQTT server
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP32_client")) { // Change the name of client here if multiple ESP32 are connected
          //attempt successful
          Serial.println("connected");
          // Subscribe to topics here
          // client.subscribe("WTin");
          client.subscribe("rpi/broadcast");
          // client.subscribe("alarm");
          // client.subscribe("door");
          // client.subscribe("lights");
          // client.subscribe("speaker");
          // client.subscribe("fridge");
          // client.subscribe("camera");
          // client.subscribe("fan");
          // client.subscribe("tv");
          client.subscribe("status");
          //client.subscribe("rpi/xyz"); //subscribe more topics here
        } 
        else {
          //attempt not successful
          Serial.print("failed, rc=");
          Serial.print(client.state());
          Serial.println(" trying again in 2 seconds");
    
          blink_led(3,200); //blink LED three times (200ms on duration) to show that MQTT server connection attempt failed
          // Wait 2 seconds before retrying
          delay(2000);
        }
  }
  
}

//this function will be executed whenever there is data available on subscribed topics
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  // Check if a message is received on the topic "rpi/broadcast"
if(messageTemp == "lights on"){
   lightOn();
  }  
  
  if(messageTemp == "lights off"){
   lightOff();
// digitalWrite(light,0);   
  }    
  if(messageTemp == "tv on"){
   tvOn();
  }    
  if(messageTemp == "tv off"){
   tvOff();
  }  

  if(messageTemp == "camera on"){
   cameraOn();
  }  
  
  if(messageTemp == "camera off"){
   cameraOff();
  }
  
  if(messageTemp == "fridge on"){
   fridgeOn();
  }  
  if(messageTemp == "fridge off"){
   fridgeOff();
  }  
    if(messageTemp == "door open"){
        myservo.write(0);
      Serial.println("The door will be opened");

  }
    
    if(messageTemp == "door close"){
    myservo.write(110);    
      Serial.println("The door will be closed");
  }  
  
     //Similarly add more if statements to check for other subscribed topics 
    
  }
  Serial.println();


}

void setup() {
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(115200);
  // setup_pins();
 myservo.attach(5);
   pinMode(light, OUTPUT);   
  digitalWrite(light,1);
  setup_wifi();
  client.setServer(mqtt_server,1883);//1883 is the default port for MQTT server
  client.setCallback(callback);

  pinMode(camera, OUTPUT);
  pinMode(tv, OUTPUT);   
  pinMode(fridge, OUTPUT); 

}

void loop() {
  
  if (!client.connected()) {
    connect_mqttServer();
  }

  client.loop();
  
  long now = millis();
  if (now - lastMsg > 4000) {
    lastMsg = now;
    client.publish("WTout", "Still connected!"); //topic name (to which this ESP32 publishes its data). 37 is the dummy value.   
  }
//   lightOn();
//   delay(200);
// lightOff();
// delay(200);  
}

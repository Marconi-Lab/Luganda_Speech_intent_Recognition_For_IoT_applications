#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

#define ledPin 2
Servo myservo;

// Replace the SSID/Password details as per your wifi router
const char* ssid = "KasuleJT";
const char* password = "kasulejohn";

// const char* ssid = "kl";
// const char* password = "9999999980";

// const char* ssid = "CYBERSPACE 4070";
// const char* password = "Tonny123";

// const char* ssid = "Elvis";
// const char* password = "1234567890";

// Replace your MQTT Broker IP address here:
// const char* mqtt_server = "broker.mqtt-dashboard.com";
// const char* mqtt_server = "192.168.137.187";

const char* mqtt_server = "192.168.86.252";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
int f=1;

// Motor A
int motor1Pin1 = 27; 
int motor1Pin2 = 26; 
int enable1Pin = 14; 
int light = 32 ;
int light1 = 4;

// // Setting PWM properties
// const int freq = 30000;
// const int pwmChannel = 0;
// const int resolution = 8;
int dutyCycle = 195;

// Define the buzzer pin
int buzzerPin = 22;

// Function to turn on the buzzer
void turnOnBuzzer() {
  tone(buzzerPin, 1);
  digitalWrite(light, 1);  
  
  // del
  Serial.println("Buzzer turned ON");
}

void lightOn(){
  digitalWrite(light1,1); 
}

void lightOff(){
  digitalWrite(light1,0);
}

// Function to turn off the buzzer
void turnOffBuzzer() {
  noTone(buzzerPin);
  digitalWrite(light, 0);
  Serial.println("Buzzer turned OFF");
}

void turnOnFan() {
    
  digitalWrite(motor1Pin2, 1);
analogWrite (14, dutyCycle);  
  Serial.println("The fan will be turned on");
  Serial.println("Moving Forward");
}

void turnOffFan() {
    
    digitalWrite(motor1Pin2, 0);
  Serial.println("The fan will be turned off");
}

void increaseFan(){

  if(dutyCycle != 255)
{  dutyCycle += 20; }
  else{
Serial.println("That is the maximum speed available please.....");
  }
  analogWrite (14, dutyCycle);
}

void decreaseFan(){

  if(dutyCycle != 195)
{  dutyCycle -= 20; }
  else{   
Serial.println("That is the minimum speed available please.....");

  }
analogWrite (14, dutyCycle);
}


void blink_led(unsigned int times, unsigned int duration){
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, 1);
    delay(duration);
    digitalWrite(ledPin, 0); 
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
        if (client.connect("ESP32_client2")) { // Change the name of client here if multiple ESP32 are connected
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
          // client.subscribe("status");
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
    if(messageTemp == "fan decrease"){
    decreaseFan();
    }
  if(messageTemp == "lights on"){
   lightOn();
  }  
  
  if(messageTemp == "lights off"){
   lightOff();
// digitalWrite(light,0);   
  } 

    if(messageTemp == "fan increase"){ 
    increaseFan();    
    Serial.println("The fan will be increased");    
      }
  
    if(messageTemp == "fan off"){
      turnOffFan();
    // Stop the DC motor
    Serial.println("Motor stopped");
    }

    if(messageTemp == "fan on"){
    turnOnFan();
    }  

    if(messageTemp == "alarm on"){
      Serial.println("The alarm will be turned on");
    // Serial.println("Alarm is going on");
    turnOnBuzzer();  
    }
    
    if(messageTemp == "alarm off"){
      Serial.println("The alarm will be turned off");
      Serial.println("Alarm is going off");
      turnOffBuzzer(); // Turns off the buzzer 
    }

    if(messageTemp == "door open"){
        myservo.write(45);
      Serial.println("The door will be opened");

  }
    
    if(messageTemp == "door close"){
    myservo.write(90);    
      Serial.println("The door will be closed");
  }
  }
  Serial.println();



    //Similarly add more if statements to check for other subscribed topics 

}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(light, OUTPUT); 
  pinMode(light1, OUTPUT);  
  Serial.begin(115200);

  pinMode(buzzerPin, OUTPUT);  
  setup_wifi();
    // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
 
  digitalWrite(motor1Pin1, 0);
  
  client.setServer(mqtt_server,1883);//1883 is the default port for MQTT server
  client.setCallback(callback);
  
  myservo.attach(5);

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
  
}

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// IN1  IN2 Motor direction
// 0    0  Stop
// 0    1  clockwise
// 1    0 clockwise
// 1    1 Stop 
int IN1 = D1; //Direction pins
int IN2 = D2; 
// limit switches
int STOP_OPEN = D5; // limit switch for open
int STOP_CLOSED = D6; // limit switch for closed
const char* ssid = "themacs"; // Enter your WiFi name
const char* password =  "macmac51"; // Enter WiFi password
const char* mqttServer = "192.168.0.130";
const int mqttPort = 1883;
const char* mqttUser = "ian";
const char* mqttPassword = "macmac51";


unsigned int openAwning(void){
  Serial.println("hello from open awningfunction");
  // start motor to open
  // this needs a while loop for the hall sensor 
  // use delay for testin
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  delay(3000);
  return 1;
}

unsigned int closeAwning(void){
  Serial.println("hello from close awning function");
    // start motor to close
  // this needs a while loop for the hall sensor 
  // use delay for testin
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  delay(3000);
  return 1;
}


WiFiClient espClient;
PubSubClient client(espClient);
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  String strTopic;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  int msg = message.toInt();
  strTopic = String((char*)topic);
  int str_len = strTopic.length() + 1; 
  char char_array[str_len];
  strTopic.toCharArray(char_array, str_len);
  if (strTopic == "awning/setTargetPosition") {
    Serial.println(topic);
    if (msg) {
      char blindposition[10];
      client.publish("awning/getPositionState", "INCREASING");
      int open = openAwning();
      if (open) {
        client.publish("awning/getPositionState", "STOPPED");
        Serial.println("awning open");
      }
     //needs a char array so convert 
     itoa(msg, blindposition, 10);
     client.publish("awning/CurrentPosition", blindposition);
    }
    else {
      client.publish("awning/getPositionState", "DECREASING");      
      int closed = closeAwning();
      if (closed) {
      client.publish("awning/getPositionState", "STOPPED");  
      Serial.println("awning closed");
      }
      client.publish("awning/CurrentPosition", "0"); 
    }
  }
 
 
}


void setup() {
  // config motor direction pins output
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT); 
  
  // config limit switches 
  pinMode(STOP_OPEN, INPUT);
  pinMode(STOP_CLOSED, INPUT);  
  Serial.begin(115200);
//  pinMode(LED, OUTPUT); //LED pin as output
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
//  client.setCallback(callback2)
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
 // client.publish("awning/CurrentPosition", "0"); //Topic name
  client.subscribe("awning/setTargetPosition");
  client.subscribe("awning/getPositionState");
}
 

 
void loop() {
  client.loop();
}

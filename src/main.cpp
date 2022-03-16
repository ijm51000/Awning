#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
const char* ssid = "themacs"; // Enter your WiFi name
const char* password =  "macmac51"; // Enter WiFi password
const char* mqttServer = "192.168.0.130";
const int mqttPort = 1883;
const char* mqttUser = "ian";
const char* mqttPassword = "macmac51";
 
WiFiClient espClient;
PubSubClient client(espClient);
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  if (topic == "awning/setTargetPosition") {
    Serial.println(topic);
    client.publish("awning/CurrentPosition", "100");
  }
 
  Serial.print("Message:");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}


void setup() {
 
  Serial.begin(115200);
 
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
 
  client.publish("awning/CurrentPosition", "0"); //Topic name
  client.subscribe("awning/setTargetPosition");
  client.subscribe("awning/getPositionState");
}
 

 
void loop() {
  client.loop();
}

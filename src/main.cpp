#include <Arduino.h>
#include <WEMOS_SHT3X.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "config.h"

//I2C address of sensor
SHT3X sht30(0x45);

const char* ssid = SSID;          
const char* wifiPassword = WIFIPASSWORD;
char server[] = MQTTSERVER;
uint16_t port = MQTTPORT; 
char username[] = MQTTUSERNAME;
char password[] = MQTTPASSWORD;
char clientId[] = MQTTCLIENT;
char topic[] = MQTTTOPIC;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//callback for when mqtt subscribing
void callback(char* topic, byte* payload, unsigned int length) {
}

void setup() {
    Serial.begin(9600);

    WiFi.mode(WIFI_STA); // Define the NodeMCU Wifi mode

    Serial.println("Try to connect to " + String(ssid));
    WiFi.begin(ssid, wifiPassword); // Start the Wifi connection

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");

    mqttClient.setServer(server, port); // Define the server and port for the Mqtt Client
    mqttClient.setCallback(callback); // Define the callback function for the Mqtt Client. Not used in our case.
}

void mqttPublish(String payload) {

    while (!mqttClient.connected()) { // If the Mqtt Client is not connected, so try to connect
        Serial.print("MQTT connection...");
        if (mqttClient.connect(clientId, username, password)) { // Try to connect the Mqtt Client
            Serial.println("connected");
        } else {
        Serial.println("failed, rc=" + mqttClient.state());
        Serial.println(" try again in 1 second");
        delay(1000);
        }
    }

    mqttClient.publish(topic, (char*) payload.c_str()); // Send a message to MQTT broker
}

void loop() {

  if(sht30.get()==0){
    Serial.print("Temperature in Celsius : ");
    Serial.println(sht30.cTemp);
    Serial.print("Temperature in Fahrenheit : ");
    Serial.println(sht30.fTemp);
    Serial.print("Relative Humidity : ");
    Serial.println(sht30.humidity);
    Serial.println();

    String json = "{\"temp\":"+String(sht30.cTemp)+",";
    json +=  "\"hum\":"+String(sht30.humidity)+"}";
    mqttPublish(json);
  }
  else
  {
    Serial.println("Error!");
  }

 delay(10000);
} 

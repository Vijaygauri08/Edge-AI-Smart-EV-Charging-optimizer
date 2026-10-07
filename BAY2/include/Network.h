#ifndef NETWORK_H
#define NETWORK_H

#include <WiFiClient.h>
#define MQTT_MAX_PACKET_SIZE 512
#include <PubSubClient.h>

extern WiFiClient espClient;
extern PubSubClient mqtt;

void connectWiFi();
void connectMQTT();

void mqttCallback(char* topic, byte* payload, unsigned int length);


#endif
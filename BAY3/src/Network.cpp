#include <WiFi.h> 
#include <ArduinoJson.h> 
#include "network.h" 
#include "config.h" 
#include "attributes.h" 
#include "rpc.h" 
 
 
WiFiClient espClient; 
PubSubClient mqtt(espClient); 
 
// --------------------------------------------------------------------- 
void connectWiFi()  
{ 
  Serial.print("Connecting to WiFi"); 
  WiFi.begin(WIFI_SSID, WIFI_PASS); 
  unsigned long start = millis(); 
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) { 
    delay(300); 
    Serial.print("."); 
  } 
  Serial.println(WiFi.status() == WL_CONNECTED ? " connected." : " FAILED (will retry)."); 
} 
 
void connectMQTT()  
{ 
  if (WiFi.status() != WL_CONNECTED) return; 
  Serial.print("Connecting to ThingsBoard MQTT..."); 
  if (mqtt.connect(BAY_ID, TB_TOKEN, NULL))  
  { 
    Serial.println(" connected."); 

    // FIX: Register MQTT callback
    mqtt.setCallback(mqttCallback);

    mqtt.subscribe("v1/devices/me/attributes");             
    mqtt.subscribe("v1/devices/me/attributes/response/+");  
    mqtt.subscribe("v1/devices/me/rpc/request/+");          

    requestSharedAttributes();
  }  
  else  
  { 
    Serial.print(" failed, rc="); 
    Serial.println(mqtt.state()); 
    delay(1000); 
  } 
} 
 
// --------------------------------------------------------------------- 
// Single MQTT callback, dispatched by topic: attribute push/response vs. 
// RPC request. This is where Phase 6's two device-side features live. 
// --------------------------------------------------------------------- 
void mqttCallback(char* topic, byte* payload, unsigned int length)  
{ 
  String topicStr = String(topic); 
  char buf[400]; 
  unsigned int n = length < sizeof(buf) - 1 ? length : sizeof(buf) - 1; 
  memcpy(buf, payload, n); 
  buf[n] = '\0'; 
 
  Serial.print("[MQTT <<] "); 
  Serial.print(topicStr); 
  Serial.print(" "); 
  Serial.println(buf); 
 
  if (topicStr.startsWith("v1/devices/me/rpc/request/")) { 
    String requestId = topicStr.substring(topicStr.lastIndexOf('/') + 1); 
    handleRpc(requestId, buf); 
    return; 
  } 
 
  JsonDocument doc; 
  DeserializationError err = deserializeJson(doc, buf); 
  if (err) return; 
 
  JsonObject attrs;
  if (doc["shared"].is<JsonObject>()) {
      attrs = doc["shared"].as<JsonObject>();
  } else {
      attrs = doc.as<JsonObject>();
  }
  applySharedAttributes(attrs); 
}
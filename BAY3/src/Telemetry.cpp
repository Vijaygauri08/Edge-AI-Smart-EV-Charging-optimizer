#include <ArduinoJson.h>
#include "telemetry.h"
#include "network.h"
#include "state.h"
#include "config.h"

// ---------------------------------------------------------------------
// FR-7 telemetry publish. Adds `overloadActive` (additive, beyond SRS
// 8.3's baseline schema) so the ThingsBoard Overcurrent alarm rule can
// be a one-line filter. Also adds `manualOverrideActive` so the
// dashboard can show when a bay is under operator control instead of
// automatic optimization.
// ---------------------------------------------------------------------
void publishTelemetry(void)
{ //check if the devices are connected to the cloud or not
  if (!mqtt.connected()) return;
  //store the values as key,value pair format 
  JsonDocument doc;
  doc["bayId"] = BAY_ID;
  doc["voltage"] = round(voltage * 10) / 10.0;
  doc["current"] = round(current * 10) / 10.0;
  doc["power"] = round(power * 10) / 10.0;
  doc["temperature"] = round(temperature * 10) / 10.0;
  doc["bayStatus"] = bayStatus;
  //adding 2 more values to the buffer 
  doc["predictedArrivalProb"]= round(predictedArrivalProb * 100 )/100.0;
  doc["predictedDurationMin"]= predictedDurationMin;
  //add load decision,throttle level,overload is active or not 
  doc["throttleLevel"]=throttleLevel;
  doc["loadDecision"]=loadDecision;
  doc["overloadActive"]=overloadActive;
  doc["manualOverrideActive"]= manualOverrideActive;
  
  char buffer[512];
  serializeJson(doc, buffer);
  //push the data to the cloud  ,topic,data->buffer
  bool publishResult = mqtt.publish(
      "v1/devices/me/telemetry",
      buffer
  );

  Serial.print("[MQTT >>] ");
  Serial.println(buffer);

  Serial.print("[PAYLOAD SIZE] ");
  Serial.println(strlen(buffer));

  Serial.print("[MQTT BUFFER] ");
  Serial.println(mqtt.getBufferSize());

  Serial.print("[MQTT PUBLISH] ");
  Serial.println(publishResult ? "SUCCESS" : "FAILED");

  Serial.print("[MQTT CONNECTED] ");
  Serial.println(mqtt.connected() ? "YES" : "NO");
}

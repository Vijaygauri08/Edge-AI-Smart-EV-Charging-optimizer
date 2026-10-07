#include "attributes.h"
#include "network.h"
#include "state.h"

// SRS 8.4: ask ThingsBoard for the current value of each shared attribute
// on connect, instead of waiting for them to change first.
void requestSharedAttributes() {
  JsonDocument doc;
  doc["sharedKeys"] = "maxStationLoadW,overloadCurrentA,peakTariffStartHr,peakTariffEndHr,predictionThreshold";
  char buffer[128];
  serializeJson(doc, buffer);
  mqtt.publish("v1/devices/me/attributes/request/1", buffer);
}

void applySharedAttributes(JsonObject attrs) { 
  if (!attrs["maxStationLoadW"].isNull())      maxStationLoadW     = attrs["maxStationLoadW"]; 
  if (!attrs["overloadCurrentA"].isNull())     overloadCurrentA    = attrs["overloadCurrentA"];
  if (!attrs["peakTariffStartHr"].isNull())    peakTariffStartHr = attrs["peakTariffStartHr"];
  if (!attrs["peakTariffEndHr"].isNull())      peakTariffEndHr     = attrs["peakTariffEndHr"];
  if (!attrs["predictionThreshold"].isNull())  predictionThreshold = attrs["predictionThreshold"];

  Serial.print("[Attributes updated] maxStationLoadW="); Serial.print(maxStationLoadW);
  Serial.print(" overloadCurrentA="); Serial.print(overloadCurrentA);
  Serial.print(" peakTariff="); Serial.print(peakTariffStartHr); Serial.print("-"); Serial.print(peakTariffEndHr);
  Serial.print(" predictionThreshold="); Serial.println(predictionThreshold);
}

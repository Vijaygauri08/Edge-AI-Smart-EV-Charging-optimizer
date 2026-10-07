#include <Arduino.h>
#include <ArduinoJson.h>

#include "rpc.h"
#include "network.h"
#include "state.h"
#include "config.h"

// =====================================================
// ThingsBoard RPC Handler
// =====================================================

void handleRpc(String requestId, const char* payload)
{
    Serial.println();
    Serial.println("========================================");
    Serial.println("        RPC COMMAND RECEIVED");
    Serial.println("========================================");

    Serial.print("Request ID: ");
    Serial.println(requestId);

    Serial.print("Payload: ");
    Serial.println(payload);

    // -------------------------------------------------
    // Parse JSON
    // -------------------------------------------------

    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
        Serial.print("JSON ERROR: ");
        Serial.println(error.c_str());
        Serial.println("========================================");
        return;
    }

    // -------------------------------------------------
    // Read method
    // -------------------------------------------------

    String method = doc["method"] | "";

    Serial.print("RPC Method: ");
    Serial.println(method);

    JsonObject params = doc["params"];

    // -------------------------------------------------
    // Response document
    // -------------------------------------------------

    JsonDocument response;

    // =================================================
    // SET RELAY STATE
    // =================================================

    if (method == "setRelayState")
    {
        bool state = params["state"] | false;

        Serial.print("Relay command: ");

        if (state)
        {
            Serial.println("ON");

            digitalWrite(RELAY_PIN, HIGH);

            Serial.println("RELAY PIN -> HIGH");
        }
        else
        {
            Serial.println("OFF");

            digitalWrite(RELAY_PIN, LOW);

            Serial.println("RELAY PIN -> LOW");
        }

        // Manual control is active
        manualOverrideActive = true;

        // Update system state
        throttleLevel = state ? 100 : 0;

        loadDecision = state ? "ALLOW" : "MANUAL_OFF";

        // RPC response
        response["success"] = true;
        response["relayState"] = state;
        response["throttleLevel"] = throttleLevel;

        Serial.println("Manual override engaged.");
    }

    // =================================================
    // SET THROTTLE
    // =================================================

    else if (method == "setThrottle")
    {
        int level = params["level"] | 100;

        // Limit between 0 and 100
        level = constrain(level, 0, 100);

        manualOverrideActive = true;

        throttleLevel = level;

        loadDecision = "MANUAL_THROTTLE";

        response["success"] = true;
        response["throttleLevel"] = throttleLevel;

        Serial.print("Throttle level: ");
        Serial.println(level);
    }

    // =================================================
    // UNKNOWN METHOD
    // =================================================

    else
    {
        Serial.print("Unknown RPC method: ");
        Serial.println(method);

        response["success"] = false;
        response["error"] = "unknown method";
    }

    // -------------------------------------------------
    // Send RPC response to ThingsBoard
    // -------------------------------------------------

    char buffer[256];

    serializeJson(response, buffer, sizeof(buffer));

    String responseTopic =
        "v1/devices/me/rpc/response/" + requestId;

    bool published =
        mqtt.publish(responseTopic.c_str(), buffer);

    Serial.print("RPC Response: ");
    Serial.println(buffer);

    Serial.print("Response Topic: ");
    Serial.println(responseTopic);

    Serial.print("Response Published: ");

    if (published)
        Serial.println("YES");
    else
        Serial.println("NO");

    Serial.println("========================================");
    Serial.println();
}
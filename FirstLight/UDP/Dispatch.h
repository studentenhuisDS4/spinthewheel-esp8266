#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager

extern WiFiManager wifiManager;

void resetESP(OSCMessage &msg)
{
    Serial.println("Reset ESP");
    ESP.restart();
}

void resetWifi(OSCMessage &msg)
{
    wifiManager.resetSettings();
    ESP.restart();
}

// setWifi();
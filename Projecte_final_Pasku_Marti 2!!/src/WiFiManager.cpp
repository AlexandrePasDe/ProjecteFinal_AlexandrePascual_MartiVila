#include "WiFiManager.hpp"
#include <Arduino.h>

void WiFiManager::connect(const char* ssid, const char* password) {
  Serial.begin(115200);
  Serial.println("Conectando a WiFi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado con éxito.");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

#include <Arduino.h> 
#include "Hardware.hpp"
#include "WebHandler.hpp"
#include "WiFiManager.hpp"
#include <WebServer.h>
#include "Storage.hpp"
#include "utils.hpp"
#include "AudioManager.hpp"

// Configuración de red WiFi
const char* ssid = "tu_red";
const char* password = "tu_contraseña";

WebServer server;

void changeStation(int direction);
String getStationName(const String& url);

void setup() {
  Serial.begin(115200);

  // Inicializar almacenamiento y precargar estaciones
  Storage::begin();
  Storage::initDefaultStations();

  // Conexión a WiFi desde módulo separado
  WiFiManager::connect(ssid, password);

  // Inicialización del hardware
  Hardware::setup();
  Hardware::initButtons();

  // Configurar pines de audio
  setupAudioPins();

  // Configuración del servidor web
  WebHandler::setup(server);
  server.begin();
  Serial.println("Servidor web iniciado.");

  // Reproducir la primera estación precargada al arrancar
  if (Storage::getStationCount() > 0) {
    String stationUrl = Storage::loadStation(0);
    setAudio(stationUrl);
    Hardware::displayStation(getStationName(stationUrl));
  }
}

void loop() {
  server.handleClient();

  loopAudio();

  static unsigned long lastDebounce = 0;
  unsigned long now = millis();

  if (now - lastDebounce > 200) {  // Anti-rebote mejorado
    if (!digitalRead(Hardware::prevButton)) {
      changeStation(-1);
      lastDebounce = now;
    }
    if (!digitalRead(Hardware::nextButton)) {
      changeStation(1);
      lastDebounce = now;
    }
  }
}

void changeStation(int direction) {
  static int current = 0;
  int maxIndex = Storage::getStationCount() - 1;
  if (maxIndex < 0) return;  // No hay estaciones guardadas

  current += direction;
  if (current < 0) current = maxIndex;
  if (current > maxIndex) current = 0;

  String stationUrl = Storage::loadStation(current);
  setAudio(stationUrl);
  Hardware::displayStation(getStationName(stationUrl));
}

String getStationName(const String& url) {
  int start = url.indexOf("://");
  if (start == -1) start = 0; else start += 3;

  int end = url.indexOf('/', start);
  if (end == -1) end = url.length();

  String domain = url.substring(start, end);
  return domain;  
}

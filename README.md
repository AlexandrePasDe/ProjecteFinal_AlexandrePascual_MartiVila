# ProjecteFinal_AlexandrePascual_MartiVila

# ESP32-S3 Radio Streaming por WiFi

Proyecto para reproducir estaciones de radio por streaming en un ESP32-S3, controlado vía botones y con una interfaz web para agregar nuevas URLs.

---

## Descripción

Este proyecto permite:

- Conectarse a una red WiFi.
- Reproducir estaciones de radio por streaming desde URLs.
- Controlar el cambio de estaciones con botones físicos.
- Añadir nuevas estaciones vía interfaz web.
- Mostrar el nombre de la estación actual en una pantalla OLED.

---

## Estructura del Código

### `main.cpp`

```cpp

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

  // Reproducir la primera esta



```

- Configura la conexión WiFi.
- Inicializa hardware (pantalla OLED, botones).
- Inicia servidor web.
- Gestiona la lógica principal: escucha botones, cambia estación, procesa clientes web.


### `AudioManager.hpp / AudioManager.cpp`

```cpp
#include "AudioManager.hpp"
#include <Audio.h>  // Librería ESP32-audioI2S

Audio audio;  // Instancia global del manejador de audio

void setupAudioPins() {
  // Pines conectados al DAC o módulo I2S
  audio.setPinout(14, 13, 20);  // BCLK=14, LRC=13, DIN=20
}

void setAudio(const String& url) {
  Serial.println("Iniciando streaming: " + url);
  audio.stopSong();                  // Detiene reproducción previa
  audio.connecttohost(url.c_str()); // Inicia la nueva
}

void loopAudio() {
  audio.loop();  // Mantiene activo el flujo de datos
}
```


- Configura los pines del audio para I2S.
- Controla la reproducción de streaming.
- Funciones para iniciar y mantener el audio activo.

### `Hardware.hpp / Hardware.cpp`

```cpp

#include "Hardware.hpp"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Variables estáticas
Adafruit_SSD1306* Hardware::oled = nullptr;
int Hardware::prevButton = 18;
int Hardware::nextButton = 19;

void Hardware::setup() {
  Wire.begin(4, 21);  // SDA=4, SCL=21

  Hardware::oled = new Adafruit_SSD1306(128, 64, &Wire, -1);
  if (!Hardware::oled->begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true) {
      Serial.println("Error inicializando OLED");
      delay(500);
    }
  }

  Hardware::oled->clearDisplay();
  Hardware::oled->display();
}

void Hardware::initButtons() {
  pinMode(prevButton, INPUT_PULLUP);
  pinMode(nextButton, INPUT_PULLUP);
}

void Hardware::displayStation(String stationName) {
  Hardware::oled->clearDisplay();
  Hardware::oled->setTextSize(2);
  Hardware::oled->setTextColor(SSD1306_WHITE);
  Hardware::oled->setCursor(0, 0);
  Hardware::oled->println(stationName);
  Hardware::oled->display();
}

```

- Inicializa la pantalla OLED (I2C).
- Configura los botones (prev, next).
- Funciones para mostrar la estación actual en pantalla.

### `Storage.hpp / Storage.cpp`

```cpp

#include "Storage.hpp"
#include <EEPROM.h>

#define EEPROM_SIZE 512
#define MAX_STATIONS 10
#define MAX_URL_LENGTH 64

void Storage::begin() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.end();
}

void Storage::saveStation(const String& url, int index) {
  if (index < 0 || index >= MAX_STATIONS) return;

  EEPROM.begin(EEPROM_SIZE);
  int addr = index * MAX_URL_LENGTH;

  for (int i = 0; i < MAX_URL_LENGTH; i++) {
    EEPROM.write(addr + i, (i < url.length()) ? url[i] : 0);
  }

  int count = getStationCount();
  if (index >= count) {
    EEPROM.write(EEPROM_SIZE - 1, index + 1);
  }

  EEPROM.commit();
  EEPROM.end();
}

String Storage::loadStation(int index) {
  if (index < 0 || index >= getStationCount()) return "";

  EEPROM.begin(EEPROM_SIZE);
  int addr = index * MAX_URL_LENGTH;
  char buffer[MAX_URL_LENGTH + 1];

  for (int i = 0; i < MAX_URL_LENGTH; i++) {
    buffer[i] = EEPROM.read(addr + i);
    if (buffer[i] == 0) break;
  }
  buffer[MAX_URL_LENGTH] = 0;

  EEPROM.end();
  return String(buffer);
}

int Storage::getStationCount() {
  EEPROM.begin(EEPROM_SIZE);
  int count = EEPROM.read(EEPROM_SIZE - 1);
  EEPROM.end();
  return (count < 0 || count > MAX_STATIONS) ? 0 : count;
}

void Storage::initDefaultStations() {
  if (getStationCount() == 0) {
    saveStation("http://stream.live.vc.bbcmedia.co.uk/bbc_radio_one", 0);
    saveStation("http://icecast.omroep.nl/radio2-bb-mp3", 1);
    saveStation("http://stream-dc1.radioparadise.com/aac-320", 2);
    saveStation("http://stream.live.vc.bbcmedia.co.uk/bbc_6music", 3);
    saveStation("http://ice1.somafm.com/groovesalad-128-mp3", 4);
  }
}

```

- Gestión de memoria EEPROM para guardar URLs de estaciones.
- Funciones para guardar, cargar y contar estaciones guardadas.

### `WebHandler.hpp / WebHandler.cpp`

```cpp

#include "WebHandler.hpp"
#include "Storage.hpp"
#include <WebServer.h>

static WebServer* _server;

void WebHandler::setup(WebServer& server) {
  _server = &server;
  _server->on("/", HTTP_GET, handleRoot);
  _server->on("/station", HTTP_POST, addStation);
}

void WebHandler::handleRoot() {
  String html = "<html><body><h1>Radio Streaming</h1>\
                 <form method='POST' action='/station'>\
                 <input type='text' name='url'>\
                 <button type='submit'>Agregar</button></form>\
                 <h2>Estaciones guardadas:</h2><ul>";

  int count = Storage::getStationCount();
  for (int i = 0; i < count; i++) {
    html += "<li>" + Storage::loadStation(i) + "</li>";
  }

  html += "</ul></body></html>";

  _server->send(200, "text/html", html);
}

void WebHandler::addStation() {
  if (_server->hasArg("url")) {
    String url = _server->arg("url");
    int count = Storage::getStationCount();

    if (count < 10) {
      Storage::saveStation(url, count);
      _server->send(200, "text/html", "<p>Estación guardada: " + url + "</p><a href='/'>Volver</a>");
    } else {
      _server->send(400, "text/html", "<p>Máximo de estaciones alcanzado.</p><a href='/'>Volver</a>");
    }
  } else {
    _server->send(400, "text/html", "<p>Falta parámetro 'url'</p><a href='/'>Volver</a>");
  }
}

```

- Controla el servidor web.
- Permite mostrar la página principal con formulario para agregar nuevas estaciones.
- Recibe nuevas URLs vía POST y las guarda en EEPROM.

### `WiFiManager.hpp / WiFiManager.cpp`

```cpp

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

```

- Maneja la conexión WiFi con SSID y contraseña.
- Reintenta hasta conectar.


---

## Pines usados

| Función       | GPIO ESP32-S3 |
|---------------|---------------|
| Botón Anterior| 18            |
| Botón Siguiente| 19           |
| I2C SDA       | 4             |
| I2C SCL       | 21            |
| I2S BCLK      | 14            |
| I2S LRCLK     | 13            |
| I2S DIN       | 20            |

---

## Cómo usar

1. Configura en `main.cpp` tu SSID y contraseña WiFi.
2. Conecta la pantalla OLED, botones y módulo de audio según los pines indicados.
3. Compila y sube el código a tu ESP32-S3.
4. Accede a la IP que se muestra en el monitor serie para agregar estaciones.
5. Usa los botones para cambiar entre las estaciones guardadas.
6. La estación actual se muestra en la pantalla OLED.

---

## Notas

- La EEPROM tiene espacio para hasta 10 URLs.
- Cada URL puede tener hasta 64 caracteres.
- Los botones usan resistencias pull-up internas.
- El proyecto usa la librería `ESP32-audioI2S` para la reproducción de streaming.



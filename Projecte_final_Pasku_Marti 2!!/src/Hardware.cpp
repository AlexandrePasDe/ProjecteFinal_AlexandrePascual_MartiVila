#include "Hardware.hpp"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Variables estáticas
Adafruit_SSD1306* Hardware::oled = nullptr;
int Hardware::prevButton = 18; // Pin para botón "anterior"
int Hardware::nextButton = 19; // Pin para botón "siguiente"

void Hardware::setup() {
  // Inicializa el bus I2C con SDA en GPIO 22 y SCL en GPIO 21
  Wire.begin(4, 21);

  // Crea pantalla OLED de 128x64 con I2C
  Hardware::oled = new Adafruit_SSD1306(128, 64, &Wire, -1);
  if (!Hardware::oled->begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true) {
      Serial.println("Error inicializando OLED");
      delay(500);
    }
  }

  // Limpiar pantalla al iniciar
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

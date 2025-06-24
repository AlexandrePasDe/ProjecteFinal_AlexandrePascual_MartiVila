#ifndef HARDWARE_HPP
#define HARDWARE_HPP

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

class Hardware {
  public:
    static void setup();
    static void initButtons();
    static void displayStation(String stationName);

    // Variables estáticas para botones y OLED
    static Adafruit_SSD1306* oled;  // Puntero a instancia OLED
    static int prevButton;
    static int nextButton;
};

#endif


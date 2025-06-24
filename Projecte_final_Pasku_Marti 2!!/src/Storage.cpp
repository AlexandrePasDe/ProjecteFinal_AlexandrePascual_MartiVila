#include "Storage.hpp"
#include <EEPROM.h>

#define EEPROM_SIZE 512
#define MAX_STATIONS 10
#define MAX_URL_LENGTH 64

void Storage::saveStation(const String& url, int index) {
  if (index < 0 || index >= MAX_STATIONS) return;

  EEPROM.begin(EEPROM_SIZE);

  int addr = index * MAX_URL_LENGTH;

  for (int i = 0; i < MAX_URL_LENGTH; i++) {
    if (i < url.length()) {
      EEPROM.write(addr + i, url[i]);
    } else {
      EEPROM.write(addr + i, 0);
    }
  }

  // Guardar cantidad de estaciones en la última posición de EEPROM
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

  if (count < 0 || count > MAX_STATIONS) return 0;
  return count;
}

void Storage::initDefaultStations() {
  if (getStationCount() > 0) return;  // Ya hay estaciones guardadas, no sobrescribimos

  const char* defaultStations[] = {
    "http://stream.radioparadise.com/mp3-192",
    "http://bbcmedia.ic.llnwd.net/stream/bbcmedia_radio1_mf_p",
    "http://icy1.abacast.com/kplu-jazz24mp3-64",
    "http://kusc.streamguys1.com/kusc128.mp3",
    "https://npr-ice.streamguys1.com/live.mp3"
  };

  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < 5; i++) {
    saveStation(String(defaultStations[i]), i);
  }
  EEPROM.write(EEPROM_SIZE - 1, 5);  // Guardamos el número de estaciones
  EEPROM.commit();
  EEPROM.end();
}

#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

inline void delay_ms(int ms) {
  delay(ms);
}

// Define el número máximo de estaciones permitidas
constexpr int MAX_STATIONS = 10;
constexpr int MAX_URL_LENGTH = 64;

// Devuelve el índice máximo permitido para estaciones
inline int maxStationIndex() {
  return MAX_STATIONS - 1;
}

// Extrae un nombre simple desde una URL, por ejemplo, usando parte del dominio
inline String getStationName(const String& url) {
  int start = url.indexOf("://");
  start = (start >= 0) ? start + 3 : 0;
  int end = url.indexOf('/', start);
  if (end < 0) end = url.length();
  return url.substring(start, end); // Devuelve el dominio como nombre
}

#endif // UTILS_H

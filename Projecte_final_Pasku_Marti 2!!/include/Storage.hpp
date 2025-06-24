// Storage.hpp
#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <Arduino.h>

class Storage {
  public:
    static void begin();
    static void saveStation(const String& url, int index);
    static String loadStation(int index);
    static int getStationCount();
    static void initDefaultStations();  // Nueva función
};

#endif


#include "AudioManager.hpp"
#include <Audio.h>  // Librería ESP32-audioI2S

Audio audio;  // Instancia global del manejador de audio

void setupAudioPins() {
  // Cambia estos pines a los que tengas disponibles en tu placa
  audio.setPinout(14, 13, 20);  // BCLK=14, LRC=13, DIN=20
}

void setAudio(const String& url) {
  Serial.println("Iniciando streaming: " + url);
  audio.stopSong();                 // Detiene cualquier reproducción anterior
  audio.connecttohost(url.c_str());  // Inicia el streaming de la nueva URL
}

void loopAudio() {
  audio.loop();  // Debe llamarse frecuentemente para mantener el flujo de datos
}












/*#include "AudioManager.hpp"
#include <Audio.h>  // Librería ESP32-audioI2S

Audio audio;  // Instancia global del manejador de audio

void setAudio(const String& url) {
  Serial.println("Iniciando streaming: " + url);
  audio.stopSong();                 // Detiene cualquier reproducción anterior
  audio.connecttohost(url.c_str());  // Inicia el streaming de la nueva URL
}

void loopAudio() {
  audio.loop();  // Debe llamarse frecuentemente para mantener el flujo de datos
}
*/
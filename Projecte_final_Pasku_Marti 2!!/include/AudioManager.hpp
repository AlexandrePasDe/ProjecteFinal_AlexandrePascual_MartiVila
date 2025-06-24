#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <Arduino.h>

void setupAudioPins();
void setAudio(const String& url);
void loopAudio();

#endif
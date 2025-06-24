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

    if (count < 10) {  // MAX_STATIONS
      Storage::saveStation(url, count);
      _server->send(200, "text/html", "<p>Estación guardada: " + url + "</p><a href='/'>Volver</a>");
    } else {
      _server->send(400, "text/html", "<p>Máximo de estaciones alcanzado.</p><a href='/'>Volver</a>");
    }
  } else {
    _server->send(400, "text/html", "<p>Falta parámetro 'url'</p><a href='/'>Volver</a>");
  }
}

#ifndef WEBHANDLER_H
#define WEBHANDLER_H

#include <WebServer.h>

class WebHandler {
  public:
    static void setup(WebServer& server);
  private:
    static void handleRoot();
    static void addStation();
};

#endif // WEBHANDLER_H

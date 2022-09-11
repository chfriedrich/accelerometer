
#ifndef network_h
#define network_h

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#define BUF_LEN_BYTE 5000

class network
{
  public:
    network() {};
    ~network() {};

    void init(const char* ssid, const char* password);
    void run();

  private:

};

#endif
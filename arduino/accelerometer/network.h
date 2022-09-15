
#ifndef network_h
#define network_h

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#define BUF_LEN_BYTE 5000


class Twebsite
{
  public:
    Twebsite() {};
    ~Twebsite() {};

    void init(const char* ssid, const char* password);
    void run();

    uint8_t isStartReq()    { return start_requested; };
    uint8_t isStopReq()     { return stop_requested; };
    uint8_t isClearReq()    { return clear_requested; };
    uint8_t isCalibReq()    { return calib_requested; };

    void setStartReq(uint8_t val)    { start_requested = val; };
    void setStopReq(uint8_t val)     { stop_requested = val; };
    void setClearReq(uint8_t val)    { clear_requested = val; };
    void setCalibReq(uint8_t val)    { calib_requested = val; };

    void clear_requests();

  private:
    uint8_t start_requested = 0;
    uint8_t stop_requested = 0;
    uint8_t clear_requested = 0;
    uint8_t calib_requested = 0;
};

extern Twebsite g_website;

#endif
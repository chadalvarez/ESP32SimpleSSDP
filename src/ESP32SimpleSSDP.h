#ifndef ESP32SIMPLESSDP_H
#define ESP32SIMPLESSDP_H

#include <WiFi.h>
#include <AsyncUDP.h>
#include <ESPAsyncWebServer.h>

class ESP32SimpleSSDP {
public:
    ESP32SimpleSSDP();
    void begin();
    void loop();

    void setRootHandler(ArRequestHandlerFunction handler);
    void addWebHandler(const char* path, ArRequestHandlerFunction handler);
    void setDeviceInfo(const char* key, const char* value);
    String getDeviceInfo(const char* key);
    void printDeviceInfo();

    AsyncWebServer& getServer(); 

private:
    AsyncWebServer server;
    AsyncUDP udp;
    String deviceUUID;
    unsigned long lastNotifyTime;
    static const unsigned long notifyInterval = 1800UL * 1000UL;
    static int bootId;

    void generateUUID();
    void startUDP();
    void sendSSDPNotify();
    void setupWebServer();

    ArRequestHandlerFunction customRootHandler = nullptr; // 🔹 Stores user-defined root handler
};

#endif

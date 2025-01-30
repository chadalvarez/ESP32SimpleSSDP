#include <ESP32SimpleSSDP.h>

ESP32SimpleSSDP ssdp;

void setup() {
    Serial.begin(115200);
    delay(1000);

    WiFi.begin("YourSSID", "YourPassword");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());

    // Set custom root handler BEFORE calling `begin()`
    ssdp.setRootHandler([](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Custom Root Handler: SSDP Ready!");
    });



    // Option1: Add Custom Web Handlers
    ssdp.addWebHandler("/status", [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Device is Running!");
    });

    ssdp.addWebHandler("/temperature", [](AsyncWebServerRequest *request) {
        float temp = random(0, 100); // Example temperature data
        request->send(200, "text/plain", String("Temperature: ") + temp + " °C");
    });

    // Option2: Directly Customize the Web Server
    AsyncWebServer& server = ssdp.getServer();

    server.on("/custom", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Custom API Endpoint");
    });

    server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Server Time: " + String(millis()));
    });



    ssdp.setDeviceInfo("friendlyName", "Custom ESP32 Device");
    ssdp.setDeviceInfo("manufacturer", "ESP32");
    ssdp.setDeviceInfo("manufacturerURL", "http://esp32.com");
    ssdp.setDeviceInfo("modelName", "ESP32-SSDP");
    ssdp.setDeviceInfo("modelURL", "http://esp32.com/viewforum.php?f=18");    
    ssdp.setDeviceInfo("modelNumber", "2.0");
    ssdp.setDeviceInfo("serialNumber", "123456");
    ssdp.setDeviceInfo("presentationURL", "/");

    ssdp.begin();
}

void loop() {
    ssdp.loop();
}

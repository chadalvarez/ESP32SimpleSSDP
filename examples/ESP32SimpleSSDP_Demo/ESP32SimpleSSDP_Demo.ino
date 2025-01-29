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

    ssdp.setDeviceInfo("friendlyName", "Custom ESP32 Device");

    ssdp.setDeviceInfo("manufacturer", "ESP32");
    ssdp.setDeviceInfo("manufacturerURL", "http://esp32.com");

    ssdp.setDeviceInfo("modelName", "ESP32-SSDP");
    ssdp.setDeviceInfo("modelURL", "http://esp32.com/model");    
    ssdp.setDeviceInfo("modelNumber", "2.0");

    ssdp.setDeviceInfo("serialNumber", "123456");

    ssdp.setDeviceInfo("presentationURL", "/");

    
    ssdp.begin();
    
    ssdp.printDeviceInfo(); // 🔹 Print SSDP details to Serial

    // 🔹 Override the "/" root handler
    ssdp.setRootHandler([](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Custom Root Handler: SSDP Ready!");
    });
}

void loop() {
    ssdp.loop();
}

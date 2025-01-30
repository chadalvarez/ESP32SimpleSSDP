#include "ESP32SimpleSSDP.h"

// Device Info Storage (Dynamic)
struct DeviceInfo {
    const char* key;
    String value;
} deviceInfo[] = {
    {"friendlyName", "ESP32 SSDP Device"},
    {"deviceType", "urn:schemas-upnp-org:device:Basic:1"},
    {"manufacturer", "ESP32"},
    {"manufacturerURL", "http://esp32.com"},
    {"modelName", "ESP32-SSDP"},
    {"modelNumber", "1.0"},
    {"modelURL", "http://esp32.com/model"},
    {"serialNumber", "123456"},
    {"presentationURL", "/"}
};

int ESP32SimpleSSDP::bootId = 1;

ESP32SimpleSSDP::ESP32SimpleSSDP() : server(80), lastNotifyTime(0) {}

void ESP32SimpleSSDP::begin() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("ERROR: WiFi not connected! Please connect before calling begin().");
        return;
    }

    bootId++;

    generateUUID();
    startUDP();
    setupWebServer();

    printDeviceInfo();

    for (int i = 0; i < 3; i++) {
        sendSSDPNotify();
        delay(1000);
    }
}

void ESP32SimpleSSDP::loop() {
    if (millis() - lastNotifyTime > notifyInterval) {
        lastNotifyTime = millis();
        sendSSDPNotify();
    }
}

// Print SSDP Device Details to Serial
void ESP32SimpleSSDP::printDeviceInfo() {
    Serial.println("\n--- SSDP Device Info ---");
    for (const auto& item : deviceInfo) {
        Serial.printf("%-18s: %s\n", item.key, item.value.c_str());
    }
    Serial.printf("UUID               : %s\n", deviceUUID.c_str());
    Serial.printf("Boot ID            : %d\n", bootId);
    Serial.println("-------------------------");
}

// Modify Device Info at Runtime
void ESP32SimpleSSDP::setDeviceInfo(const char* key, const char* value) {
    for (auto& item : deviceInfo) {
        if (strcmp(item.key, key) == 0) {
            item.value = value;
            return;
        }
    }
}

// Retrieve Device Info
String ESP32SimpleSSDP::getDeviceInfo(const char* key) {
    for (const auto& item : deviceInfo) {
        if (strcmp(item.key, key) == 0) {
            return item.value;
        }
    }
    return "";
}

// Generate UUID based on ESP32 MAC Address
void ESP32SimpleSSDP::generateUUID() {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char uuidStr[64];

    snprintf(uuidStr, sizeof(uuidStr),
             "uuid:%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X",
             mac[0], mac[1], mac[2],
             mac[3], mac[4], mac[5],
             mac[0], mac[1], mac[2],
             mac[3], mac[4], mac[5]);

    deviceUUID = String(uuidStr);

    Serial.println("Generated UUID: " + deviceUUID);  // Debugging log
}


// Start UDP listener for SSDP M-SEARCH
void ESP32SimpleSSDP::startUDP() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("ERROR: WiFi not connected. Cannot start SSDP UDP.");
        return;
    }

    if (udp.listenMulticast(IPAddress(239, 255, 255, 250), 1900)) {
        Serial.println("Listening for SSDP M-SEARCH...");
        udp.onPacket([this](AsyncUDPPacket packet) {
            String request = (char*)packet.data();
            if (request.indexOf("M-SEARCH") >= 0 &&
                (request.indexOf("upnp:rootdevice") >= 0 ||
                 request.indexOf("ssdp:all")       >= 0 ||
                 request.indexOf("ssdp:discover")  >= 0)) 
            {
                String response =
                    "HTTP/1.1 200 OK\r\n"
                    "CACHE-CONTROL: max-age=1800\r\n"
                    "EXT:\r\n"
                    "LOCATION: http://" + WiFi.localIP().toString() + ":80/description.xml\r\n"
                    "SERVER: Arduino/1.0 UPnP/1.0 ESP32/1.0\r\n"
                    "ST: upnp:rootdevice\r\n"
                    "USN: " + deviceUUID + "::upnp:rootdevice\r\n"
                    "BOOTID.UPNP.ORG: " + String(bootId) + "\r\n"
                    "\r\n";
                packet.printf(response.c_str());
                Serial.println("SSDP Response Sent");
            }
        });
    }
}

// Send SSDP NOTIFY packets
void ESP32SimpleSSDP::sendSSDPNotify() {
    String notifyMessage =
        "NOTIFY * HTTP/1.1\r\n"
        "HOST: 239.255.255.250:1900\r\n"
        "NT: upnp:rootdevice\r\n"
        "NTS: ssdp:alive\r\n"
        "LOCATION: http://" + WiFi.localIP().toString() + ":80/description.xml\r\n"
        "USN: " + deviceUUID + "::upnp:rootdevice\r\n"
        "CACHE-CONTROL: max-age=1800\r\n"
        "SERVER: Arduino/1.0 UPnP/1.0 ESP32/1.0\r\n"
        "BOOTID.UPNP.ORG: " + String(bootId) + "\r\n"
        "\r\n";
    udp.broadcastTo(notifyMessage.c_str(), 1900);
    Serial.println("Sent SSDP NOTIFY");
}

// Set up the SSDP Web Server
void ESP32SimpleSSDP::setupWebServer() {
    server.on("/description.xml", HTTP_GET, [this](AsyncWebServerRequest *request) {
        String xml =
            "<?xml version=\"1.0\"?>\n"
            "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\n"
            "  <specVersion>\n"
            "    <major>1</major>\n"
            "    <minor>0</minor>\n"
            "  </specVersion>\n"
            "  <device>\n"
            "    <deviceType>" + getDeviceInfo("deviceType") + "</deviceType>\n"
            "    <friendlyName>" + getDeviceInfo("friendlyName") + "</friendlyName>\n"
            "    <manufacturer>" + getDeviceInfo("manufacturer") + "</manufacturer>\n"
            "    <manufacturerURL>" + getDeviceInfo("manufacturerURL") + "</manufacturerURL>\n"
            "    <modelName>" + getDeviceInfo("modelName") + "</modelName>\n"
            "    <modelNumber>" + getDeviceInfo("modelNumber") + "</modelNumber>\n"
            "    <modelURL>" + getDeviceInfo("modelURL") + "</modelURL>\n"
            "    <serialNumber>" + getDeviceInfo("serialNumber") + "</serialNumber>\n"
            "    <UDN>" + deviceUUID + "</UDN>\n"
            "    <presentationURL>http://" + WiFi.localIP().toString() + getDeviceInfo("presentationURL") + "</presentationURL>\n"
            "  </device>\n"
            "</root>\n";

        request->send(200, "text/xml", xml);
    });

    // Use stored custom root handler if set
    if (customRootHandler) {
        server.on("/", HTTP_ANY, customRootHandler);
    } else {
        server.on("/", HTTP_ANY, [](AsyncWebServerRequest *request) {
            request->send(200, "text/plain", "Hello from ESP32 SSDP!");
        });
    }

    server.begin();
}

void ESP32SimpleSSDP::setRootHandler(ArRequestHandlerFunction handler) {
    customRootHandler = handler; // Store user handler for later use
}

void ESP32SimpleSSDP::addWebHandler(const char* path, ArRequestHandlerFunction handler) {
    server.on(path, HTTP_ANY, handler);
    Serial.printf("Custom Web Handler added: %s\n", path);
}

AsyncWebServer& ESP32SimpleSSDP::getServer() {
    return server;
}


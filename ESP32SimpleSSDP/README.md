# ESP32SimpleSSDP
A simple ESP32 SSDP (UPnP) library for easy device discovery.

## Features
- ✅ Customizable SSDP device details
- ✅ Customizable root (`/`) handler
- ✅ Easy-to-use API
- ✅ Serial debugging with `printDeviceInfo()`
- ✅ Works with `ESPAsyncWebServer`

## Installation
1. Download the `ESP32SimpleSSDP` library.
2. Place it in your `Arduino/libraries/` folder.
3. Restart the Arduino IDE.

## Usage
```cpp
#include <ESP32SimpleSSDP.h>
ESP32SimpleSSDP ssdp;

void setup() {
    WiFi.begin("SSID", "PASSWORD");
    ssdp.setDeviceInfo("friendlyName", "My ESP32 Device");
    ssdp.begin();
}
void loop() {
    ssdp.loop();
}

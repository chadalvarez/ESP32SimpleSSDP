# CMakeLists.txt for ESP32SimpleSSDP
cmake_minimum_required(VERSION 3.16)

idf_component_register(
    SRCS "ESP32SimpleSSDP.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_wifi esp_eth esp_http_server esp_timer nvs_flash
)

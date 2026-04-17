#pragma once

/*
  Copy this file to include/config.h and update the values for your local setup.
  The real config.h file is ignored by Git so that Wi-Fi and MQTT credentials
  do not get published in the repository.
*/

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define MQTT_SERVER "192.168.1.100"
#define MQTT_PORT 1883
#define MQTT_USER "YOUR_MQTT_USERNAME"
#define MQTT_PASSWORD "YOUR_MQTT_PASSWORD"
#define MQTT_CLIENT_ID "ld2410_radar"

#define MQTT_TOPIC_STATUS "home/ld2410/status"
#define MQTT_TOPIC_MOTION "home/ld2410/motion"
#define MQTT_TOPIC_DISTANCE "home/ld2410/distance"

/*
  UART wiring used by the current firmware:
  - LD2410 TX -> ESP32 GPIO16
  - LD2410 RX -> ESP32 GPIO17

  Verify these pins against your hardware before uploading.
*/
#define LD2410_RX_PIN 16
#define LD2410_TX_PIN 17
#define LD2410_SERIAL_BAUD 256000

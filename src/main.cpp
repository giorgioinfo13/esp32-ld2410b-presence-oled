#include <WiFi.h>
#include <PubSubClient.h>
#include <U8g2lib.h>
#include <ld2410.h>

#include "config.h"

/* ───── GLOBAL OBJECTS ───── */
WiFiClient        wifiClient;
PubSubClient      mqtt(wifiClient);
HardwareSerial    radarSerial(2);               // GPIO16 (RX2), GPIO17 (TX2)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
ld2410            radar;

/* ───── PROTOTYPES ───── */
void drawWifiIcon();
void drawScreen(bool motion, int dist);

/* ───── MQTT RECONNECT ───── */
void mqttReconnect() {
  if (WiFi.status() != WL_CONNECTED) return;

  if (!mqtt.connected()) {
    if (mqtt.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      mqtt.publish(MQTT_TOPIC_STATUS, "online", true);
    }
  }
}

/* ───── SETUP ───── */
void setup() {
  Serial.begin(115200);

  /* OLED init */
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  /* Non-blocking Wi-Fi connect attempt with a 5 second timeout. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 5000) {
    delay(100);
  }

  /* MQTT */
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);

  /* Radar */
  radarSerial.begin(LD2410_SERIAL_BAUD, SERIAL_8N1, LD2410_RX_PIN, LD2410_TX_PIN);
  radar.begin(radarSerial);
  radar.debug(Serial);

  drawScreen(false, 0);
}

/* ───── LOOP ───── */
void loop() {
  /* Retry Wi-Fi every 10 seconds if disconnected. */
  static unsigned long lastWifiAttempt = 0;
  if (WiFi.status() != WL_CONNECTED && millis() - lastWifiAttempt > 10000) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    lastWifiAttempt = millis();
  }

  /* Gestione MQTT */
  if (!mqtt.connected()) mqttReconnect();
  mqtt.loop();

  /* Read radar state. */
  static bool  motion = false;
  static int   dist   = 0;

  if (radar.read()) {
    motion = radar.presenceDetected();
    dist   = radar.movingTargetDistance();

    /* Publish radar status when MQTT is connected. */
    if (mqtt.connected()) {
      mqtt.publish(MQTT_TOPIC_MOTION, motion ? "ON" : "OFF", true);
      mqtt.publish(MQTT_TOPIC_DISTANCE, String(dist).c_str(), true);
    }
  }

  /* Refresh the OLED every 200 ms. */
  static unsigned long lastDraw = 0;
  if (millis() - lastDraw > 200) {
    drawScreen(motion, dist);
    lastDraw = millis();
  }
}

/* ───── DRAW FUNCTIONS ───── */
void drawScreen(bool motion, int dist) {
  u8g2.clearBuffer();

  /* Title */
  u8g2.drawStr(0, 10, "LD2410B Radar");
  u8g2.drawHLine(0, 12, 128);

  /* Motion */
  u8g2.setCursor(0, 30);
  u8g2.print("Motion: ");
  u8g2.print(motion ? "YES" : "NO");

  /* Distance */
  u8g2.setCursor(0, 50);
  u8g2.print("Distance: ");
  u8g2.print(dist);
  u8g2.print(" cm");

  /* Wi-Fi icon if connected */
  if (WiFi.status() == WL_CONNECTED) drawWifiIcon();

  u8g2.sendBuffer();
}

void drawWifiIcon() {
  long rssi = WiFi.RSSI();           // dBm
  /* Icon bars start at x=102. */
  if (rssi >= -55) {
    u8g2.drawBox(102,  7, 4, 1);
    u8g2.drawBox(107,  6, 4, 2);
    u8g2.drawBox(112,  4, 4, 4);
    u8g2.drawBox(117,  2, 4, 6);
    u8g2.drawBox(122,  0, 4, 8);
  } else if (rssi >= -65) {
    u8g2.drawBox(102,  7, 4, 1);
    u8g2.drawBox(107,  6, 4, 2);
    u8g2.drawBox(112,  4, 4, 4);
    u8g2.drawBox(117,  2, 4, 6);
    u8g2.drawFrame(122, 0, 4, 8);
  } else if (rssi >= -75) {
    u8g2.drawBox(102,  7, 4, 1);
    u8g2.drawBox(107,  6, 4, 2);
    u8g2.drawBox(112,  4, 4, 4);
    u8g2.drawFrame(117, 2, 4, 6);
    u8g2.drawFrame(122, 0, 4, 8);
  } else if (rssi >= -85) {
    u8g2.drawBox(102,  7, 4, 1);
    u8g2.drawBox(107,  6, 4, 2);
    u8g2.drawFrame(112, 4, 4, 4);
    u8g2.drawFrame(117, 2, 4, 6);
    u8g2.drawFrame(122, 0, 4, 8);
  } else if (rssi >= -96) {
    u8g2.drawBox(102,  7, 4, 1);
    u8g2.drawFrame(107, 6, 4, 2);
    u8g2.drawFrame(112, 4, 4, 4);
    u8g2.drawFrame(117, 2, 4, 6);
    u8g2.drawFrame(122, 0, 4, 8);
  } else {
    u8g2.drawFrame(102, 7, 4, 1);
    u8g2.drawFrame(107, 6, 4, 2);
    u8g2.drawFrame(112, 4, 4, 4);
    u8g2.drawFrame(117, 2, 4, 6);
    u8g2.drawFrame(122, 0, 4, 8);
  }
}

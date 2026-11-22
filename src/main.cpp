#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "secrets.h"

// ---- Configuration -------------------------------------------------------
// GPIO for LED1..LED13 on the PCB (index 0 = LED1). LEDs are active high.
// LED3 and LED4 sit on GPIO1/GPIO3 (the USB serial pins), so this firmware
// deliberately does not use Serial.
constexpr uint8_t LED_PINS[13] = {23, 22, 1, 3, 21, 19, 32, 33, 25, 26, 27, 14, 12};

// Layout (binary-coded decimal: each group shows the ones digit and the tens digit
// separately):
//   Hours:   LED1..LED4 = 1,2,4,8   LED5..LED6  = 10,20
//   Minutes: LED7..LED10 = 1,2,4,8  LED11..LED13 = 10,20,40
constexpr uint8_t HOUR_FIRST = 0, HOUR_BITS = 6;
constexpr uint8_t MINUTE_FIRST = 6, MINUTE_BITS = 7;

constexpr const char *NTP_SERVER = "pool.ntp.org";
// POSIX TZ string (Netherlands, CET/CEST); see https://github.com/nayarsystems/posix_tz_db
constexpr const char *TIMEZONE = "CET-1CEST,M3.5.0,M10.5.0/3";
// --------------------------------------------------------------------------

constexpr uint8_t LED_COUNT = sizeof(LED_PINS);

void setLed(uint8_t i, bool on) { digitalWrite(LED_PINS[i], on ? HIGH : LOW); }

void setAll(bool on) {
  for (uint8_t i = 0; i < LED_COUNT; i++) setLed(i, on);
}

void showBits(uint8_t first, uint8_t count, uint8_t value) {
  for (uint8_t b = 0; b < count; b++) setLed(first + b, (value >> b) & 1);
}

// Ones digit in the low 4 bits, tens digit in the bits above.
uint8_t toBcd(uint8_t value) { return ((value / 10) << 4) | (value % 10); }

// Startup test: count up in binary on each LED group, so every LED can be
// checked by eye. Hour LEDs count 0..63, then minute LEDs 0..127.
void countUp(uint8_t first, uint8_t bits, uint16_t stepMs) {
  for (uint16_t v = 0; v < (1u << bits); v++) {
    showBits(first, bits, v);
    delay(stepMs);
  }
  showBits(first, bits, 0);
}

void startupSweep() {
  countUp(HOUR_FIRST, HOUR_BITS, 80);
  countUp(MINUTE_FIRST, MINUTE_BITS, 40);
  delay(300);
  setAll(false);
}

void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  bool on = false;
  while (WiFi.status() != WL_CONNECTED) {
    setAll(on = !on);  // flash all LEDs while connecting
    delay(500);
  }
}

void syncTime() {
  configTzTime(TIMEZONE, NTP_SERVER);
  struct tm t;
  bool on = false;
  while (!getLocalTime(&t, 1000)) {
    setAll(on = !on);  // flash while waiting for NTP
  }
}

void setup() {
  for (uint8_t p : LED_PINS) pinMode(p, OUTPUT);
  setAll(false);
  startupSweep();
  connectWifi();
  syncTime();
  setAll(false);
}

void loop() {
  struct tm t;
  if (getLocalTime(&t, 100)) {
    showBits(MINUTE_FIRST, MINUTE_BITS, toBcd(t.tm_min));
    showBits(HOUR_FIRST, HOUR_BITS, toBcd(t.tm_hour));
  }
  delay(50);
}

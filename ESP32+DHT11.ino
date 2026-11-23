/*
 * ESP32 + DHT11 — วัดอุณหภูมิและความชื้น
 *
 * - อ่านค่า DHT11 ทุก 2 วินาที แสดงใน Serial Monitor (115200)
 * - ต่อ WiFi แล้วเปิดหน้าเว็บดูค่าได้จากมือถือ/คอม ที่ http://<IP ของบอร์ด>/
 * - JSON สำหรับต่อระบบอื่น: http://<IP>/api
 *
 * ไลบรารี: "DHT sensor library" (Adafruit) + "Adafruit Unified Sensor"
 * Board: ESP32 Dev Module
 *
 * การต่อสาย DHT11 (โมดูล 3 ขา):
 *   VCC  -> 3V3
 *   DATA -> GPIO 4
 *   GND  -> GND
 * (ถ้าเป็นตัวเปล่า 4 ขา ใส่ R 10k ดึง DATA ขึ้น 3V3)
 */

#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include "secrets.h"  // WIFI_SSID / WIFI_PASS (ไม่ขึ้น git · ดู secrets.example.h)

// ================== ตั้งค่า ==================
#define DHT_PIN   4
#define DHT_TYPE  DHT11
const unsigned long READ_MS = 2000;  // DHT11 อ่านได้เร็วสุด ~1 ครั้ง/วินาที

DHT dht(DHT_PIN, DHT_TYPE);
WebServer server(80);

float temperature = NAN;
float humidity    = NAN;
unsigned long lastRead = 0;
unsigned long lastOk   = 0;

// ================== เซนเซอร์ ==================
void readSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();  // °C
  if (isnan(h) || isnan(t)) {
    Serial.println("อ่าน DHT11 ไม่ได้ — เช็กสายและขา DATA");
    return;
  }
  humidity = h;
  temperature = t;
  lastOk = millis();
  Serial.printf("Temp: %.1f °C   Humidity: %.0f %%\n", t, h);
}

bool sensorOk() {
  return lastOk != 0 && millis() - lastOk < 10000;
}

// ================== เว็บ ==================
void handleRoot() {
  String t = sensorOk() ? String(temperature, 1) : "--";
  String h = sensorOk() ? String(humidity, 0) : "--";
  String html =
    "<!doctype html><html lang='th'><head><meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<meta http-equiv='refresh' content='5'><title>ESP32 + DHT11</title>"
    "<style>body{font-family:sans-serif;background:#111827;color:#f9fafb;text-align:center;margin:0;padding:24px}"
    ".row{display:flex;gap:16px;justify-content:center;flex-wrap:wrap}"
    ".card{background:#1f2937;border-radius:16px;padding:24px;min-width:140px}"
    ".v{font-size:48px;font-weight:bold}.t{color:#f97316}.h{color:#38bdf8}.l{color:#9ca3af}</style></head><body>"
    "<h2>ESP32 + DHT11</h2><div class='row'>"
    "<div class='card'><div class='l'>อุณหภูมิ</div><div class='v t'>" + t + "&deg;C</div></div>"
    "<div class='card'><div class='l'>ความชื้น</div><div class='v h'>" + h + "%</div></div>"
    "</div><p class='l'>อัปเดตทุก 5 วินาที</p></body></html>";
  server.send(200, "text/html; charset=utf-8", html);
}

void handleApi() {
  String json = "{\"ok\":" + String(sensorOk() ? "true" : "false");
  if (sensorOk()) {
    json += ",\"temperature\":" + String(temperature, 1) + ",\"humidity\":" + String(humidity, 0);
  }
  json += ",\"uptime_s\":" + String(millis() / 1000) + "}";
  server.send(200, "application/json", json);
}

// ================== WiFi ==================
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("กำลังต่อ WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("ต่อ WiFi แล้ว เปิดดูได้ที่ http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("ต่อ WiFi ไม่ได้ — ยังดูค่าใน Serial Monitor ได้ (จะลองใหม่อัตโนมัติ)");
  }
}

// ================== หลัก ==================
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\nESP32 + DHT11 เริ่มทำงาน");
  dht.begin();
  connectWiFi();
  server.on("/", handleRoot);
  server.on("/api", handleApi);
  server.begin();
}

void loop() {
  if (millis() - lastRead >= READ_MS) {
    lastRead = millis();
    readSensor();
  }

  static unsigned long lastWiFiTry = 0;
  if (WiFi.status() != WL_CONNECTED && millis() - lastWiFiTry > 30000) {
    lastWiFiTry = millis();
    WiFi.reconnect();
  }

  server.handleClient();
}

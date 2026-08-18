// install library yang dibutuhkan: LiquidCrystal_I2C, WiFi, HTTPClient, GP2YDustSensor, MQ135
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <GP2YDustSensor.h>
#include <MQ135.h> // Tambahan Library MQ135
#include "config.h"

// inisiasi pin dan konstanta 
#define MQ135_DO_PIN 15
#define MQ135_AO_PIN 34
#define BUZZER_PIN 27
#define FAN_RELAY_PIN 26
#define GREEN_RELAY_PIN 14
#define YELLOW_RELAY_PIN 12
#define RED_RELAY_PIN 13
#define DUST_VO_PIN 35
#define DUST_LED_PIN 25
#define DUST_WARNING_THRESHOLD_UGM3 1000.0f // 1000 ug/m³ = 1.0 mg/m³ (warning polusi)
#define DUST_DANGER_THRESHOLD_UGM3 2000.0f  // 2000 ug/m³ = 2.0 mg/m³ (polusi, fan ON)
#define DUST_NORMAL_THRESHOLD_UGM3 1000.0f  // fan OFF saat dust kembali < 1000 ug/m³
#define LCD_SDA_PIN 21
#define LCD_SCL_PIN 22
#define THINGSPEAK_UPDATE_INTERVAL_MS 15000UL // 15 detik batas waktu update ThingSpeak
#define PPM_WARNING_THRESHOLD 50.0f  // ambang batas PPM untuk status warning
#define PPM_DANGER_THRESHOLD 150.0f  // ambang batas PPM untuk status danger (fan ON)
#define PPM_NORMAL_THRESHOLD 50.0f   // ambang batas PPM kembali normal (fan OFF)

// inisialisasi LCD I2C 20x4, GP2Y dust sensor, dan MQ135 sensor library
LiquidCrystal_I2C lcd(0x27, 20, 4);
GP2YDustSensor dustSensor(GP2YDustSensorType::GP2Y1010AU0F, DUST_LED_PIN, DUST_VO_PIN);
MQ135 mq135_sensor = MQ135(MQ135_AO_PIN); // Inisialisasi Objek MQ135

unsigned long lastThingSpeakUpdate = 0;
unsigned long lastBuzzerToggle = 0;
bool buzzerState = false;

// State fan dengan hysteresis untuk mencegah flickering
bool fanActiveFromGas = false;
bool fanActiveFromDust = false;

// Status udara
#define AIR_NORMAL 0
#define AIR_WARNING 1
#define AIR_DANGER 2

// Fungsi untuk mendapatkan label status udara berdasarkan nilai status
const char* getAirStatusLabel(int status) {
  if (status == AIR_WARNING) {
    return "Warning";
  }
  if (status == AIR_DANGER) {
    return "Danger ";
  }
  return "Normal ";
}

// Fungsi untuk mendapatkan label LED berdasarkan status relay
const char* getLEDStatusLabel(bool greenOn, bool yellowOn, bool redOn) {
  if (redOn) {
    return "Merah ";
  }
  if (yellowOn) {
    return "Kuning";
  }
  if (greenOn) {
    return "Hijau ";
  }
  return "Off   ";
}

// Fungsi untuk mengatur pola buzzer berdasarkan status udara
bool updateBuzzerPattern(int systemStatus) {
  unsigned long now = millis();

  if (systemStatus == AIR_DANGER) {
    buzzerState = true;
    return true;
  }

  if (systemStatus != AIR_WARNING) {
    buzzerState = false;
    lastBuzzerToggle = 0;
    return false;
  }

  if (lastBuzzerToggle == 0 || now - lastBuzzerToggle >= 300UL) {
    lastBuzzerToggle = now;
    buzzerState = !buzzerState;
  }

  return buzzerState;
}

// Fungsi untuk mendapatkan status debu berdasarkan ug/m³
int getDustStatus(float dustDensityUgM3) {
  if (dustDensityUgM3 >= DUST_DANGER_THRESHOLD_UGM3) {
    return AIR_DANGER;
  }
  if (dustDensityUgM3 >= DUST_WARNING_THRESHOLD_UGM3) {
    return AIR_WARNING;
  }
  return AIR_NORMAL;
}

// Fungsi untuk mendapatkan label status debu (Diperbarui untuk aman - warning - polusi)
const char* getDustStatusLabel(int dustStatus) {
  if (dustStatus == AIR_DANGER) {
    return "POLUSI "; // Padding spasi agar menimpa karakter lama di LCD
  }
  if (dustStatus == AIR_WARNING) {
    return "WARNING";
  }
  return "AMAN   "; 
}

// Fungsi untuk memperbarui tampilan LCD dengan informasi terbaru
void updateDisplay(float ppmValue, const char* airStatusLabel, bool wifiOn, float dustDensityMgM3, const char* dustStatusLabel, bool fanOn, bool greenOn, bool yellowOn, bool redOn) {
  // Hanya memperbarui area tertentu untuk menghindari LCD berkedip
  lcd.setCursor(0, 0);
  lcd.print("Gas:");
  lcd.print(ppmValue, 0);
  lcd.print("ppm ");
  lcd.print(airStatusLabel);
  
  lcd.setCursor(0, 1);
  lcd.print("Dust:");
  lcd.print(dustDensityMgM3, 2);
  lcd.print("mg ");
  lcd.print(dustStatusLabel);
  
  lcd.setCursor(0, 2);
  lcd.print("LED:");
  lcd.print(getLEDStatusLabel(greenOn, yellowOn, redOn));
  lcd.print("   "); // Membersihkan sisa karakter
  
  lcd.setCursor(0, 3);
  lcd.print("WiFi:");
  lcd.print(wifiOn ? "ON " : "OFF");
  lcd.print(" Fan:");
  lcd.print(fanOn ? "ON " : "OFF");
}

// Fungsi untuk menghubungkan ke WiFi 
void connectWiFi() {
  Serial.print("Connecting WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connect failed");
  }
}

// Fungsi untuk mengirim data ke ThingSpeak
void sendToThingSpeak(int analogValue, float voltageAO, float ppmEstimate, bool gasDetected, float dustDensityUgM3, bool fanOn) {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("ThingSpeak skipped: WiFi down");
    return;
  }

  // Konversi ug/m³ ke mg/m³ untuk ThingSpeak (1 mg = 1000 ug)
  float dustDensityMgM3 = dustDensityUgM3 / 1000.0f;

  // Membangun URL untuk mengirim data ke ThingSpeak  
  String url = "https://api.thingspeak.com/update?api_key=";
  url += THINGSPEAK_API_KEY;
  url += "&field1=";
  url += analogValue;
  url += "&field2=";
  url += String(voltageAO, 3);
  url += "&field3=";
  url += String(ppmEstimate, 0);
  url += "&field4=";
  url += gasDetected ? 1 : 0;
  url += "&field5=";
  url += String(dustDensityMgM3, 3);
  url += "&field6=";
  url += fanOn ? 1 : 0;

  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  Serial.print("ThingSpeak response: ");
  Serial.println(httpCode);
  if (httpCode > 0) {
    Serial.println(http.getString());
  }
  http.end();
}

// fungsi utama pada app inisialisasi pin tampilan board led dll
void setup() {
  Serial.begin(115200);
  
  // Inisialisasi pin GPIO
  pinMode(MQ135_DO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  pinMode(GREEN_RELAY_PIN, OUTPUT);
  pinMode(YELLOW_RELAY_PIN, OUTPUT);
  pinMode(RED_RELAY_PIN, OUTPUT);
  
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(FAN_RELAY_PIN, LOW);
  digitalWrite(GREEN_RELAY_PIN, LOW);
  digitalWrite(YELLOW_RELAY_PIN, LOW);
  digitalWrite(RED_RELAY_PIN, LOW);
  
  // Test buzzer startup
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);

  // Inisialisasi ADC
  analogReadResolution(12);
  analogSetPinAttenuation(MQ135_AO_PIN, ADC_11db);
  analogSetPinAttenuation(DUST_VO_PIN, ADC_11db);

  // Inisialisasi LCD
  Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Sugeng IOT");
  lcd.setCursor(0, 1);
  lcd.print("  Air Quality Mon");
  lcd.setCursor(0, 2);
  lcd.print("MQ135 + GP2Y1010");
  lcd.setCursor(0, 3);
  lcd.print("    Starting...");
  delay(2000);
  lcd.clear(); // Bersihkan layar setelah loading

  // Inisialisasi GP2Y dust sensor library
  dustSensor.begin();

  // Debug messages
  Serial.println("=== Sugeng IOT start ===");
  Serial.println("Hardware:");
  Serial.println("  MQ-135 DO -> GPIO15");
  Serial.println("  MQ-135 AO -> GPIO34");
  Serial.println("  GP2Y LED  -> GPIO25");
  Serial.println("  GP2Y Vo   -> GPIO35");
  Serial.println("  LCD SDA   -> GPIO21");
  Serial.println("  LCD SCL   -> GPIO22");
  Serial.println("  Buzzer    -> GPIO27");
  Serial.println("  Relay fan -> GPIO26");
  Serial.println("  Relay hijau  -> GPIO14");
  Serial.println("  Relay kuning -> GPIO12");
  Serial.println("  Relay merah  -> GPIO13");
  Serial.println();
  Serial.println("Logika Gas MQ-135:");
  Serial.println("  PPM 0-49:   Normal  -> Hijau");
  Serial.println("  PPM 50-149: Warning -> Kuning + buzzer flash");
  Serial.println("  PPM >=150:  Danger  -> Merah + fan ON + buzzer");
  Serial.println();
  Serial.println("Logika Dust GP2Y1010AU0F:");
  Serial.println("  < 1000 ug/m3 (1.0 mg/m3):   AMAN -> Hijau, fan OFF");
  Serial.println("  1000-1999 ug/m3 (1.0-1.9):  WARNING -> Kuning + buzzer flash");
  Serial.println("  >= 2000 ug/m3 (2.0 mg/m3):  POLUSI -> Merah + fan ON + buzzer");
  Serial.println();
  Serial.println("ThingSpeak:");
  Serial.println("  field1 = MQ135 ADC");
  Serial.println("  field2 = MQ135 Voltage");
  Serial.println("  field3 = MQ135 PPM");
  Serial.println("  field4 = Gas Status (0/1)");
  Serial.println("  field5 = Dust Density (mg/m3)");
  Serial.println("  field6 = Fan Status (0/1)");
  Serial.println();

  connectWiFi();
}

// fungsi utama looping atau pengulangan pengecekan pada esp dan alatnya
void loop() {
  // Baca sensor MQ-135 menggunakan library
  int nilaiDigital = digitalRead(MQ135_DO_PIN);
  int nilaiAnalog = analogRead(MQ135_AO_PIN); 
  float voltageAO = nilaiAnalog * (3.3f / 4095.0f); // Hanya untuk display/ThingSpeak
  float ppmEstimate = mq135_sensor.getPPM();        // Menggunakan library untuk mendapatkan PPM

  // Baca sensor dust menggunakan library GP2YDustSensor
  float dustDensityUgM3 = dustSensor.getDustDensity();      // Instant reading (ug/m³)
  float dustRunningAvg = dustSensor.getRunningAverage();    // Running average (ug/m³)
  
  // WiFi status
  bool wifiOn = (WiFi.status() == WL_CONNECTED);

  // Status gas MQ-135
  int airStatus = AIR_NORMAL;
  if (ppmEstimate >= PPM_DANGER_THRESHOLD) {
    airStatus = AIR_DANGER;
  } else if (ppmEstimate >= PPM_WARNING_THRESHOLD) {
    airStatus = AIR_WARNING;
  }

  // Hysteresis untuk fan dari gas (MQ135)
  if (ppmEstimate >= PPM_DANGER_THRESHOLD) {
    fanActiveFromGas = true;
  } else if (ppmEstimate < PPM_NORMAL_THRESHOLD) {
    fanActiveFromGas = false;
  }

  // Hysteresis untuk fan dari debu (GP2Y1010AU0F)
  if (dustRunningAvg >= DUST_DANGER_THRESHOLD_UGM3) {
    fanActiveFromDust = true;
  } else if (dustRunningAvg < DUST_NORMAL_THRESHOLD_UGM3) {
    fanActiveFromDust = false;
  }

  // Status debu
  int dustStatus = getDustStatus(dustRunningAvg);
  float dustDensityMgM3 = dustRunningAvg / 1000.0f;
  const char* dustStatusLabel = getDustStatusLabel(dustStatus);
  
  // Sistem membaca status yang lebih parah antara gas dan debu
  int systemStatus = airStatus;
  if (dustStatus > systemStatus) {
    systemStatus = dustStatus;
  }

  // Kontrol fan, buzzer, dan relay
  bool fanOn = fanActiveFromGas || fanActiveFromDust;
  bool buzzerOn = updateBuzzerPattern(systemStatus);
  bool greenRelayOn = (systemStatus == AIR_NORMAL);
  bool yellowRelayOn = (systemStatus == AIR_WARNING);
  bool redRelayOn = (systemStatus == AIR_DANGER);

  // Aktifkan hardware
  digitalWrite(BUZZER_PIN, buzzerOn ? HIGH : LOW);
  digitalWrite(FAN_RELAY_PIN, fanOn ? HIGH : LOW);
  digitalWrite(GREEN_RELAY_PIN, greenRelayOn ? HIGH : LOW);
  digitalWrite(YELLOW_RELAY_PIN, yellowRelayOn ? HIGH : LOW);
  digitalWrite(RED_RELAY_PIN, redRelayOn ? HIGH : LOW);
  
  // Update LCD
  updateDisplay(ppmEstimate, getAirStatusLabel(airStatus), wifiOn, dustDensityMgM3, dustStatusLabel, fanOn, greenRelayOn, yellowRelayOn, redRelayOn);

  // Serial monitor debug
  Serial.print("MQ-135 DO:");
  Serial.print(nilaiDigital);
  Serial.print(" | ADC:");
  Serial.print(nilaiAnalog);
  Serial.print(" | Volt:");
  Serial.print(voltageAO, 3);
  Serial.print("V | PPM:");
  Serial.print(ppmEstimate, 0);
  Serial.print(" | Status:");
  Serial.print(getAirStatusLabel(airStatus));
  Serial.print(" | Dust:");
  Serial.print(dustDensityUgM3, 0);
  Serial.print(" ug/m3 | DustAvg:");
  Serial.print(dustRunningAvg, 0);
  Serial.print(" ug/m3 (");
  Serial.print(dustDensityMgM3, 2);
  Serial.print(" mg/m3) | ");
  Serial.print(dustStatusLabel);
  Serial.print(" | Fan:");
  Serial.print(fanOn ? "ON" : "OFF");
  Serial.print(" (Gas:");
  Serial.print(fanActiveFromGas ? "Y" : "N");
  Serial.print(" Dust:");
  Serial.print(fanActiveFromDust ? "Y" : "N");
  Serial.print(") | Buzzer:");
  Serial.print(buzzerOn ? (systemStatus == AIR_DANGER ? "CONT" : "FLASH") : "OFF");
  Serial.print(" | LED:");
  Serial.print(getLEDStatusLabel(greenRelayOn, yellowRelayOn, redRelayOn));
  Serial.print(" | WiFi:");
  Serial.println(wifiOn ? "ON" : "OFF");

  // Upload ke ThingSpeak setiap 15 detik
  unsigned long now = millis();
  if (now - lastThingSpeakUpdate >= THINGSPEAK_UPDATE_INTERVAL_MS) {
    lastThingSpeakUpdate = now;
    sendToThingSpeak(nilaiAnalog, voltageAO, ppmEstimate, airStatus != AIR_NORMAL, dustRunningAvg, fanOn);
  }

  delay(200);
}

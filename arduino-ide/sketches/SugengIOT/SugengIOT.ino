// install library yang dibutuhkan: LiquidCrystal_I2C, WiFi, HTTPClient
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
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
#define LCD_SDA_PIN 21
#define LCD_SCL_PIN 22
#define THINGSPEAK_UPDATE_INTERVAL_MS 15000UL // 15 detik batas waktu update ThingSpeak
#define PPM_WARNING_THRESHOLD 50.0f // ambang batas PPM untuk status warning
#define PPM_DANGER_THRESHOLD 150.0f // ambang batas PPM untuk status danger
#define DUST_THRESHOLD_ADC 800 // ambang batas ADC untuk status warning debu

// inisialisasi LCD I2C 20x4 
LiquidCrystal_I2C lcd(0x27, 20, 4);
unsigned long lastThingSpeakUpdate = 0;
unsigned long lastBuzzerToggle = 0;
bool buzzerState = false;

// Fungsi untuk membaca nilai analog MQ135 dengan rata-rata
int readMQ135AnalogAverage() {
  long total = 0;
  for (int i = 0; i < 16; i++) {
    total += analogRead(MQ135_AO_PIN);
    delay(2);
  }
  return total / 16;
}

// Fungsi untuk mengubah nilai analog menjadi tegangan (Volt)
float analogToVoltage(int analogValue) {
  return analogValue * (3.3f / 4095.0f);
}

// Fungsi untuk memperkirakan nilai PPM MQ135
float estimateMQ135PPM(int analogValue) {
  float voltage = analogToVoltage(analogValue);
  return voltage * 300.0f;
}

// Fungsi untuk memperkirakan kepadatan debu (mg/m3) dari tegangan debu
float estimateDustDensity(float dustVoltage) {
  float densityMgM3 = (dustVoltage - 0.6f) / 0.005f;
  if (densityMgM3 < 0.0f) {
    densityMgM3 = 0.0f;
  }
  return densityMgM3;
}

// Fungsi untuk membaca nilai analog debu
int readDustAnalogRaw() {
  digitalWrite(DUST_LED_PIN, LOW);
  delayMicroseconds(280);
  int raw = analogRead(DUST_VO_PIN);
  delayMicroseconds(40);
  digitalWrite(DUST_LED_PIN, HIGH);
  delayMicroseconds(9680);
  return raw;
}

// Fungsi untuk mendapatkan label status udara berdasarkan nilai status
#define AIR_NORMAL 0
#define AIR_WARNING 1
#define AIR_DANGER 2

// Fungsi untuk mendapatkan label status udara berdasarkan nilai status
const char* getAirStatusLabel(int status) {
  if (status == AIR_WARNING) {
    return "warning";
  }
  if (status == AIR_DANGER) {
    return "danger";
  }
  return "normal";
}

// Fungsi untuk mengatur pola buzzer berdasarkan status udara
bool updateBuzzerPattern(int airStatus) {
  unsigned long now = millis();

  if (airStatus == AIR_DANGER) {
    buzzerState = true;
    return true;
  }

  if (airStatus != AIR_WARNING) {
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

// Fungsi untuk memperbarui tampilan LCD dengan informasi terbaru
void updateDisplay(float ppmValue, const char* airStatusLabel, bool wifiOn, float dustDensity, bool fanOn, bool greenOn, bool yellowOn, bool redOn) {
  lcd.clear();
  
  // Baris 1: PPM dan status
  lcd.setCursor(0, 0);
  lcd.print("PPM:");
  lcd.print(ppmValue, 0);
  lcd.print(" | ");
  lcd.print(airStatusLabel);
  
  // Baris 2: Debu density
  lcd.setCursor(0, 1);
  lcd.print("Dust:");
  lcd.print(dustDensity, 1);
  lcd.print(" mg/m3");
  
  // Baris 3: Status relay
  lcd.setCursor(0, 2);
  lcd.print("LED:");
  lcd.print(greenOn ? "G" : "-");
  lcd.print(yellowOn ? "Y" : "-");
  lcd.print(redOn ? "R" : "-");
  lcd.print(" Fan:");
  lcd.print(fanOn ? "ON " : "OFF");
  
  // Baris 4: WiFi status
  lcd.setCursor(0, 3);
  lcd.print("WiFi:");
  lcd.print(wifiOn ? "ON " : "OFF");
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
void sendToThingSpeak(int analogValue, float voltageAO, float ppmEstimate, bool gasDetected, float dustDensity, bool fanOn) {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("ThingSpeak skipped: WiFi down");
    return;
  }

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
  url += String(dustDensity, 3);
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
  pinMode(MQ135_DO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  pinMode(GREEN_RELAY_PIN, OUTPUT);
  pinMode(YELLOW_RELAY_PIN, OUTPUT);
  pinMode(RED_RELAY_PIN, OUTPUT);
  pinMode(DUST_LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(FAN_RELAY_PIN, LOW);
  digitalWrite(GREEN_RELAY_PIN, LOW);
  digitalWrite(YELLOW_RELAY_PIN, LOW);
  digitalWrite(RED_RELAY_PIN, LOW);
  digitalWrite(DUST_LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);

  analogReadResolution(12);
  analogSetPinAttenuation(MQ135_AO_PIN, ADC_11db);
  analogSetPinAttenuation(DUST_VO_PIN, ADC_11db);

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

  // debuggin pada console atau log terminal
  Serial.println("Sugeng IOT start");
  Serial.println("Wiring test awal aktif");
  Serial.println("MQ-135 DO -> GPIO15");
  Serial.println("MQ-135 AO -> GPIO34");
  Serial.println("LCD SDA -> GPIO21");
  Serial.println("LCD SCL -> GPIO22");
  Serial.println("Buzzer -> GPIO27");
  Serial.println("Relay fan -> GPIO26");
  Serial.println("Relay hijau -> GPIO14");
  Serial.println("Relay kuning -> GPIO12");
  Serial.println("Relay merah -> GPIO13");
  Serial.println("PPM 0-49 hijau | 50-149 kuning + buzzer ritme | >=150 fan + buzzer panjang");
  Serial.println("Dust >= 800 paksa kuning + fan ON");
  Serial.println("AO dibaca dengan ADC 12-bit + averaging");
  Serial.println("ThingSpeak field1=ADC field2=Volt field3=PPM field4=Gas field5=DustDensity field6=Fan");
  Serial.println("Test debu: Vo -> GPIO35 | LED -> GPIO25");

  connectWiFi();
}

// fungsi utama looping atau pengulanangan pengecekan pada esp dan alatnya
void loop() {
  // int inisiasi penyimpanan interger / numberic pasti (1,2,3,0)
  int nilaiDigital = digitalRead(MQ135_DO_PIN);
  int nilaiAnalog = readMQ135AnalogAverage();
  int dustRaw = readDustAnalogRaw();

  // inisiasi penyimpanan angka decimal koma dibelakang 
  float voltageAO = analogToVoltage(nilaiAnalog);
  float dustVoltage = analogToVoltage(dustRaw);
  float dustDensity = estimateDustDensity(dustVoltage);
  float ppmEstimate = estimateMQ135PPM(nilaiAnalog);

  // fungsi penyimpanan logika true / false 
  bool dustDetected = (dustRaw >= DUST_THRESHOLD_ADC);
  bool wifiOn = (WiFi.status() == WL_CONNECTED);

  // pengecekan status apakah sudah sesuai dengan batas yang ditentukan atau belum
  int airStatus = AIR_NORMAL;
  if (ppmEstimate >= PPM_DANGER_THRESHOLD) {
    airStatus = AIR_DANGER;
  } else if (ppmEstimate >= PPM_WARNING_THRESHOLD) {
    airStatus = AIR_WARNING;
  }

  // inisiasi lagi penggunaan apakah perlu nyala atau tidak jika telah memenuhi suatu kondisi yang telah ditentukan
  bool fanOn = (airStatus == AIR_DANGER) || dustDetected;
  bool buzzerOn = updateBuzzerPattern(airStatus);
  bool greenRelayOn = (airStatus == AIR_NORMAL) && !dustDetected;
  bool yellowRelayOn = (airStatus == AIR_WARNING) || dustDetected;
  bool redRelayOn = (airStatus == AIR_DANGER);

  // inisiasi tampilan dari led yang mana sesuai keadaan yang didapatkan dari pengecekan 
  digitalWrite(BUZZER_PIN, buzzerOn ? HIGH : LOW);
  digitalWrite(FAN_RELAY_PIN, fanOn ? HIGH : LOW);
  digitalWrite(GREEN_RELAY_PIN, greenRelayOn ? HIGH : LOW);
  digitalWrite(YELLOW_RELAY_PIN, yellowRelayOn ? HIGH : LOW);
  digitalWrite(RED_RELAY_PIN, redRelayOn ? HIGH : LOW);
  updateDisplay(ppmEstimate, getAirStatusLabel(airStatus), wifiOn, dustDensity, fanOn, greenRelayOn, yellowRelayOn, redRelayOn);

  // console log debuggin tampilan untuk mengecek adanya error atau tidak
  Serial.print("Nilai DO MQ-135: ");
  Serial.print(nilaiDigital);
  Serial.print(" | ADC AO: ");
  Serial.print(nilaiAnalog);
  Serial.print(" | AO Volt: ");
  Serial.print(voltageAO, 3);
  Serial.print("V | PPM : ");
  Serial.print(ppmEstimate, 0);
  Serial.print(" | ");
  Serial.print(getAirStatusLabel(airStatus));
  Serial.print(" | Wifi : ");
  Serial.print(wifiOn ? "ON" : "OFF");
  Serial.print(" | Dust ADC: ");
  Serial.print(dustRaw);
  Serial.print(" | Dust Volt: ");
  Serial.print(dustVoltage, 3);
  Serial.print("V | Dust Density: ");
  Serial.print(dustDensity, 3);
  Serial.print(" mg/m3 | Dust Status: ");
  Serial.print(dustDetected ? "warning" : "normal");
  Serial.print(" | Fan: ");
  Serial.print(fanOn ? "ON" : "OFF");
  Serial.print(" | Buzzer: ");
  Serial.print(buzzerOn ? (airStatus == AIR_DANGER ? "CONTINUOUS" : "FLASHING") : "OFF");
  Serial.print(" | Relay Hijau: ");
  Serial.print(greenRelayOn ? "ON" : "OFF");
  Serial.print(" | Relay Kuning: ");
  Serial.print(yellowRelayOn ? "ON" : "OFF");
  Serial.print(" | Relay Merah: ");
  Serial.println(redRelayOn ? "ON" : "OFF");


  // fungsi mengirim sesuai interval atau ambang batas yang ditentukan diatas yaitu update per 15 detik mengirim data
  unsigned long now = millis();
  if (now - lastThingSpeakUpdate >= THINGSPEAK_UPDATE_INTERVAL_MS) {
    lastThingSpeakUpdate = now;
    sendToThingSpeak(nilaiAnalog, voltageAO, ppmEstimate, airStatus != AIR_NORMAL, dustDensity, fanOn);
  }

  delay(200);
}

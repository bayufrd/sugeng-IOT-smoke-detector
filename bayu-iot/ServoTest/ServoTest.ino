#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <time.h>

static const char* WIFI_SSID = "MOMO32";
static const char* WIFI_PASSWORD = "p@keotak!";
static const IPAddress LOCAL_IP(192, 168, 1, 110);
static const IPAddress GATEWAY(192, 168, 1, 1);
static const IPAddress SUBNET(255, 255, 255, 0);
static const IPAddress PRIMARY_DNS(8, 8, 8, 8);
static const IPAddress SECONDARY_DNS(1, 1, 1, 1);

static const char* CALLBACK_URL = "https://api-whatsapp-bot.dastrevas.com/api/whatsapp/send-personal";
static const char* CALLBACK_BEARER_TOKEN = "Yv8u1hFjwaHAuwx2dgBeqyi9TGqeAEKgLUcYlg1RO2hKCdGStqaAGQOrckQ8dgaqVU9gQgztuIfFJgHJ49paAPlErZvyg76YVh3oI13k5QAyH4Xt3EGww484kJaDqky3yTkP0riwtT3VIi92I4tplFpsXxIYaoSA1TlY7mw5TDqoHgXFEUl1DgUOlk7Sdf2oweGcK26TapsTYbHgAz7VEVfBOPiwqcRyZv";

static const int SERVER_PORT = 80;
static const int SERVO_PIN = 18;
static const int TOUCH_PIN = 27;
static const int BUZZER_PIN = 26;
static const int VIBRO1_PIN = 25;
static const int VIBRO2_PIN = 33;
static const int SERVO_OPEN_ANGLE = 90;
static const int SERVO_CLOSE_ANGLE = 0;

Servo feederServo;
WebServer server(SERVER_PORT);
bool lastTouchState = false;
bool feedInProgress = false;
unsigned long lastFeedAt = 0;
String lastFeedSource = "none";
String lastFeedNumber = "";

String extractJsonString(const String& body, const String& key) {
  String pattern = "\"" + key + "\"";
  int keyIndex = body.indexOf(pattern);
  if (keyIndex < 0) {
    return "";
  }

  int colonIndex = body.indexOf(':', keyIndex + pattern.length());
  if (colonIndex < 0) {
    return "";
  }

  int firstQuote = body.indexOf('"', colonIndex + 1);
  if (firstQuote < 0) {
    return "";
  }

  int secondQuote = body.indexOf('"', firstQuote + 1);
  while (secondQuote > 0 && body.charAt(secondQuote - 1) == '\\') {
    secondQuote = body.indexOf('"', secondQuote + 1);
  }

  if (secondQuote < 0) {
    return "";
  }

  String value = body.substring(firstQuote + 1, secondQuote);
  value.replace("\\\"", "\"");
  value.replace("\\n", "\n");
  return value;
}

String jsonEscape(String value) {
  value.replace("\\", "\\\\");
  value.replace("\"", "\\\"");
  value.replace("\n", "\\n");
  value.replace("\r", "\\r");
  return value;
}

String getTimeLabel() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 1000)) {
    return String(millis() / 1000) + "s";
  }

  char buffer[32];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}

void moveServoTo(int angle) {
  Serial.print("Move servo to: ");
  Serial.println(angle);
  feederServo.write(angle);
  delay(1000);
}

void beepOnce() {
  Serial.println("Buzzer beep once");
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
}

void runVibrationMotors(unsigned long durationMs) {
  Serial.println("Vibration motors ON");
  digitalWrite(VIBRO1_PIN, HIGH);
  digitalWrite(VIBRO2_PIN, HIGH);
  delay(durationMs);
  digitalWrite(VIBRO1_PIN, LOW);
  digitalWrite(VIBRO2_PIN, LOW);
  Serial.println("Vibration motors OFF");
}

bool sendSuccessCallback(const String& waNumber, const String& source) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Skip callback: WiFi disconnected");
    return false;
  }

  if (waNumber.length() == 0) {
    Serial.println("Skip callback: empty waNumber");
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  if (!http.begin(client, CALLBACK_URL)) {
    Serial.println("HTTP begin callback failed");
    return false;
  }

  String message = "Kasih makan anak berhasil pada jam " + getTimeLabel() + " via " + source;
  String payload = "{\"number\":\"" + jsonEscape(waNumber) + "\",\"message\":\"" + jsonEscape(message) + "\"}";
  String authHeader = "Bearer ";
  authHeader += CALLBACK_BEARER_TOKEN;

  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", authHeader);

  int httpCode = http.POST(payload);
  Serial.print("Callback HTTP code: ");
  Serial.println(httpCode);
  if (httpCode > 0) {
    Serial.println(http.getString());
  }
  http.end();

  return httpCode > 0 && httpCode < 300;
}

void feedOnce(const String& source, const String& waNumber) {
  if (feedInProgress) {
    Serial.println("Feed request ignored: still in progress");
    return;
  }

  feedInProgress = true;
  lastFeedSource = source;
  lastFeedNumber = waNumber;
  lastFeedAt = millis();

  Serial.print("Feed triggered by: ");
  Serial.println(source);
  runVibrationMotors(5000);
  beepOnce();
  moveServoTo(SERVO_OPEN_ANGLE);
  delay(1500);
  moveServoTo(SERVO_CLOSE_ANGLE);
  sendSuccessCallback(waNumber, source);
  feedInProgress = false;
}

void handleRoot() {
  server.send(200, "application/json", "{\"ok\":true,\"service\":\"pet-feeder\",\"ip\":\"192.168.1.110\"}");
}

void handleFeedApi() {
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"ok\":false,\"message\":\"body required\"}");
    return;
  }

  String body = server.arg("plain");
  Serial.println("Incoming API payload:");
  Serial.println(body);

  String command = extractJsonString(body, "command");
  String waNumber = extractJsonString(body, "waNumber");

  if (command != "makan") {
    server.send(400, "application/json", "{\"ok\":false,\"message\":\"unsupported command\"}");
    return;
  }

  feedOnce("api", waNumber);
  server.send(200, "application/json", "{\"ok\":true,\"message\":\"feed triggered\",\"source\":\"api\"}");
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);
  delay(300);

  if (!WiFi.config(LOCAL_IP, GATEWAY, SUBNET, PRIMARY_DNS, SECONDARY_DNS)) {
    Serial.println("Static IP config failed");
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting WiFi");

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 40) {
    delay(500);
    Serial.print(".");
    retries++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connect failed");
  }
}

void setupTime() {
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
}

void setupApiServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/feed", HTTP_POST, handleFeedApi);
  server.begin();
  Serial.print("HTTP server started on port ");
  Serial.println(SERVER_PORT);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Servo + touch + buzzer + WiFi API starting...");

  pinMode(TOUCH_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(VIBRO1_PIN, OUTPUT);
  pinMode(VIBRO2_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(VIBRO1_PIN, LOW);
  digitalWrite(VIBRO2_PIN, LOW);

  feederServo.setPeriodHertz(50);
  feederServo.attach(SERVO_PIN, 500, 2400);
  moveServoTo(SERVO_CLOSE_ANGLE);

  connectWiFi();
  setupTime();
  setupApiServer();
}

void loop() {
  server.handleClient();

  bool touchState = digitalRead(TOUCH_PIN) == HIGH;
  if (touchState && !lastTouchState) {
    feedOnce("touch", "");
  }

  lastTouchState = touchState;
  delay(50);
}
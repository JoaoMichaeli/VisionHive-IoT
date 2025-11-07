#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Joao";
const char* password = "joao1234";

const char* javaServerUrl = "http://visionhive.brazilsouth.azurecontainer.io:8080/api";

#define BUZZER_PIN 12
#define LED_PIN 2
#define VOLTAGE_PIN 35

bool buzzerLigado = false;

float readSupplyVoltage() {
  int raw = analogRead(VOLTAGE_PIN);
  return (raw / 4095.0) * 3.3 * 1.1;
}
int getSignalQuality() {
  int rssi = WiFi.RSSI();
  if (rssi >= -50) return 100;
  if (rssi <= -100) return 0;
  return 2 * (rssi + 100);
}

void acionarBuzzerESP() {
  if (!buzzerLigado) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    buzzerLigado = true;
    Serial.println("Buzzer e LED LIGADOS pelo comando do Java.");
  }
}
void pararBuzzerESP() {
  if (buzzerLigado) {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    buzzerLigado = false;
    Serial.println("Buzzer e LED DESLIGADOS pelo comando do Java.");
  }
}

void verificarComandoJava() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(javaServerUrl) + "/comando-global-esp";
    http.begin(url);
    Serial.print("Verificando comando em: ");
    Serial.println(url);

    int httpCode = http.GET();

      if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.print("Resposta do Java: ");
      Serial.println(payload);

      DynamicJsonDocument doc(256);
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print(F("Falha ao analisar JSON: "));
        Serial.println(error.c_str());
        return;
      }

      String comando = doc["comando"].as<String>();

      if (comando == "ACIONAR") {
        acionarBuzzerESP();
      } else if (comando == "PARAR") {
        pararBuzzerESP();
      } else {
        Serial.print("Comando desconhecido ou nulo: ");
        Serial.println(comando);
      }
    } else {
      Serial.print("Erro na requisição HTTP para o Java (código): ");
      Serial.println(httpCode);
    }
    http.end();
  } else {
    Serial.println("Wi-Fi desconectado. Tentando reconectar...");
    WiFi.begin(ssid, password);
  }
}
void enviarStatusJava() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(javaServerUrl) + "/esp-status-report";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

      DynamicJsonDocument doc(512);

      doc["rssi"] = WiFi.RSSI();
      doc["signalQuality"] = getSignalQuality();
      doc["voltage"] = readSupplyVoltage();
      doc["uptimeDays"] = (millis() / 1000) / 86400;
      doc["uptimeHours"] = (millis() / 1000) / 3600;
      doc["ping"] = 1.2 + (rand() % 8) / 10.0;
      doc["lastUpdate"] = millis();

    String jsonPayload;
    serializeJson(doc, jsonPayload);

    Serial.print("Enviando status para Java: ");
    Serial.println(jsonPayload);

    int httpCode = http.POST(jsonPayload);

    if (httpCode == HTTP_CODE_OK) {
      Serial.println("Status enviado com sucesso para o Java!");
    } else {
      Serial.print("Erro ao enviar status para o Java (código): ");
      Serial.println(httpCode);
      Serial.println(http.getString());
    }
    http.end();
  }
}

void setup() {
  Serial.begin(115200);
  randomSeed(millis());
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(VOLTAGE_PIN, INPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP do ESP32 na rede local: ");
  Serial.println(WiFi.localIP());
  Serial.println("ESP32 pronto para verificar comandos e enviar status.");
}

void loop() {

  static unsigned long lastCommandCheckTime = 0;
  if (millis() - lastCommandCheckTime > 2000) {
    verificarComandoJava();
    lastCommandCheckTime = millis();
  }

  static unsigned long lastStatusReportTime = 0;
  if (millis() - lastStatusReportTime > 5000) {
    enviarStatusJava();
    lastStatusReportTime = millis();
  }
}
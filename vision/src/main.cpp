#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic_telemetry = "moto/001/telemetry";
const char* mqtt_topic_commands = "moto/001/commands";

#define PIR_PIN 13
#define BUZZER_PIN 4
#define NEOPIXEL_PIN 2
#define BUTTON_PIN 12

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_NeoPixel pixel(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

bool motionDetected = false;
bool alarmActive = false;
bool locatingActive = false;
unsigned long lastPublish = 0;

void setLEDColor(String color) {
  if (color == "RED") {
    pixel.setPixelColor(0, pixel.Color(255, 0, 0));
  } else if (color == "GREEN") {
    pixel.setPixelColor(0, pixel.Color(0, 255, 0));
  } else if (color == "BLUE") {
    pixel.setPixelColor(0, pixel.Color(0, 0, 255));
  } else if (color == "YELLOW") {
    pixel.setPixelColor(0, pixel.Color(255, 255, 0));
  } else {
    pixel.setPixelColor(0, pixel.Color(0, 0, 0));
  }
  pixel.show();
}

void activateLocating() {
  locatingActive = true;
  alarmActive = false;
  setLEDColor("YELLOW");
  Serial.println("🔍 MODO LOCALIZAÇÃO ATIVADO - Buzzer piscando!");
}

void deactivateLocating() {
  locatingActive = false;
  digitalWrite(BUZZER_PIN, LOW);
  setLEDColor("GREEN");
  Serial.println("✅ MODO LOCALIZAÇÃO DESATIVADO");
}

void activateAlarm() {
  alarmActive = true;
  setLEDColor("RED");
  Serial.println("Alarme ativado!");
}

void deactivateAlarm() {
  alarmActive = false;
  digitalWrite(BUZZER_PIN, LOW);
  setLEDColor("GREEN");
  Serial.println("Alarme desativado!");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("Comando recebido: ");
  Serial.println(message);
  
  if (message == "LOCATE_ON") {
    activateLocating();
  } else if (message == "LOCATE_OFF") {
    deactivateLocating();
  } else if (message == "ALARM_OFF") {
    deactivateAlarm();
    deactivateLocating();
  }
}

void setup_wifi() {
  delay(10);
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    String clientId = "ESP32-Moto-001";
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");
      client.subscribe(mqtt_topic_commands);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  pixel.begin();
  pixel.setBrightness(50);
  setLEDColor("BLUE");
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  Serial.println("=== SISTEMA DE MONITORAMENTO DE MOTO INICIALIZADO ===");
  Serial.println("Aguardando comandos do dashboard...");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  bool currentMotion = digitalRead(PIR_PIN);
  if (currentMotion && !motionDetected) {
    motionDetected = true;
    Serial.println("🚨 MOVIMENTO DETECTADO!");
  } else if (!currentMotion && motionDetected) {
    motionDetected = false;
  }
  
  if (locatingActive) {
    static unsigned long lastBeep = 0;
    if (millis() - lastBeep > 1000) {
      digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));
      lastBeep = millis();
    }
  }
  
  if (alarmActive) {
    digitalWrite(BUZZER_PIN, HIGH);
  }
  
  if (millis() - lastPublish > 3000) {
    String payload = "{";
    payload += "\"motion\":" + String(motionDetected ? "true" : "false");
    payload += ",\"alarm\":" + String(alarmActive ? "true" : "false");
    payload += ",\"locating\":" + String(locatingActive ? "true" : "false");
    payload += ",\"battery\":" + String(random(85, 100));
    payload += ",\"timestamp\":" + String(millis());
    payload += "}";
    
    client.publish(mqtt_topic_telemetry, payload.c_str());
    Serial.println("📊 Telemetria: " + payload);
    lastPublish = millis();
  }
  
  if (digitalRead(BUTTON_PIN) == LOW) {
    deactivateAlarm();
    deactivateLocating();
    setLEDColor("GREEN");
    delay(500);
  }
  
  delay(100);
}
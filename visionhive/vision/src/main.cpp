#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

// ========== Protótipos de Função ==========
void conectarWiFi();
void conectarMQTT();
void reconectarMQTT();
void tratarMensagemMQTT(char* topico, byte* payload, unsigned int tamanho);
void ativarAlarme();
void desativarAlarme();
void publicarStatus(const char* status);

// ========== IDs do Dispositivo ==========
const char* ID_GRUPO = "VisionHive";
const char* ID_DISPOSITIVO = "MOTO_001";

// ========== Configurações Wi-Fi ==========
const char* SSID_WIFI = "Wokwi-GUEST";
const char* SENHA_WIFI = "";

// ========== Configurações MQTT ==========
const char* BROKER_MQTT = "broker.hivemq.com";
const int PORTA_MQTT = 1883;
const char* USUARIO_MQTT = "";
const char* SENHA_MQTT = "";

// ========== Tópicos MQTT ==========
const char* TOPICO_SUBSCRIBE = "visionhive/comandos";
const char* TOPICO_PUBLISH = "visionhive/status";

// ========== Hardware ==========
#define PINO_BUZZER 4
#define PINO_NEOPIXEL 2

// ========== Variáveis de Estado ==========
bool alarmeAtivo = false;

Adafruit_NeoPixel neoPixel(1, PINO_NEOPIXEL, NEO_GRB + NEO_KHZ800);
WiFiClient clienteWiFi;
PubSubClient clienteMQTT(clienteWiFi);

// ========== Setup ==========
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Sistema de Localização VisionHive ===");
  Serial.println("Inicializando...");
  
  // Configuração do hardware
  pinMode(PINO_BUZZER, OUTPUT);
  neoPixel.begin();
  neoPixel.show();
  Serial.println("- Hardware configurado");

  // Sequência de inicialização
  conectarWiFi();
  conectarMQTT();
}

// ========== Loop Principal ==========
void loop() {
  if (!clienteMQTT.connected()) {
    reconectarMQTT();
  }
  clienteMQTT.loop();

  // Mantém o buzzer ativo enquanto o alarme estiver ligado
  if(alarmeAtivo) {
    tone(PINO_BUZZER, 2000); // Mantém o tom ativo
  }
}

// ========== Conexão Wi-Fi ==========
void conectarWiFi() {
  Serial.println("\n[WiFi] Iniciando conexão...");
  Serial.print("SSID: ");
  Serial.println(SSID_WIFI);
  
  neoPixel.setPixelColor(0, neoPixel.Color(0, 0, 255)); // Azul: conectando
  neoPixel.show();

  WiFi.begin(SSID_WIFI, SENHA_WIFI);

  unsigned long inicioConexao = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - inicioConexao < 15000) {
    Serial.print(".");
    delay(100);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WiFi] Conexão estabelecida!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[WiFi] Falha na conexão!");
    neoPixel.setPixelColor(0, neoPixel.Color(255, 0, 0)); // Vermelho: erro
    neoPixel.show();
    while(true);
  }
}

// ========== Conexão MQTT ==========
void conectarMQTT() {
  Serial.println("\n[MQTT] Conectando ao broker...");
  Serial.print("Broker: ");
  Serial.print(BROKER_MQTT);
  Serial.print(":");
  Serial.println(PORTA_MQTT);
  
  clienteMQTT.setServer(BROKER_MQTT, PORTA_MQTT);
  clienteMQTT.setCallback(tratarMensagemMQTT);

  String clientId = "ESP32-" + String(ID_DISPOSITIVO);
  Serial.print("Client ID: ");
  Serial.println(clientId);

  unsigned long inicioConexao = millis();
  
  while (!clienteMQTT.connect(clientId.c_str(), USUARIO_MQTT, SENHA_MQTT)) {
    Serial.print("[MQTT] Tentativa falhou. Estado: ");
    Serial.println(clienteMQTT.state());
    
    neoPixel.setPixelColor(0, neoPixel.Color(255, 165, 0)); // Laranja: tentando
    neoPixel.show();
    
    if (millis() - inicioConexao > 10000) break;
    delay(500);
  }

  if (clienteMQTT.connected()) {
    Serial.println("[MQTT] Conectado com sucesso!");
    clienteMQTT.subscribe(TOPICO_SUBSCRIBE);
    Serial.print("Inscrito no tópico: ");
    Serial.println(TOPICO_SUBSCRIBE);
    
    neoPixel.setPixelColor(0, neoPixel.Color(0, 255, 0)); // Verde: conectado
    neoPixel.show();
    publicarStatus("CONECTADO");
  } else {
    Serial.println("[MQTT] Falha na conexão permanente!");
    neoPixel.setPixelColor(0, neoPixel.Color(255, 0, 0)); // Vermelho: erro
    neoPixel.show();
    while(true);
  }
}

// ========== Reconexão MQTT ==========
void reconectarMQTT() {
  Serial.println("\n[MQTT] Conexão perdida! Reconectando...");
  neoPixel.setPixelColor(0, neoPixel.Color(255, 255, 0)); // Amarelo: reconectando
  neoPixel.show();
  conectarMQTT();
}

// ========== Tratamento de Mensagens ==========
void tratarMensagemMQTT(char* topico, byte* payload, unsigned int tamanho) {
  Serial.println("\n[MQTT] Nova mensagem recebida");
  Serial.print("Tópico: ");
  Serial.println(topico);
  
  char mensagem[tamanho + 1];
  memcpy(mensagem, payload, tamanho);
  mensagem[tamanho] = '\0';
  
  Serial.print("Conteúdo: ");
  Serial.println(mensagem);

  if (strcmp(mensagem, "LOCALIZAR") == 0) {
    alarmeAtivo = true;
    ativarAlarme();
  } else if (strcmp(mensagem, "PAUSAR") == 0) {
    alarmeAtivo = false;
    desativarAlarme();
  } else if (strcmp(mensagem, "RESET") == 0) {
    publicarStatus("REINICIANDO");
    delay(100);
    ESP.restart();
  }
}

// ========== Ativação do Alarme ==========
void ativarAlarme() {
  Serial.println("\n[ALARME] Ativando sistema...");
  publicarStatus("ALARME_ATIVO");
  
  neoPixel.setPixelColor(0, neoPixel.Color(255, 0, 0)); // Vermelho
  neoPixel.show();
  tone(PINO_BUZZER, 2000);
}

// ========== Desativação do Alarme ==========
void desativarAlarme() {
  Serial.println("\n[ALARME] Desativando sistema...");
  publicarStatus("ALARME_DESATIVADO");
  
  noTone(PINO_BUZZER);
  neoPixel.setPixelColor(0, neoPixel.Color(0, 255, 0)); // Verde
  neoPixel.show();
}

// ========== Publicação de Status ==========
void publicarStatus(const char* status) {
  String mensagem = String(ID_GRUPO) + "|" + String(ID_DISPOSITIVO) + "|" + status;
  Serial.print("[STATUS] Publicando: ");
  Serial.println(mensagem);
  
  clienteMQTT.publish(TOPICO_PUBLISH, mensagem.c_str());
}
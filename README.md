# VisionHive IoT System

## Visão Geral do Sistema

O sistema IoT VisionHive integra um dispositivo ESP32 com um backend Spring Boot para monitorar e controlar remotamente um buzzer e um LED, onde ele é fixado ao guidom de várias motocicletas em um pátio extremamente lotado, dificultando a localização de motos exatas, e ao ser acionado ele ativar o buzzer e o led, ou seja, você saberá o local exato da motocicleta que procura.

# Video demonstração:

[https://youtu.be/FOvRFe8t4co](https://youtu.be/FOvRFe8t4co)

# Link para acessar a aplicação:

[http://visionhive.brazilsouth.azurecontainer.io:8080/login](http://visionhive.brazilsouth.azurecontainer.io:8080/login)

## Login para testes como admin

- Login:
  ```adminCM```
- Senha:
  ```admin123```

## Login para testes como operador

- Login:
  ```operadorCM```
- Senha:
  ```operador123```

### Componentes Principais:

1.  **Dispositivo ESP32:** Responsável por interagir com os hardwares Buzzer e LED, conectar-se à rede Wi-Fi e comunicar-se com o backend Java.
2.  **Backend Spring Boot (Java):** Uma aplicação web que expõe endpoints REST para controlar o buzzer, receber o status do ESP32 e registrar logs das ações.

## Componentes Detalhados

### 1. Dispositivo ESP32

O código do ESP32 (`.ino`) gerencia a conexão Wi-Fi, controla o buzzer e o LED, e se comunica com o servidor Java para:

*   **Verificar comandos:** A cada 2 segundos, o ESP32 consulta o endpoint `/api/comando-global-esp` para verificar se há um comando para "ACIONAR" ou "PARAR" o buzzer.
*   **Enviar status:** A cada 5 segundos, o ESP32 envia dados de status (qualidade do sinal Wi-Fi, tensão da fonte, uptime) para o endpoint `/api/esp-status-report`.

**Hardware Utilizado:**

*   **`BUZZER_PIN` (GPIO 12):** Conectado ao buzzer.
*   **`LED_PIN` (GPIO 2):** Conectado a um LED.
*   **`VOLTAGE_PIN` (GPIO 35):** Entrada analógica para leitura da tensão da fonte.

**Configuração Essencial:**

*   `const char* ssid = "SuaRedeWifi";`
*   `const char* password = "SuaSenhaWifi";`
*   `const char* javaServerUrl = "http://visionhive.brazilsouth.azurecontainer.io:8080";` (URL base do seu backend Spring Boot)

### 2. Backend Spring Boot (Java)

O backend é construído com Spring Boot e fornece as seguintes funcionalidades:

*   **`BuzzerController`:**
    *   **`/api/acionar-buzzer/moto/{id}` (GET):** Aciona o buzzer globalmente e registra a ação no banco de dados associada a uma motocicleta.
    *   **`/api/parar-buzzer/moto/{id}` (GET):** Para o buzzer globalmente e registra a ação no banco de dados.
    *   **`/api/comando-global-esp` (GET):** Endpoint consultado pelo ESP32 para obter o comando atual ("ACIONAR" ou "PARAR").
    *   **`/api/esp-status-report` (POST):** Recebe e armazena o status reportado pelo ESP32.
    *   **`/api/status` (GET):** Retorna o último status conhecido do ESP32, incluindo informações de conexão e timestamp da última atualização.

*   **`BuzzerLog` (Model):** Entidade JPA para persistência das ações do buzzer (ação, data/hora, IP do ESP32, placa da moto, sucesso/erro).

*   **`SecurityConfig`:**
    *   **`ApiSecurityConfig`:** Protege os endpoints `/api/**`, permitindo acesso público aos endpoints de comunicação do ESP32 (`/api/comando-global-esp`, `/api/esp-status-report`, `/api/status`) e restringindo outros endpoints a usuários com roles `ADMIN` ou `OPERADOR`.
    *   **`WebSecurityConfig`:** Configura a segurança para a aplicação web, incluindo login baseado em formulário, CSRF, e controle de acesso baseado em roles (`ADMIN`, `AUTHENTICATED`). Utiliza `BCryptPasswordEncoder` para segurança de senhas.

## Como Funciona?

1.  **Inicialização do ESP32:** O ESP32 se conecta à rede Wi-Fi configurada.
2.  **Reporte de Status:** Regularmente (a cada 5 segundos), o ESP32 envia dados de status (RSSI, tensão da fonte, uptime, etc.) para o backend Java através do endpoint `/api/esp-status-report`.
3.  **Verificação de Comando:** Regularmente (a cada 2 segundos), o ESP32 consulta o backend Java para verificar o estado global desejado para o buzzer (`/api/comando-global-esp`).
4.  **Controle do Buzzer:**
    *   Um usuário pode acionar ou parar o buzzer através dos endpoints `/api/acionar-buzzer/moto/{id}` ou `/api/parar-buzzer/moto/{id}` no backend Java.
    *   Essa ação atualiza o `estadoBuzzerGlobal` no backend e registra um log no banco de dados (`BuzzerLog`).
    *   Quando o ESP32 consulta o `/api/comando-global-esp` e detecta uma mudança no comando, ele aciona ou desaciona fisicamente o buzzer e o LED conectados.
5.  **Monitoramento:** O status atualizado do ESP32 pode ser verificado a qualquer momento através do endpoint `/api/status` no backend.

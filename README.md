
# Documentação do Sistema de Localização VisionHive

## Descrição Geral
Este projeto consiste em um sistema de localização embarcado desenvolvido para ESP32, que utiliza Wi-Fi e protocolo MQTT para comunicação. O objetivo é possibilitar o rastreamento ou localização de um dispositivo, como uma motocicleta, por meio de comandos enviados remotamente.

## Funcionalidades Principais
- Conexão à rede Wi-Fi.
- Comunicação com um broker MQTT (HiveMQ) para envio e recebimento de mensagens.
- Controle de um alarme sonoro (buzzer) e visual (NeoPixel) via comandos MQTT.
- Feedback visual do estado do dispositivo (conectando, conectado, erro, alarme ativo).

## Hardware Utilizado
- **ESP32**
- **Buzzer** (ativo)
- **NeoPixel** (1 LED RGB)
- Fonte de alimentação compatível

## Bibliotecas Utilizadas
- `WiFi.h`: Gerencia conexão Wi-Fi.
- `PubSubClient.h`: Comunicação MQTT.
- `Adafruit_NeoPixel.h`: Controle do LED NeoPixel.

## Estrutura do Código

### 1. Identificação do Dispositivo
- `ID_GRUPO`: Identifica o grupo, no exemplo `"VisionHive"`.
- `ID_DISPOSITIVO`: Identifica o dispositivo, no exemplo `"MOTO_001"`.

### 2. Configurações de Conexão
- **Wi-Fi:** 
  - SSID: `"Wokwi-GUEST"`
  - Senha: `""`
- **MQTT:** 
  - Broker: `broker.hivemq.com`
  - Porta: `1883`
  - Sem autenticação (usuário e senha vazios).

### 3. Tópicos MQTT
- **Subscribe:** `visionhive/comandos` — recebe comandos.
- **Publish:** `visionhive/status` — envia status do dispositivo.

### 4. Hardware
- Pino do **buzzer**: GPIO 4.
- Pino do **NeoPixel**: GPIO 2.

## Funcionalidades Implementadas

### Conexão Wi-Fi
- A função `conectarWiFi()` conecta à rede Wi-Fi.
- LED NeoPixel azul durante a tentativa.
- Verde se conectado, vermelho em caso de erro.

### Conexão MQTT
- Função `conectarMQTT()` conecta ao broker MQTT.
- Inscrição no tópico de comandos.
- LED NeoPixel laranja durante a tentativa e verde se conectado.

### Reconexão MQTT
- A função `reconectarMQTT()` é chamada caso a conexão MQTT caia.

### Tratamento de Mensagens
Quando uma mensagem é recebida no tópico `visionhive/comandos`, ela pode conter:

- **"LOCALIZAR"**: 
  - Ativa o alarme sonoro (buzzer em 2000Hz) e visual (LED vermelho).
  - Publica o status `ALARME_ATIVO`.

- **"PAUSAR"**:
  - Desativa o alarme e retorna o LED para verde.
  - Publica o status `ALARME_DESATIVADO`.

- **"RESET"**:
  - Publica `REINICIANDO` e reinicia o ESP32.

### Publicação de Status
Formato da mensagem publicada no tópico `visionhive/status`:
```
<ID_GRUPO>|<ID_DISPOSITIVO>|<STATUS>
```
Exemplo:
```
VisionHive|MOTO_001|ALARME_ATIVO
```

### Estados do LED (NeoPixel)
| Estado           | Cor do LED |
|------------------|-------------|
| Conectando Wi-Fi | Azul        |
| Erro Wi-Fi       | Vermelho    |
| Conectando MQTT  | Laranja     |
| Conectado        | Verde       |
| Alarme Ativo     | Vermelho    |
| Reconectando MQTT| Amarelo     |

## Fluxo de Funcionamento

1. Inicializa Wi-Fi e conecta.
2. Conecta ao broker MQTT.
3. Mantém conexão MQTT ativa.
4. Monitora comandos MQTT e responde aos comandos recebidos.
5. Publica estados operacionais no tópico de status.

---
**Desenvolvido por:** Projeto VisionHive

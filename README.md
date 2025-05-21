## Mapeamento Inteligente do Pátio - Mottu
Solução de IoT com ESP32 para identificação, rastreamento e visualização em tempo real da localização de motos nos pátios das mais de 100 filiais da Mottu no Brasil e no México.

# Documentação do Sistema de Localização VisionHive

# Descrição Geral
Este projeto tem como objetivo automatizar, otimizar e tornar mais eficiente a gestão das motos nos pátios da Mottu. A solução é baseada em um sistema embarcado utilizando microcontroladores ESP32 conectados via Wi-Fi, sensores físicos e um sistema web responsivo.

O funcionamento se dá por meio de um módulo de localização embarcado nos ESP32, que se comunica utilizando o protocolo MQTT através de redes Wi-Fi. Isso permite o rastreamento e a localização em tempo real dos veículos, como motocicletas, diretamente pela plataforma web. A comunicação é feita por meio de comandos enviados remotamente, tornando o processo mais ágil e confiável.

A proposta visa eliminar o controle manual, garantindo maior segurança, escalabilidade e precisão na operação dos pátios. Com isso, a localização, o monitoramento e a gestão dos veículos se tornam muito mais eficientes, reduzindo erros operacionais e aumentando a produtividade.

## Objetivo
- Identificar rapidamente uma moto específica no pátio utilizando um alerta visual e/ou sonoro.
- Fornecer uma visualização **em tempo real** da chamada e do status de resposta da moto.
- Garantir escalabilidade para aplicação em mais de 100 filiais com diferentes layouts.
- Oferecer uma interface intuitiva, acessível por desktop e mobile.
- Integrar sensores IoT nas motos para coleta automatizada de dados e status.

## Funcionalidades Principais
- Conexão à rede Wi-Fi.
- Comunicação com um broker MQTT (HiveMQ) para envio e recebimento de mensagens.
- Controle de um alarme sonoro (buzzer) e visual (NeoPixel) via comandos MQTT.
- Feedback visual do estado do dispositivo (conectando, conectado, erro, alarme ativo).

## Alimentação e Eficiência Energética

- A bateria **18650 recarregável (2500 mAh)** continua sendo a fonte principal de energia, garantindo autonomia prolongada.
- O sistema agora conta com uma **placa solar integrada à caixa 3D**, que permite o carregamento contínuo durante o dia, garantindo funcionamento autossustentável nos pátios.
- O módulo **TP4056** permanece responsável pelo gerenciamento de carga, oferecendo carregamento seguro e proteção contra sobrecarga e descarga excessiva.
- A caixa 3D foi adaptada para acomodar a placa solar e mantém proteção **IP54**, garantindo resistência à poeira e respingos de água, ideal para uso em ambientes externos.

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

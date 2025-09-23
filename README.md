# SISTEMA DE MONITORAMENTO DE MOTO - IOT

## DESCRIÇÃO DO PROJETO
Sistema completo de monitoramento e localização de motos utilizando ESP32, sensores IoT e dashboard web em tempo real. Desenvolvido para o projeto de Arquiteturas Disruptivas: IoT, IoB & Generative AI.

## DEMONSTRAÇÃO YOUTUBE
```text
https://youtu.be/WHP_xYQ4_fY
```

## FUNCIONALIDADES PRINCIPAIS

### 🔄 MONITORAMENTO EM TEMPO REAL
- Sensor PIR de movimento  
- Status de bateria simulado  
- Atualização contínua via MQTT  

### 📡 CONTROLE REMOTO
- Localização sonora/visual da moto (buzzer + LED)  
- Dashboard web responsivo  
- Comandos em tempo real  

### 💾 PERSISTÊNCIA DE DADOS
- Logs em arquivo texto (telemetry.log)  
- Histórico de telemetria  
- Comandos registrados  

### 🎨 INTERFACE MODERNA
- Dashboard com Tailwind CSS  
- Design responsivo  
- Animações e feedback visual  

## GUIA DE INSTALAÇÃO PASSO A PASSO

### PRÉ-REQUISITOS
- Node.js (v16 ou superior)  
- Python (para PlatformIO)  
- Visual Studio Code com extensão PlatformIO  
- Conta Wokwi (para simulação)  

### PASSO 1: CONFIGURAR O ESP32 (FIRMWARE)

**Opção A: Simulação no Wokwi**
1. Acesse Wokwi.com  
2. Crie novo projeto ESP32  
3. Cole o código do firmware/src/main.cpp  
4. Configure o diagrama conforme especificado  

**DIAGRAMA DE CONEXÕES**
- PIR Sensor → GPIO 13  
- Buzzer → GPIO 4  
- NeoPixel LED → GPIO 2  
- Botão → GPIO 12 (INPUT_PULLUP)  

### PASSO 2: CONFIGURAR O DASHBOARD
```bash
# Navegue até a pasta do dashboard
cd dashboard

# Instale as dependências
npm install

# Inicie o servidor
npm start
```

**SAÍDA ESPERADA**
```text
- 🚀 Dashboard rodando na porta 3000  
- 📊 Acesse: http://localhost:3000  
- 📝 Logs serão salvos em: telemetry.log  
- ✅ Conectado ao MQTT Broker
```

### PASSO 3: ACESSAR O SISTEMA
- Dashboard Principal: http://localhost:3000  
- Visualizar Logs: http://localhost:3000/logs  
- API de Logs: http://localhost:3000/api/logs  

## CONFIGURAÇÃO DETALHADA

**FIRMWARE ESP32 (firmware/src/main.cpp)**  
Configurações de Rede:  
- `const char* ssid = "Wokwi-GUEST";`  
- `const char* password = "";`  

Configurações MQTT:  
- `const char* mqtt_server = "broker.hivemq.com";`  
- `const int mqtt_port = 1883;`  

**DASHBOARD (dashboard/server.js)**  
- `const PORT = 3000;`  
- `const MQTT_BROKER = "mqtt://broker.hivemq.com";`  

## CASOS DE USO E TESTES

### CASO 1: LOCALIZAÇÃO DA MOTO
- **Ação:** Clique em "Localizar Moto" no dashboard  
- **Resultado:** Buzzer pisca, LED amarelo, status "LOCALIZANDO"  

### CASO 2: DETECÇÃO DE MOVIMENTO
- **Ação:** Ative o sensor PIR (simule movimento)  
- **Resultado:** Dashboard mostra "MOVIMENTO DETECTADO", log registra evento  

### CASO 3: MONITORAMENTO DE BATERIA
- **Ação:** Observe a barra de bateria no dashboard  
- **Resultado:** Valor atualizado a cada 5 segundos, logs mostram histórico  

### CASO 4: CONTROLE POR BOTÃO FÍSICO
- **Ação:** Pressione o botão no ESP32  
- **Resultado:** Desativa alarmes, LED volta para verde  

## ESTRUTURA DE DADOS

**TELEMETRIA PUBLICADA (ESP32 → DASHBOARD)**
```json
{
  "motion": true,
  "locating": false,
  "battery": 85,
  "timestamp": 1234567890
}
```

**COMANDOS ENVIADOS (DASHBOARD → ESP32)**
- LOCATE_ON: Ativa modo localização  
- LOCATE_OFF: Desativa modo localização  
- ALARM_OFF: Para todos os sistemas  

**FORMATO DO LOG (telemetry.log)**
```log
2024-01-15T10:30:00.000Z | Moto: 001 | Movimento: SIM | Localizando: NÃO | Bateria: 85%
2024-01-15T10:30:05.000Z | Moto: 001 | Movimento: NÃO | Localizando: SIM | Bateria: 84%
```

## SOLUÇÃO DE PROBLEMAS

**ESP32 NÃO CONECTA AO WIFI**
- Verifique credenciais WiFi  
- Wokwi: "Wokwi-GUEST" sem senha  
- Rede real: Use suas credenciais  

**DASHBOARD NÃO RECEBE DADOS**
- Verifique se servidor está rodando (npm start)  
- Confirme se ESP32 está publicando no MQTT  
- Verifique console do navegador (F12)  

**BUZZER NÃO EMITE SOM**
- Wokwi: Buzzer é simulado visualmente  
- ESP32 físico: Verifique conexões e código  

**LOGS NÃO SÃO SALVOS**
- Comando: `ls -la dashboard/`  
- Teste: `node -e "require('fs').appendFileSync('test.log', 'test')"`  

## MONITORAMENTO E DEBUG

**ENDPOINTS DA API**
- GET /api/status - Status atual da moto  
- GET /api/logs - Histórico de logs  
- POST /api/command - Enviar comando  

**EXEMPLO DE USO DA API**
```javascript
fetch('/api/command', {
  method: 'POST',
  headers: {'Content-Type':'application/json'},
  body: JSON.stringify({command: 'LOCATE_ON'})
})
```

**LOGS DO SISTEMA**
```bash
tail -f dashboard/telemetry.log
wc -l dashboard/telemetry.log
```

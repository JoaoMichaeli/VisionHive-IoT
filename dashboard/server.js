const express = require('express');
const http = require('http');
const socketIo = require('socket.io');
const mqtt = require('mqtt');
const fs = require('fs');
const path = require('path');

const app = express();
const server = http.createServer(app);
const io = socketIo(server);

const LOG_FILE = path.join(__dirname, 'telemetry.log');

const mqttClient = mqtt.connect('mqtt://broker.hivemq.com');

let motoData = {
  motion: false,
  locating: false,
  battery: 100,
  lastUpdate: new Date()
};

function saveToLog(data) {
  const logEntry = `${new Date().toISOString()} | Moto: 001 | Movimento: ${data.motion ? 'SIM' : 'NÃO'} | Localizando: ${data.locating ? 'SIM' : 'NÃO'} | Bateria: ${data.battery}%\n`;
  
  fs.appendFile(LOG_FILE, logEntry, (err) => {
    if (err) {
      console.error('❌ Erro ao salvar log:', err);
    } else {
      console.log('💾 Dados salvos no log');
    }
  });
}

app.use(express.static('public'));
app.use(express.json());

mqttClient.on('connect', () => {
  console.log('✅ Conectado ao MQTT Broker');
  mqttClient.subscribe('moto/001/telemetry');
});

mqttClient.on('message', (topic, message) => {
  try {
    const data = JSON.parse(message.toString());
    console.log('📨 Dados recebidos via MQTT:', data);
    
    motoData = { 
      ...motoData, 
      ...data, 
      lastUpdate: new Date() 
    };
    
    saveToLog(data);
    
    io.emit('motoData', motoData);
    
  } catch (error) {
    console.error('❌ Erro ao processar mensagem MQTT:', error);
  }
});

app.post('/api/command', (req, res) => {
  const { command } = req.body;
  
  if (!command) {
    return res.status(400).json({ error: 'Comando não especificado' });
  }
  
  mqttClient.publish('moto/001/commands', command);
  console.log('📤 Comando enviado para a moto:', command);
  
  const commandLog = `${new Date().toISOString()} | COMANDO: ${command}\n`;
  fs.appendFile(LOG_FILE, commandLog, () => {});
  
  res.json({ success: true, command: command });
});

app.get('/api/logs', (req, res) => {
  fs.readFile(LOG_FILE, 'utf8', (err, data) => {
    if (err) {
      res.json({ 
        success: false, 
        message: 'Arquivo de log vazio ou não criado ainda',
        logs: [] 
      });
    } else {
      const logs = data.split('\n').filter(line => line.trim() !== '');
      res.json({ 
        success: true, 
        total: logs.length,
        logs: logs 
      });
    }
  });
});

app.get('/api/status', (req, res) => {
  res.json(motoData);
});

app.get('/', (req, res) => {
  res.sendFile(__dirname + '/public/index.html');
});

app.get('/logs', (req, res) => {
  res.send(`
    <html>
      <head><title>Logs do Sistema</title></head>
      <body>
        <h1>📝 Logs do Sistema Moto IoT</h1>
        <button onclick="loadLogs()">Carregar Logs</button>
        <div id="logs"></div>
        <script>
          async function loadLogs() {
            const response = await fetch('/api/logs');
            const data = await response.json();
            document.getElementById('logs').innerHTML = 
              '<pre>' + (data.logs ? data.logs.join('\\n') : 'Nenhum log encontrado') + '</pre>';
          }
        </script>
      </body>
    </html>
  `);
});

server.listen(3000, () => {
  console.log('🚀 Dashboard rodando na porta 3000');
  console.log('📊 Acesse: http://localhost:3000');
  console.log('📝 Logs serão salvos em: telemetry.log');
  console.log('📋 Ver logs em: http://localhost:3000/logs');
});